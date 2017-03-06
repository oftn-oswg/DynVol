/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 OFTN Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include "util.h"
#include "logging.h"
#include "io.h"
#include "dynvol.h"
#include "dynvol_private.h"
#include <glib.h>
#include <gio/gio.h>

gchar* readpart(struct volio *volio, const goffset offset, const guint64 bytes)
{
    /* rework this to return an error somehow
     * (or just don't use it...)
     */

    gchar* ret;
    log_moredebug("Allocating memory...");
    ret = g_malloc0(sizeof(gchar)*bytes);
    readinto(volio, offset, bytes, (gpointer)ret);
    return ret;
}

vol_err_t readbyte(struct volio *volio, const goffset offset, guint8 *byte)
{
    log_moredebug("Reading byte at offset 0x%lx.", offset);
    (*byte) = 0x00;
    gsize rd;
    GError *error = NULL;
    log_moredebug("Seeking...");
    if (!g_seekable_seek(G_SEEKABLE(volio->readstream), offset, G_SEEK_SET,
                         NULL, &error))
    {
        log_critical("Seek failed:\t%s", error->message);
        return VOL_ERR_SEEK;
    }
    log_moredebug("Reading...");
    if(!g_input_stream_read_all(G_INPUT_STREAM(volio->readstream), byte,
                                (gsize)sizeof(guint8), &rd, NULL, &error))
    {
        log_critical("Could not read from stream:\t%s", error->message);
        log_debug("Read %lu bytes before error.", rd);
        return VOL_ERR_READ;
    } else if (rd == 0) {
        log_critical("Stream has already ended. offset 0x%lx not reached.",
                     offset);
        return VOL_ERR_EOF;
    } else if (rd != (gsize)sizeof(guint8)) {
        log_message("Reached end of stream. Read %lu bytes.", rd);
    } else {
        log_moredebug("Read %lu bytes.", rd);
    }
    return VOL_ERR_OK;
}

vol_err_t readinto(struct volio *volio, const goffset offset, const gsize size,
                  gpointer container)
{
    log_debug("Reading %lu bytes at offset 0x%lx.", size, offset);
    gsize rd;
    GError *error = NULL;
    log_moredebug("Seeking...");
    if (!g_seekable_seek(G_SEEKABLE(volio->readstream), offset, G_SEEK_SET,
                         NULL, &error))
    {
        log_critical("Seek failed:\t%s", error->message);
        return VOL_ERR_SEEK;
    }
    log_moredebug("Reading...");
    if(!g_input_stream_read_all(G_INPUT_STREAM(volio->readstream), container,
                                size, &rd, NULL, &error))
    {
        log_critical("Could not read from stream:\t%s", error->message);
        log_debug("Read %lu bytes before error.", rd);
        return VOL_ERR_READ;
    } else if (rd == 0) {
        log_critical("Stream has already ended. offset 0x%lx not reached.",
                     offset);
        return VOL_ERR_EOF;
    } else if (rd != size) {
        log_message("Reached end of stream. Read %lu bytes.", rd);
    } else {
        log_moredebug("Read %lu bytes.", rd);
    }
    return VOL_ERR_OK;
}

/* This is the beginnings of our extraction function.
 * Right now we're lucky if we can extract an uncompressed file.
 * Since I haven't really found any GIO example C code online anywhere, this is
 * probably going to get very nasty, at least for now.
 * Je ne regrette rien.
 *
 * For now we will simply extract to the working directory. We can deal with
 * folders and such later.
 */
vol_err_t copyout(struct vfile *vfile)
{
    struct volio *volio = &vfile->vol->volio;
    struct rleio fileio;
    GError *error = NULL;
    gchar *path = vfile->name;
    gchar buffer[512];
    guint32 i, v_offset, f_offset = 0;
    gsize rt, rd, buffsize;
    if (g_file_test (path, G_FILE_TEST_EXISTS))
    {
        log_warning("Destination file %s already exists..", path);
        return VOL_ERR_EXISTS;
    }
    log_info("Attempting to open %s for writing.", path);

    fileio.identifier = g_file_new_for_path(path);
    fileio.writestream = g_file_create(fileio.identifier, G_FILE_CREATE_NONE,
                                       NULL, &error);
    if (!fileio.writestream)
    {
        log_error("Failed opening %s for writing.\n\t%s", path, error->message);
        return VOL_ERR_OPEN;
    }

    v_offset = vfile->b_offset + sizeof(struct header);

    log_debug("Seeking to offset 0x%lx in volume.", v_offset);

    if (!g_seekable_seek(G_SEEKABLE(volio->readstream), v_offset, G_SEEK_SET,
                         NULL, &error))
    {
        log_critical("Seek failed:\t%s", error->message);
        return VOL_ERR_SEEK;
    }
    for (i=0; i < vfile->packed_size; i+=512)
    {
        if ((vfile->packed_size - i) > 512)
            buffsize = 512;
        else
            buffsize = vfile->packed_size - i;

        //log_moredebug("Reading...");
        log_moredebug("Attempting to read %lu bytes.", buffsize);
        if(!g_input_stream_read_all(G_INPUT_STREAM(volio->readstream), &buffer,
                                    buffsize, &rd, NULL, &error))
        {
            log_critical("Could not read from stream:\t%s", error->message);
            log_debug("Read %lu bytes before error.", rd);
            return VOL_ERR_READ;
        } else if (rd == 0) {
            log_critical("Stream has already ended.");
            return VOL_ERR_EOF;
        } else if (rd != buffsize) {
            log_message("Reached end of stream. Read %lu bytes.", rd);
        } else {
            log_moredebug("Read %lu bytes.", rd);
        }
        log_moredebug("Writing...");
        if(!g_output_stream_write_all(G_OUTPUT_STREAM(fileio.writestream),
                                      &buffer, rd, &rt, NULL, &error))
        {
            log_critical("Could not write to file:\t%s", error->message);
            log_debug("Wrote %u bytes before error.", rt);
            return VOL_ERR_READ;
        } else if (rt != buffsize) {
            log_message("Reached end of stream. Wrote %u bytes.", rt);
        } else {
            log_moredebug("Wrote %lu bytes.", rt);
        }
    }
    if (!(g_output_stream_close(G_OUTPUT_STREAM(fileio.writestream), NULL,
                                &error)))
        log_critical("Couldn't close output stream:\t%s", error->message);

    log_debug("Freeing writestream");
    g_object_unref(fileio.writestream);
    log_debug("Freeing file identifier");
    g_object_unref(fileio.identifier);

    return VOL_ERR_OK;
}
