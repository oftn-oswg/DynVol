/* Dynvol - Dynamix Volume Toolkit
 * Copyright © 2014-2017 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright © 2014-2017 ΩF:∅ OSWG
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef VOL_ERR_H
#define VOL_ERR_H

#include <glib.h>

#ifndef VOL_ERR_PREFIX
#define VOL_ERR_PREFIX VOL_ERR_
#endif

#ifdef __cplusplus
#ifndef VOL_ERR_TYPE
typedef guint32 vol_err_t;
#define VOL_ERR_TYPE
#endif
#endif

#ifndef VOL_ERR_DEF_
#define VOL_ERR_DEF_(code, string, value) code = value,
#ifdef __cplusplus
#define VOL_ERR_START_LIST enum : vol_err_t {
#define VOL_ERR_END_LIST };
#else
#define VOL_ERR_START_LIST typedef enum {
#define VOL_ERR_END_LIST } vol_err_t ;
#endif
#endif

#define VOL_ERR_CAT_(prefix, error) prefix ## error
#define VOL_ERR_CAT(prefix, error) VOL_ERR_CAT_(prefix, error)

#define VOL_ERR_DEF(code, string, value) \
    VOL_ERR_DEF_(VOL_ERR_CAT(VOL_ERR_PREFIX, code), string, value)

#ifdef VOL_ERR_START_LIST
VOL_ERR_START_LIST
#endif

#include "vol_err_defs.h"

#ifdef VOL_ERR_END_LIST
VOL_ERR_END_LIST
#endif

#endif
