/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 ΩF:∅ Working Group contributors
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
	//rework this to return an error somehow
	//(or just don't use it...)

	gchar* ret;
	log_moredebug("Allocating memory...");
	ret = g_malloc0(sizeof(gchar)*bytes);
	readinto(volio, offset, bytes, (gpointer)ret);
	return ret;
}

VErrcode readbyte(struct volio *volio, const goffset offset, guint8 *byte)
{
	log_moredebug("Reading byte at offset 0x%lx.", offset);
	(*byte) = 0x00;
	guint64 rd;
	GError *error = NULL;
	log_moredebug("Seeking...");
	if (!g_seekable_seek(volio->readstream, offset, G_SEEK_SET, NULL, &error))
	{
		log_critical("Seek failed:\t%s", error->message);
		return VERR_SEEK_FAILED;
	}
	log_moredebug("Reading...");
	if(!g_input_stream_read_all(volio->readstream, byte, (gsize)sizeof(guint8), &rd, NULL, &error))
	{
		log_critical("Could not read from stream:\t%s", error->message);
		log_debug("Read %lu bytes before error.", rd);
		return VERR_READ_FAILED;
	} else if (rd == 0) {
		log_critical("Stream has already ended. offset 0x%lx not reached.", offset);
		return VERR_UNEXPECTED_EOF;
	} else if (rd =! (gsize)sizeof(guint8)) {
		log_message("Reached end of stream. Read %lu bytes.", rd);
	} else {
		//log_moredebug("Read %lu bytes.", rd);
		log_fixme("Read function does not return proper number of bytes read.");
	}
	return VERR_OK;
}

VErrcode readinto(struct volio *volio, const goffset offset, const gsize size, gpointer container)
{
	log_debug("Reading %lu bytes at offset 0x%lx.", size, offset);
	guint64 rd;
	GError *error = NULL;
	log_moredebug("Seeking...");
	if (!g_seekable_seek(volio->readstream, offset, G_SEEK_SET, NULL, &error))
	{
		log_critical("Seek failed:\t%s", error->message);
		return VERR_SEEK_FAILED;
	}
	log_moredebug("Reading...");
	if(!g_input_stream_read_all(volio->readstream, container, size, &rd, NULL, &error))
	{
		log_critical("Could not read from stream:\t%s", error->message);
		log_debug("Read %lu bytes before error.", rd);
		return VERR_READ_FAILED;
	} else if (rd == 0) {
		log_critical("Stream has already ended. offset 0x%lx not reached.", offset);
		return VERR_UNEXPECTED_EOF;
	} else if (rd =! size) {
		log_message("Reached end of stream. Read %lu bytes.", rd);
	} else {
		//log_moredebug("Read %lu bytes.", rd);
		log_fixme("Read function does not return proper number of bytes read.");
	}
	return VERR_OK;
}
