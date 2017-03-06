/* Dynvol - Dynamix Volume Toolkit
 * Copyright © 2014-2017 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright © 2014-2017 ΩF:∅ OSWG
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "dynvol.h"
#include "dynvol_private.h"
#include <glib.h>

#define STRINGIFY(x) #x
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)

vol_err_t vol_get_error(vol_t handle) {
    struct volume* vhnd = handle;
    return vhnd->error;
}

const gchar *vol_strerror(vol_err_t err) {
    #undef VOL_ERR_H
    #undef VOL_ERR_DEF_
    #undef VOL_ERR_START_LIST
    #undef VOL_ERR_END_LIST
    #define VOL_ERR_DEF_( e, s, v )  case e: return EXPAND_AND_STRINGIFY(e) ": " s;
    #define VOL_ERR_START_LIST     switch (err) {
    #define VOL_ERR_END_LIST       }
    #include "vol_err.h"
    return "(Unknown error)";
}
