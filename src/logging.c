/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 SwooshyCueb
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include "dynvol.h"
#include "dynvol_private.h"
#include "logging.h"

void logfunc(const gchar *domain, GLogLevelFlags level, const gchar *message, gpointer misc)
{
	void (*print)();
	if (level == VOL_LOG_LEVEL_INFO || level == VOL_LOG_LEVEL_DEBUG || level == VOL_LOG_LEVEL_MOREDEBUG)
		print = (gpointer)g_print;
	else
		print = (gpointer)g_printerr;
	print("%s %s ", domain, g_date_time_format(g_date_time_new_now_local (), "%T"));
	switch(level & (~(G_LOG_FLAG_RECURSION|G_LOG_FLAG_FATAL)))
	{
		case VOL_LOG_LEVEL_ERROR:
			print("   \033[1;31m[ERROR]\033[0m ");
			break;
		case VOL_LOG_LEVEL_CRITICAL:
			print("\033[38;5;208;1m[CRITICAL]\033[0m ");
			break;
		case VOL_LOG_LEVEL_WARNING:
			print(" \033[33;1m[WARNING]\033[0m ");
			break;
		case VOL_LOG_LEVEL_MESSAGE:
			print(" \033[38;5;179;1m[MESSAGE]\033[0m ");
			break;
		case VOL_LOG_LEVEL_INFO:
			print("    \033[1m[INFO]\033[0m ");
			break;
		case VOL_LOG_LEVEL_DEBUG:
			print("   \033[38;5;249;1m[DEBUG]\033[0m ");
			break;
		case VOL_LOG_LEVEL_TODO:
			print("    \033[38;5;43;1m[TODO]\033[0m ");
			break;
		case VOL_LOG_LEVEL_FIXME:
			print("   \033[38;5;107;1m[FIXME]\033[0m ");
			break;
		case VOL_LOG_LEVEL_MOREDEBUG:
			print("  \033[38;5;240;1m[DEBUG2]\033[0m ");
			break;
		}
		print("%s\n", message);
}
