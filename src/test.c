/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 OFTN Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "dynvol.h"
#include "logging.h"
#include <stdio.h>

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif
#define G_LOG_DOMAIN "vtTest"

int main(int argc, char** argv)
{
    vol_levelmask = VOL_LOG_LEVEL_MASK
                    + VOL_LOG_LEVEL_MOREDEBUG + /* VOL_LOG_LEVEL_FIXME + VOL_LOG_LEVEL_TODO + */
                    G_LOG_LEVEL_DEBUG + G_LOG_LEVEL_INFO;
    g_log_set_handler(G_LOG_DOMAIN, vol_levelmask, logfunc, NULL);
    vol_set_debug(vol_levelmask);
    vol_t volhandle;
    volhandle =
        vol_open("/home/swooshy/devstuff/libdynvol/DynVol/Starsiege/samples/set6/foldertest1.vol");
    vol_close(volhandle);
    volhandle =
        vol_open("/home/swooshy/devstuff/libdynvol/DynVol/Starsiege/samples/set3/rletest.vol");
    temp_vol_test_rleout(volhandle);
    vol_close(volhandle);
    volhandle =
        vol_open("/home/swooshy/devstuff/libdynvol/DynVol/Starsiege/samples/set1/lzhtest.vol");
    temp_vol_test_rleout(volhandle);
    vol_close(volhandle);
    volhandle = vol_open("/this/file/doesnt/exist.vol");
    vol_err_t err = vol_get_error(volhandle);
    if (err) {
        printf("Error %s\n", vol_strerror(err));
    }
    return 0;

}
