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

