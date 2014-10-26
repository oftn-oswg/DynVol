/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 ΩF:∅ Working Group contributors
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

/* Handle for your volume. */
typedef gpointer VOL;

#define VOL_ERR_LEN 128
/* This is pretty arbitrary for now */

typedef enum {
    VERR_OK= 0,
    VERR_UNKNOWN,
    VERR_FILE_NOT_FOUND,
    VERR_ARCHIVE_UNRECOGNIZED,
    VERR_UNSUPPORTED_ARCHIVE,
    VERR_BROKEN_ARCHIVE,
    VERR_UNEXPECTED_EOF,
    VERR_SEEK_FAILED,
    VERR_READ_FAILED,
    VERR_CLOSE_FAILED,
    VERR_LAST /* Not actually an error */
} VErrcode;



/* struct for DynVol errors */
struct _errstruct {
    VErrcode code;
    gchar message[VOL_ERR_LEN];
    /*
     * addtl_data not used yet
     */
    /*
    gpointer addtl_data;
    */
};

typedef struct _errstruct VErr;


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


/**
 * vol_get_error
 * @handle: A volume handle as returned by vol_load
 *
 * Returns a VErr containing information about the last run DynVol function.
 **/
VErr vol_get_error(VOL handle);


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
struct volfilelist vol_get_filelist(VOL handle);


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





#ifdef __cplusplus
}
#endif

#endif
