/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 SwooshyCueb
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "dynvol.h"
#include "logging.h"
#include <stdio.h>
#include <glib.h>
#include <unistd.h>
#include <string.h>

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif
#define G_LOG_DOMAIN "vtList"

int printhelp()
{
	g_print("Usage:\n");
	g_print("    vtList [options] expression file [file] [file]\n");
	g_print("Where:\n");
	g_print("    expression is a filter for the returned file list. * and ? work.\n");
	g_print("    file matches the volumes whose contents you want to list.\n\n");
	g_print("Options:\n");
	g_print("    -v      Increase verbosity. Can be specified up to five times.\n\n");

}

int main(int argc, char** argv)
{
	vol_levelmask = VOL_LOG_LEVEL_MASK;
	guint i, j;
	gchar a;
	guint vcount = 0;
	gchar *expr, *volname;
	VOL volhandle;
	GPatternSpec *internalglob;
	GPtrArray *volpaths =  g_ptr_array_new_with_free_func(g_free);
	struct volfilelist vfiles;
	g_log_set_handler(G_LOG_DOMAIN, vol_levelmask, logfunc, NULL);
	if (argc < 3) {
		printhelp();
	}

	while((a = getopt(argc, argv, "v")) != -1)
	{
		switch(a)
		{
			case 'v':
				vcount++;
				g_print("-v detected\n");
				break;
			default:
				g_print("Problem with argument parsing. Got unexpected result %c\n", a);
		}
	}

	if (argc < optind + 2) {
		printhelp();
	}

	expr = argv[optind];

	for (i = optind + 1; i < argc; i++)
	{
		//We're going to skip handling of external blobs for now.
		g_ptr_array_add(volpaths, (gpointer)argv[i]);
	}

	if (vcount > 4)
		vcount = 4;
	switch(vcount)
	{
		case 4:
			vol_levelmask |= VOL_LOG_LEVEL_MOREDEBUG;
		case 3:
			vol_levelmask |= (VOL_LOG_LEVEL_FIXME + VOL_LOG_LEVEL_TODO);
		case 2:
			vol_levelmask |= G_LOG_LEVEL_DEBUG;
		case 1:
			vol_levelmask |= G_LOG_LEVEL_INFO;
	}
	g_log_set_handler(G_LOG_DOMAIN, vol_levelmask, logfunc, NULL);
	vol_set_debug(vol_levelmask);

	g_debug("Entering for loop");
	for(i = 0; i < volpaths->len; i++)
	{
		g_debug("Getting file list for %s\n", (gchar*)g_ptr_array_index(volpaths,i));
		volhandle = vol_load((gchar*)g_ptr_array_index(volpaths,i));
		VErr err = vol_get_error(volhandle);
		if (err.code != 0)
		{
			for (j = 0; j < 128; j++)
			{
				if (err.message[j] == 0x00)
					break;
				else
					g_print("%c", err.message[j]);
			}
			g_print("\n");
			break;
		} else {
			volname = g_strdup((gchar*)g_ptr_array_index(volpaths,i));
			g_print("%s\n", (gchar*)basename(volname));
			g_free(volname);
			vfiles = vol_get_filelist(volhandle);
			internalglob = g_pattern_spec_new(expr);
			for (j = 0; j < vfiles.len; j++)
			{
				if (g_pattern_match_string(internalglob, vfiles.filelist[j]))
					g_print("  %s\n", vfiles.filelist[j]);
				g_free(vfiles.filelist[j]);
			}
		}
		vol_unload(volhandle);
	}

	return 0;
}