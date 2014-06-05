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

// Handle for your volume.
typedef gpointer VOL;

#define VOL_ERR_LEN 128 //This is pretty arbitrary for now

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
	VERR_LAST //Not actually an error
} VErrcode;



//struct for DynVol errors
struct _errstruct {
	VErrcode code;
	gchar message[VOL_ERR_LEN];
	//gpointer addtl_data; //not used yet
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


#ifdef __cplusplus
}
#endif

#endif
