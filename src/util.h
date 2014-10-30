/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 ΩF:∅ Working Group contributors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __DYNVOL_UTIL_H__
#define __DYNVOL_UTIL_H__

#include "dynvol.h"
#include "dynvol_private.h"
#include <glib.h>


/* This function does its best to canonicalize the internal path strings.
 * We may need a different version for Windoze.
 *
 * Should be g_free()'d when done.'
 */
gchar* sanitizepath(const gchar* dirtypath);

/* This is for string arrays */
void printfiles(gpointer filestruct, gpointer count);

struct counter {
    guint count;
};

#endif
