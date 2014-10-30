/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 ΩF:∅ Working Group contributors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "dynvol.h"
#include "dynvol_private.h"
#include <glib.h>

/* yeah, this function gets its own file */

VErr vol_get_error(VOL handle)
{
    VErr ret;
    struct volume* vhnd = handle;

    ret.code = vhnd->error;

    /* might be some memory leaks here, not sure */
    switch(vhnd->error)
    {
        case VERR_OK:
            g_stpcpy(ret.message, "No error");
            break;
        case VERR_UNKNOWN:
            g_stpcpy(ret.message, "Unknown error");
            break;
        case VERR_FILE_NOT_FOUND:
            g_stpcpy(ret.message, "File not found");
            break;
        case VERR_ARCHIVE_UNRECOGNIZED:
            g_stpcpy(ret.message, "Archive is either invalid or an unrecognized format");
            break;
        case VERR_UNSUPPORTED_ARCHIVE:
            g_stpcpy(ret.message, "Archive is of a recognized but unsupported format");
            break;
        case VERR_BROKEN_ARCHIVE:
            g_stpcpy(ret.message, "Archive is either invalid or an unrecognized format");
            /* we may be able to provide more information here */
            break;
        case VERR_UNRRECOGNIZED_HEADER:
            g_stpcpy(ret.message, "A header within the archive did not contain the expected identifier");
            break;
        case VERR_UNEXPECTED_EOF:
            g_stpcpy(ret.message, "Unexpected end of file");
            break;
        case VERR_SEEK_FAILED:
            g_stpcpy(ret.message, "Could not seek in file or stream");
            break;
        case VERR_READ_FAILED:
            g_stpcpy(ret.message, "Could not read from file or stream");
            break;
        case VERR_CLOSE_FAILED:
            g_stpcpy(ret.message, "Could not close the stream");
            break;

    }

    return ret;
}
