/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 OFTN Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _GNU_SOURCE
 #define _GNU_SOURCE
#endif

#include <string.h>
#include <glib.h>
#include "dynvol.h"
#include "dynvol_private.h"
#include "util.h"
#include "logging.h"
#include "io.h"
#include <gio/gio.h>
#include <libgen.h>

vol_t vol_open(const gchar* path)
{
    GError *error = NULL;
    GIOStatus ret = G_IO_STATUS_NORMAL;
    struct volume* handle;

    log_info("Loading volume %s.", path);

    log_debug("Allocating memory for volume data container.");
    handle=g_malloc(sizeof(struct volume));
    log_todo("Figure out how to return errors from vol_load");

    /*
     * Variable initialization
     */

    /* Volume and handle info */
    handle->path = NULL;
    handle->files = NULL;
    handle->open = FALSE;
    handle->writable = FALSE;
    handle->error = VERR_OK;

    /* Volume io */
    handle->volio.identifier = NULL;
    handle->volio.readstream = NULL;

    /* Volume header */
    memset(&handle->header.ident, 0, sizeof(gchar[4]));
    handle->header.val = 0;

    /* Volume footer array header identifiers */
    memset(&handle->footer.unknown_vstr.header.ident, 0, sizeof(gchar[4]));
    memset(&handle->footer.unknown_vval.header.ident, 0, sizeof(gchar[4]));
    memset(&handle->footer.filenames.header.ident, 0, sizeof(gchar[4]));
    memset(&handle->footer.fileprops.header.ident, 0, sizeof(gchar[4]));

    /* Volume footer array values */
    handle->footer.unknown_vstr.header.val = 0;
    handle->footer.unknown_vval.header.val = 0;
    handle->footer.filenames.header.val = 0;
    handle->footer.fileprops.header.val = 0;

    /* Volume footer array contents */
    handle->footer.unknown_vstr.data = NULL;
    handle->footer.unknown_vval.data = NULL;
    handle->footer.filenames.data = NULL;
    handle->footer.fileprops.data = NULL;

    /* Volume footer array offsets */
    handle->footer.unknown_vstr.offset = 0;
    handle->footer.unknown_vval.offset = 0;
    handle->footer.filenames.offset = 0;
    handle->footer.fileprops.offset = 0;

    handle->path = g_strdup(path);

    /* We're just going to make the assumption that the file isn't going to
     * change between testing and use.
     */
    if (!g_file_test (path, G_FILE_TEST_EXISTS))
    {
        log_critical("Volumme %s does not exist. Creation of new volumes not yet supported.",
                     path);
        handle->error = VERR_FILE_NOT_FOUND;
        return (gpointer)handle;
    } else if (!g_file_test (path, G_FILE_TEST_IS_REGULAR)) {
        log_error("Cannot open %s.", path);
        handle->error = VERR_UNKNOWN;
        return (gpointer)handle;
    }
    log_debug("Attempting to open %s", path);

    /* Just to be safe, we will only open the file for writing when we're
     * actually writing to it.
     * I found out the hard way if you screw something up just right while the
     * file is open for writing, bad things happen.
     * We don't want bad things to happen.
     * This archive format is broken enough as it is...
     */
    handle->volio.identifier = g_file_new_for_path(path);
    handle->volio.readstream = g_file_read(handle->volio.identifier, NULL,
                                           &error);
    if (!handle->volio.readstream)
    {
        log_error("Failed opening %s\n\t%s", path, error->message);
        handle->error = VERR_UNKNOWN;
        return (gpointer)handle;
    }
    handle->open = TRUE;

    /* Code left over from using glib io
     * We may need to convert it at some point
     */
    /*
    log_debug("Setting encoding to NULL.");
    ret = g_io_channel_set_encoding(handle->volio, NULL, &error);
    if (ret != G_IO_STATUS_NORMAL)
    {
    	log_warning("Failed setting encoding for %s\n\t%s", path,
    				error->message);
    }
    */

    handle->error = vol_getmetadata((gpointer)handle);
    return (gpointer)handle;
}


void vol_close(vol_t handle)
{
    log_todo("Figure out how to return errors from vol_unload");
    struct volume* vhnd = handle;
    log_info("Unoading volume %s.", vhnd->path);
    GIOStatus ret = G_IO_STATUS_NORMAL;
    GError *error = NULL;

    if (!(g_input_stream_close(vhnd->volio.readstream, NULL, &error)))
        log_critical("Couldn't close input stream:\t%s", error->message);

    log_debug("Freeing readstream");
    g_object_unref(vhnd->volio.readstream);
    log_debug("Freeing file identifier");
    g_object_unref(vhnd->volio.identifier);

    log_debug("Freeing memory.");

    log_debug("Freeing unknown_vstr.");
    if (vhnd->footer.unknown_vstr.data != NULL)
        g_ptr_array_free(vhnd->footer.unknown_vstr.data, TRUE);

    log_debug("Freeing unknown_vval.");
    if (vhnd->footer.unknown_vval.data != NULL)
        g_free(g_array_free(vhnd->footer.unknown_vval.data, TRUE));

    log_debug("Freeing filenames.");
    if (vhnd->footer.filenames.data != NULL)
        g_ptr_array_free(vhnd->footer.filenames.data, TRUE);

    log_debug("Freeing fileprops.");
    if (vhnd->footer.fileprops.data != NULL)
        g_free(g_array_free(vhnd->footer.fileprops.data, TRUE));

    log_debug("Freeing files.");
    if (vhnd->files != NULL)
        g_free(g_ptr_array_free(vhnd->files, TRUE));

    log_debug("Freeing path.");
    g_free(vhnd->path);

    log_debug("Freeing handle.");
    g_free(vhnd);

}

VErrcode vol_getheader(vol_t handle, struct header *header, const guint32 offset)
{
    struct volume* vhnd = handle;
    log_info("Fetching header");
    VErrcode err = readinto(&vhnd->volio, offset,
                            (gsize)sizeof(struct header), (gpointer)header);
    log_debug("Got header:");
    log_debug("\tIDstring: %c%c%c%c", header->ident[0], header->ident[1],
              header->ident[2], header->ident[3]);
    log_debug("\tValue: 0x%x", header->val);
    return err;
}

VErrcode vol_getfooter(vol_t handle)
{
    VErrcode err;
    struct volume* vhnd = handle;
    log_info("Fetching volume footer.");

    if (vhnd->format == VFMT_STARSIEGE)
    {
        vhnd->footer.unknown_vstr.offset = vhnd->header.val;

        /* TODO: figure out what these are for
         * TODO: figure out if content format varies from the other arrays
         */
        err = vol_getvstr(handle);
        if (err)
            return err;

        err = vol_getvval(handle);
        if (err)
            return err;
    } else if (vhnd->format == VFMT_TRIBES) {
        vhnd->footer.filenames.offset = vhnd->header.val;
    }


    err = vol_getfilenames(handle);
    if (err)
        return err;
    return vol_getfileprops(handle);
}

VErrcode vol_getmetadata(vol_t handle)
{
    struct volume* vhnd = handle;
    log_info("Fetching volume metadata.");
    VErrcode err = vol_getheader(handle, &vhnd->header, 0);
    if (err)
        return err;

    /* Header verification
     * There might be a memory leak here
     */

    if (memcmp(" VOL", vhnd->header.ident, 4) == 0)
    {
        log_debug("Magic number recognized. Archive is Starsiege volume.");
        vhnd->format = VFMT_STARSIEGE;
    } else if (memcmp("PVOL", vhnd->header.ident, 4) == 0) {
        log_debug("Magic number recognized. Archive is Tribes 1 volume.");
        vhnd->format = VFMT_TRIBES;
    } else if (memcmp("VOLN", vhnd->header.ident, 4) == 0) {
        log_debug("Magic number recognized. Archive is Earthsiege/Earthsiege 2 volume.");
        log_critical("Currently, only Starsiege and Starsiege Tribes volumes are supported.");
        return VERR_UNSUPPORTED_ARCHIVE;
    } else {
        log_critical("Magic number not recognized.");
        return VERR_ARCHIVE_UNRECOGNIZED;
    }

    return vol_getfooter(handle);
}

VErrcode vol_getvstr(vol_t handle)
{
    guint i;
    VErrcode err;
    /* gets header only */
    struct volume* vhnd = handle;
    log_info("Fetching string array.");
    log_debug("Scraping array header.");

    for (i=0; i <=4; i++)
    {
        err = vol_getheader(handle, &vhnd->footer.unknown_vstr.header,
                                     vhnd->footer.unknown_vstr.offset + i);
        if (err)
            return err;

        /* Header verification
         * There might be a memory leak here
         */
        if (memcmp("vols", vhnd->footer.unknown_vstr.header.ident, 4))
        {
            log_debug("Retrieved IDstring does not matched needed IDstring.");
            err = VERR_UNRRECOGNIZED_HEADER;
        } else {
            vhnd->footer.unknown_vstr.offset += i;
            err = VERR_OK;
            break;
        }
    }
    if (err == VERR_UNRRECOGNIZED_HEADER)
    {
        log_warning("Array identity string not recognized.");
        err = VERR_OK;
        /* Not setting error here, at least not yet */
    }
    log_info("Skipping contents of unknown array.");
    vhnd->footer.unknown_vstr.data = NULL;
    vhnd->footer.unknown_vval.offset = vhnd->footer.unknown_vstr.offset+sizeof(
                                           struct header)+vhnd->footer.unknown_vstr.header.val;
    return err;
}

VErrcode vol_getvval(vol_t handle)
{
    guint i;
    VErrcode err;
    /* gets header only */
    struct volume* vhnd = handle;
    log_info("Fetching value set array.");
    log_debug("Scraping array header.");
    for (i=0; i <=4; i++)
    {
        err = vol_getheader(handle, &vhnd->footer.unknown_vval.header,
                                     vhnd->footer.unknown_vval.offset + i);
        if (err)
            return err;

        /* Header verification
         * There might be a memory leak here
         */
        if (memcmp("voli", vhnd->footer.unknown_vval.header.ident, 4))
        {
            log_debug("Retrieved IDstring does not matched needed IDstring.");
            err = VERR_UNRRECOGNIZED_HEADER;
        } else {
            vhnd->footer.unknown_vval.offset += i;
            err = VERR_OK;
            break;
        }
    }
    if (err == VERR_UNRRECOGNIZED_HEADER)
    {
        log_critical("Array identity string not recognized.");
        err = VERR_OK;
        /* Not setting error here, at least not yet */
    }
    log_info("Skipping contents of unknown array.");
    vhnd->footer.unknown_vval.data = NULL;
    vhnd->footer.filenames.offset = vhnd->footer.unknown_vval.offset+sizeof(
                                        struct header)+vhnd->footer.unknown_vval.header.val;
    return err;
}

VErrcode vol_getfilenames(vol_t handle)
{
    struct volume* vhnd = handle;
    struct vfile *curfile;
    log_info("Fetching filenames from array.");
    guint64 os;
    os = vhnd->footer.filenames.offset;
    gint i, j = 0, l = 0;
    VErrcode err;
    gchar* datadir;
    guint8 bite;
    log_debug("Scraping array header.");
    for (i=0; i <=4; i++)
    {
        err = vol_getheader(handle, &vhnd->footer.filenames.header,
                                     vhnd->footer.filenames.offset + i);
        if (err)
            return err;

        /* Header verification
         * There might be a memory leak here
         */
        if (memcmp("vols", vhnd->footer.filenames.header.ident, 4))
        {
            log_debug("Retrieved IDstring does not matched needed IDstring.");
            err = VERR_UNRRECOGNIZED_HEADER;
        } else {
            vhnd->footer.filenames.offset += i;
            err = VERR_OK;
            break;
        }
    }
    if (err == VERR_UNRRECOGNIZED_HEADER)
    {
        log_critical("Array identity string not recognized.");
        return VERR_BROKEN_ARCHIVE;
    }
    if (vhnd->footer.filenames.header.val != 0)
    {
        os+=sizeof(struct header);
        vhnd->footer.filenames.data = g_ptr_array_new_with_free_func(g_free);
        vhnd->files = g_ptr_array_new_with_free_func(vol_filesarray_free);
        log_debug("Fetching array data.");
        for (i = 0; i < vhnd->footer.filenames.header.val; i++)
        {
            if (j == 0)
                log_debug("Seeking to end of string.");
            j++;
            readbyte(&vhnd->volio, (guint64)(os+j), &bite);
            if (bite == 0x00)
            {
                j++;
                log_debug("String at offset 0x%lx indexed. Size is %d. Pulling now.", os, j);
                gchar *data, *sanitary, *pathcopy1, *pathcopy2;
                curfile = (struct vfile*)g_malloc(sizeof(struct vfile));
                curfile->data.data = NULL;
                data = readpart(&vhnd->volio, (guint64)os, sizeof(gchar)*j);
                sanitary = sanitizepath(data);
                log_debug("Pulled string: %s.", data);
                i++;
                os+=j;
                j=0;
                log_debug("Adding string to arrays.");
                pathcopy1 = g_strdup(sanitary);
                pathcopy2 = g_strdup(sanitary);
                g_ptr_array_add(vhnd->footer.filenames.data, (gpointer)data);
                curfile->path = g_strdup(data);
                curfile->path_canonical = sanitary;
                curfile->name = g_strdup(basename(pathcopy1));
                curfile->dir = g_strdup(dirname(pathcopy2));
                g_free(pathcopy1);
                g_free(pathcopy2);
                g_ptr_array_add(vhnd->files, (gpointer)curfile);
                l++;
            } else if (bite == 0x5C) {
                log_todo("Warn windows users about invalid characters and forward slashes");
                log_warning("An internal path string may contain backslashes.");
            }
        }
        struct counter* k;
        k = g_malloc0(sizeof(struct counter));
        k->count = 0;
        log_info("File arrays created.");
        g_ptr_array_foreach(vhnd->files, printfiles, (gpointer)k);
        g_free(k);
    } else {
        log_message("Array is empty. (No files in volume?)");
        vhnd->footer.filenames.data = NULL;
    }
    vhnd->footer.fileprops.offset = vhnd->footer.filenames.header.val
                                    +vhnd->footer.filenames.offset+sizeof(struct header);
    return err;
}

VErrcode vol_getfileprops(vol_t handle)
{
    struct volume* vhnd = handle;
    struct vfile *vfile;
    log_info("Fetching file properties array.");
    guint64 os;
    os = vhnd->footer.fileprops.offset;
    gint i;
    guint c;
    VErrcode err;
    guint o;
    log_debug("Scraping array header.");
    for (i=0; i <=4; i++)
    {
        err = vol_getheader(handle, &vhnd->footer.fileprops.header,
                                     vhnd->footer.fileprops.offset + i);
        if (err)
            return err;

        /* Header verification
         * There might be a memory leak here
         */
        if (memcmp("voli", vhnd->footer.fileprops.header.ident, 4))
        {
            log_debug("Retrieved IDstring does not matched needed IDstring.");
            err = VERR_UNRRECOGNIZED_HEADER;
        } else {
            vhnd->footer.fileprops.offset += i;
            os += i;
            err = VERR_OK;
            break;
        }
    }
    if (err == VERR_UNRRECOGNIZED_HEADER)
    {
        log_critical("Array identity string not recognized.");
        err = VERR_OK;
        /* Not setting error here, at least not yet */
    }
    if (vhnd->footer.fileprops.header.val != 0)
    {
        os +=sizeof(struct header);
        vhnd->footer.fileprops.data = g_array_new(FALSE, TRUE,
                                      sizeof(struct vval));
        log_debug("Fetching array data.");
        for (i = 0; i < vhnd->footer.fileprops.header.val; i+=(sizeof(struct vval)))
        {
            struct vval *propset = g_malloc0(sizeof(struct vval));
            vfile = g_ptr_array_index(vhnd->files, o);
            log_debug("Pulling file struct from %p", (gpointer)vfile);
            o++;
            log_debug("Pulling value set.");
            err = readinto(&vhnd->volio, (guint64)(os+i),
                           (gsize)sizeof(struct vval), (gpointer)propset);
            if (err)
                return err;

            log_info("Scraping VBLK header.");
            for (c=0; c <=4; c++)
            {
                err = vol_getheader(handle, &vfile->data.header, propset->field_3 + c);
                if (err)
                    return err;

                /* Header verification
                 * There might be a memory leak here
                 */
                if (memcmp("VBLK", &vfile->data.header.ident, 4))
                {
                    log_debug("Retrieved IDstring does not matched needed IDstring.");
                    err = VERR_UNRRECOGNIZED_HEADER;
                } else {
                    propset->field_3 += c;
                    err = VERR_OK;
                    break;
                }
            }
            if (err == VERR_UNRRECOGNIZED_HEADER)
            {
                log_warning("Array identity string not recognized.");
                err = VERR_OK;
                /* Not setting error here, at least not yet */
            }

            log_info("Values pulled:");

            log_info("\t32-bit value 1:    0x%x\t(unknown)", propset->field_1);
            /* Not putting this one into the file struct until we know what
             * it's for
             */

            log_info("\t32-bit value 2:    0x%x\t(filename array offset)",
                     propset->field_2);
            vfile->n_offset = propset->field_2;

            log_info("\t32-bit value 3:    0x%x\t(file VBLK offset)",
                     propset->field_3);
            vfile->b_offset = propset->field_3;

            log_info("\t32-bit value 4:    0x%x\t(uncompressed filesize)",
                     propset->field_4);
            vfile->size = propset->field_4;

            log_info("\t8-bit value:       0x%x\t\t(compression)",
                     propset->endcap);
            if(propset->endcap != 0)
                vfile->compressed = TRUE;
            else
                vfile->compressed = FALSE;

            log_info("\tVBLK 24-bit value: 0x%x\t(compressed filesize)",
                     (vfile->data.header.val - 0x80000000));
            vfile->packed_size = (vfile->data.header.val - 0x80000000);

            g_array_append_val(vhnd->footer.fileprops.data, (*propset));
            g_free(propset);
        }
    } else {
        log_message("Array is empty. (No files in volume?)");
        vhnd->footer.fileprops.data = NULL;
    }
    return err;
}

struct volfilelist vol_get_filelist(vol_t handle, gboolean canonicalized)
{
    struct volume* vhnd = handle;
    struct volfilelist filelist;
    guint i;
    gchar *filename;
    filelist.filelist = g_malloc(vhnd->files->len * sizeof(gchar*));
    filelist.len = vhnd->files->len;
    for(i = 0; i < vhnd->files->len; i++)
    {
        struct vfile *file = (struct vfile*)g_ptr_array_index(vhnd->files, i);
        if (canonicalized)
            filelist.filelist[i] = g_strdup(file->path_canonical);
        else
            filelist.filelist[i] = g_strdup(file->path);
    }
    return filelist;
}

void vol_filesarray_free(gpointer file)
{
    struct vfile* vfile = file;
    file = NULL;
    if (vfile->data.data != NULL)
        g_free(vfile->data.data);
    if (vfile->path != NULL)
        g_free(vfile->path);
    if (vfile->path_canonical != NULL)
        g_free(vfile->path_canonical);
    if (vfile->name != NULL)
        g_free(vfile->name);
    if (vfile->dir != NULL)
        g_free(vfile->dir);
    if (vfile != NULL)
        g_free(vfile);
}

void vol_set_debug(guint mask)
{
    g_log_set_handler(G_LOG_DOMAIN, mask, logfunc, NULL);
    log_todo("Set default logging level.");
    log_todo("Potentially handle logging level per volume?");
}

void temp_vol_test_rleout(vol_t handle)
{
    struct volume* vhnd = handle;
    copyout(&vhnd->volio, (struct vfile*)g_ptr_array_index(vhnd->files, 0));
}
