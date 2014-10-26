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
            cleanpath[i] = 0x2F;
        else
            cleanpath[i] = dirtypath[i];
    }
    return cleanpath;
}

void printfiles(gpointer name, gpointer count)
{
    (*(guint*)count)++;
    log_info("\tFile %u: %s", (*(guint*)count), (gchar*)name);
}
