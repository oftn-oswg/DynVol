/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 OFTN Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __DYNVOL_H__
#define __DYNVOL_H__

#include <glib.h>
#include "vol_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Handle for your volume. */
typedef gpointer vol_t;

/**
 * vol_open:
 * @volume: A string containing the path to a volume
 *
 * Open a volume @volume and return a handle #VOL for use in subsequent
 * DynVol functions. Does not (yet) support volume creation.
 *
 * Returns: A #VOL on success, %NULL on failure.
 **/
vol_t vol_open(const gchar* volume);

/**
 * vol_close:
 * @handle: A volume handle as returned by vol_load
 *
 * Close and unload the volume associated with handle @handle.
 **/
void vol_close(vol_t handle);


/**
 * vol_get_error
 * @handle: A volume handle as returned by vol_load
 *
 * Returns a VErr containing information about the last run DynVol function.
 **/
vol_err_t vol_get_error(vol_t handle);

const gchar *vol_strerror(vol_err_t err);


struct volfilelist {
    guint len;
    gchar** filelist;
};

/**
 * vol_get_filelist
 * @handle: A volume handle as returned by vol_load
 *
 * Returns a volfilenames struct containing the filenames within the volume.
 **/
struct volfilelist vol_get_filelist(vol_t handle, gboolean canonicalize);


void vol_set_debug(guint mask);


/**
 * Log levels.
 * Use these to construct a mask to pass to vol_set_debug
 *
 *
 **/

enum vol_loglevels {
    VOL_LOG_FLAG_RECURSION          = G_LOG_FLAG_RECURSION,
    VOL_LOG_FLAG_FATAL              = G_LOG_FLAG_FATAL,

    VOL_LOG_LEVEL_ERROR             = G_LOG_LEVEL_ERROR,
    VOL_LOG_LEVEL_CRITICAL          = G_LOG_LEVEL_CRITICAL,
    VOL_LOG_LEVEL_WARNING           = G_LOG_LEVEL_WARNING,
    VOL_LOG_LEVEL_MESSAGE           = G_LOG_LEVEL_MESSAGE,
    VOL_LOG_LEVEL_INFO              = G_LOG_LEVEL_INFO,
    VOL_LOG_LEVEL_DEBUG             = G_LOG_LEVEL_DEBUG,

    VOL_LOG_LEVEL_TODO            = 1 << 8,
    VOL_LOG_LEVEL_FIXME           = 1 << 9,
    VOL_LOG_LEVEL_MOREDEBUG       = 1 << 10,

    VOL_LOG_LEVEL_MASK = ~(VOL_LOG_LEVEL_MOREDEBUG | VOL_LOG_LEVEL_TODO | VOL_LOG_LEVEL_FIXME | VOL_LOG_LEVEL_DEBUG | VOL_LOG_LEVEL_INFO)
};

/**
 * temp_vol_test_rleout
 * @handle: A volume handle as returned by vol_load
 *
 * Extracts the first file found in the volume.
 * This function is temporary, for obvious reasons.
 **/
void temp_vol_test_rleout(vol_t handle);


#ifdef __cplusplus
}
#endif

#endif
