/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 SwooshyCueb
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#include <string.h>
#include "util.h"
#include "dynvol.h"
#include "dynvol_private.h"
#include <glib.h>



gchar* sanitizepath(const gchar* dirtypath)
{
	gchar* cleanpath;
	gint i;
	cleanpath = g_malloc(sizeof(gchar)*(strlen(dirtypath)));
	for (i=0; i <= strlen(dirtypath); i++)
	{
		if (dirtypath[i] == 0x5C)
		{
			cleanpath[i] = 0x2F;
		} else {
			cleanpath[i] = dirtypath[i];
		}
	}
	return cleanpath;
}

void logfunc(const gchar *domain, GLogLevelFlags level, const gchar *message, gpointer misc)
{
	void (*print)();
	if (level == G_LOG_LEVEL_INFO || level == G_LOG_LEVEL_DEBUG)
		print = (gpointer)g_print;
	else
		print = (gpointer)g_printerr;
	print("%s %s ", domain, g_date_time_format(g_date_time_new_now_local (), "%T"));
	switch(level)
	{
		case G_LOG_LEVEL_ERROR:
			print("   \033[1;31m[ERROR]\033[0m ");
			break;
		case G_LOG_LEVEL_CRITICAL:
			print("\033[38;5;208;1m[CRITICAL]\033[0m ");
			break;
		case G_LOG_LEVEL_WARNING:
			print(" \033[33;1m[WARNING]\033[0m ");
			break;
		case G_LOG_LEVEL_MESSAGE:
			print(" \033[38;5;179;1m[MESSAGE]\033[0m ");
			break;
		case G_LOG_LEVEL_INFO:
			print("    \033[1m[INFO]\033[0m ");
			break;
		case G_LOG_LEVEL_DEBUG:
			print("   \033[1m[DEBUG]\033[0m ");
			break;
		}
		print("%s\n", message);
}

void printfiles(gpointer name, gpointer count)
{
	(*(guint*)count)++;
	log_info("\tFile %u: %s", (*(guint*)count), (gchar*)name);
}

gchar* readpart(GIOChannel *stream, const guint64 offset, const guint64 bytes)
{
	//rework this to return an error somehow
	//(or just don't use it...)

	gchar* ret;
	log_debug("Allocating memory...");
	ret = g_malloc0(sizeof(gchar)*bytes);
	readinto(stream, offset, bytes, (gpointer)ret);
	return ret;
}

VErrcode readbyte(GIOChannel *stream, const guint64 offset, guint8 *byte)
{
	//log_debug("Reading byte at offset 0x%lx.", offset);
	//Needs superverbose debug
	(*byte) = 0x00;
	guint64 rd;
	GError *error = NULL;
	GIOStatus opstat = G_IO_STATUS_NORMAL;
	//log_debug("Seeking...");
	//Needs superverbose debug
	while ((opstat = g_io_channel_seek_position(stream, offset, G_SEEK_SET, &error)) == G_IO_STATUS_AGAIN)
		log_message("Resource unavaliable! Will retry.");
	if (opstat == G_IO_STATUS_EOF)
	{
		log_critical("Stream ends before offset 0x%lx.", offset);
		return VERR_UNEXPECTED_EOF;
	} else if (opstat != G_IO_STATUS_NORMAL) {
		log_critical("Seek failed:\t%s", error->message);
		return VERR_SEEK_FAILED;
	}
	//log_debug("Reading...");
	//Needs superverbose debug
	while ((opstat = g_io_channel_read_chars(stream, byte, (gsize)sizeof(guint8), &rd, &error)) == G_IO_STATUS_AGAIN)
		log_message("Resource unavaliable! Will retry.");
	if (opstat == G_IO_STATUS_EOF)
	{
		log_message("Reached end of stream. Read %lu bytes.", rd);
	} else if (opstat != G_IO_STATUS_NORMAL) {
		log_critical("Could not read from stream:\t%s", error->message);
		return VERR_READ_FAILED;
	//} else {
	//	log_debug("Read %lu bytes.", rd);
	//Needs superverbose debug
	}
	return VERR_OK;
}

VErrcode readinto(GIOChannel *stream, const guint64 offset, const gsize size, gpointer container)
{
	log_debug("Reading %lu bytes at offset 0x%lx.", size, offset);
	guint64 rd;
	GError *error = NULL;
	GIOStatus opstat = G_IO_STATUS_NORMAL;
	log_debug("Seeking...");
	while ((opstat = g_io_channel_seek_position(stream, offset, G_SEEK_SET, &error)) == G_IO_STATUS_AGAIN)
		log_message("Resource unavaliable! Will retry.");
	if (opstat == G_IO_STATUS_EOF)
	{
		log_critical("Stream ends before offset 0x%lx.", offset);
		return VERR_UNEXPECTED_EOF;
	} else if (opstat != G_IO_STATUS_NORMAL) {
		log_critical("Seek failed:\t%s", error->message);
		return VERR_SEEK_FAILED;
	}
	log_debug("Reading...");
	while ((opstat = g_io_channel_read_chars(stream, (gchar*)container, size, &rd, &error)) == G_IO_STATUS_AGAIN)
		log_message("Resource unavaliable! Will retry.");
	if (opstat == G_IO_STATUS_EOF)
	{
		log_message("Reached end of stream. Read %lu bytes.", rd);
	} else if (opstat != G_IO_STATUS_NORMAL) {
		log_critical("Could not read from stream:\t%s", error->message);
		return VERR_READ_FAILED;
	} else {
		log_debug("Read %lu bytes.", rd);
	}
	return VERR_OK;
}