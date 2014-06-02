/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 SwooshyCueb
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __DYNVOL_H__
#define __DYNVOL_H__

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef gpointer VOL;
// Handle for your volume.



/**
 * vol_load:
 * @volume: A string containing the path to a volume
 *
 * Open a volume @volume and return a handle #VOL for use in subsequent
 * DynVol functions. Does not (yet) support volume creation.
 *
 * Returns: A #VOL on success, %NULL on failure.
 **/
VOL vol_load(const gchar* volume);

/**
 * vol_unload:
 * @handle: A volume handle as returned by vol_load
 *
 * Close and unload the volume associated with handle @handle.
 **/
void vol_unload(VOL handle);


#ifdef __cplusplus
}
#endif

#endif