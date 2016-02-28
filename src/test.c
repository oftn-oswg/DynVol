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
                    + /* VOL_LOG_LEVEL_MOREDEBUG + VOL_LOG_LEVEL_FIXME + VOL_LOG_LEVEL_TODO + */
                    G_LOG_LEVEL_DEBUG + G_LOG_LEVEL_INFO;
    g_log_set_handler(G_LOG_DOMAIN, vol_levelmask, logfunc, NULL);
    vol_set_debug(vol_levelmask);
    VOL volhandle;
    volhandle =
        vol_load("/home/swooshy/devstuff/ss/vol/Starsiege/samples/set6/foldertest1.vol");
    vol_unload(volhandle);
    volhandle =
        vol_load("/home/swooshy/devstuff/ss/vol/Starsiege/samples/set1/rletest.vol");
    temp_vol_test_rleout(volhandle);
    vol_unload(volhandle);
    volhandle = vol_load("/this/file/doesnt/exist.vol");
    VErr err = vol_get_error(volhandle);
    int i;
    for (i = 0; i < 128; i++)
    {
        if (err.message[i] == 0x00)
            break;
        else
            printf("%c", err.message[i]);
    }
    printf("\n");
    return 0;

}
