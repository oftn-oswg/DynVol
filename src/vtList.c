
#include "dynvol.h"
#include "logging.h"
#include <stdio.h>
#include <glib.h>
#include <unistd.h>

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
	guint i, exprarg = 0, filesarg = 0;
	guint vcount = 0;
	gchar a;
	gchar *expr;
	VOL volhandle;
	GPtrArray *volpaths =  g_ptr_array_new_with_free_func(g_free);
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
	g_print("Internal file blob: %s\n", expr);

	for (i = optind + 1; i < argc; i++)
	{
		g_print("External file blob: %s\n", argv[i]);
		g_ptr_array_add(volpaths, (gpointer)argv[i]);
	}

	/*for (i = 0; i < argc; i++)
	{
		g_print("arg %d is %s\n", i, argv[i]);
		if (!g_strcmp0(argv[i], "-v"))
		{
			vcount++;
			g_print("-v detected\n");
		} else if (!g_strcmp0(argv[i], "-vv")) {
			vcount+=2;
			g_print("-vv detected\n");
		} else if (!g_strcmp0(argv[i], "-vvv")) {
			vcount+=3;
			g_print("-vvv detected\n");
		} else if (!g_strcmp0(argv[i], "-vvvv")) {
			vcount+=4;
			g_print("-vvvv detected\n");
		} else if (exprarg == 0) {
			exprarg = i;
		} else if (filesarg == 0) {
			filesarg = i;
		}
	}*/
	if (vcount > 4)
		vcount = 4;
	g_print("%x\n", vol_levelmask);
	switch(vcount)
	{
		case 4:
			vol_levelmask |= VOL_LOG_LEVEL_MOREDEBUG;
			g_print("%x\n", vol_levelmask);
		case 3:
			vol_levelmask |= (VOL_LOG_LEVEL_FIXME + VOL_LOG_LEVEL_TODO);
			g_print("%x\n", vol_levelmask);
		case 2:
			vol_levelmask |= G_LOG_LEVEL_DEBUG;
			g_print("%x\n", vol_levelmask);
		case 1:
			vol_levelmask |= G_LOG_LEVEL_INFO;
			g_print("%x\n", vol_levelmask);
	}
	g_log_set_handler(G_LOG_DOMAIN, vol_levelmask, logfunc, NULL);
	vol_set_debug(vol_levelmask);

	//volhandle = vol_load()
	//TODO: Get rid of all these print statements
	//TODO: Get blob from args, store in string
	//TODO: Get vols from args, store in some sort of array or list
	//TODO: Get file lists from vols
	//TODO: Match blob against file lists
	//TODO: Return filtered file lists

	return 0;
}