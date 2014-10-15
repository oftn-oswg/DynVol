/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 SwooshyCueb
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __DYNVOL_IO_H__
#define __DYNVOL_IO_H__

#include "dynvol.h"
#include <glib.h>

//IO stuff

//Should be g_free()'d when done
gchar* readpart(GIOChannel *stream, const guint64 offset, const guint64 bytes);

VErrcode readbyte(GIOChannel *stream, const guint64 offset, guint8 *byte);
VErrcode readinto(GIOChannel *stream, const guint64 offset, const gsize size, gpointer container);

#endif