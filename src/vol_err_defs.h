/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 OFTN Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

VOL_ERR_DEF(OK,
            "No error",
            0x0000
)

VOL_ERR_DEF(UNKN,
            "Unknown error",
            0x0001
)

VOL_ERR_DEF(UNIMPL,
            "Unimplemented",
            0x0002
)

VOL_ERR_DEF(MEMORY,
            "Malloc failure",
            0x0003
)

VOL_ERR_DEF(NOENT,
            "No such file",
            0x0010
)

VOL_ERR_DEF(EXISTS,
            "File already exists",
            0x0011
)

VOL_ERR_DEF(RDONLY,
            "Read-only archive",
            0x0012
)

VOL_ERR_DEF(OPEN,
            "Can't open file",
            0x0013
)

VOL_ERR_DEF(CLOSE,
            "Closing archive failed",
            0x0014
)

VOL_ERR_DEF(SEEK,
            "Seek failed",
            0x0015
)

VOL_ERR_DEF(READ,
            "Read failed",
            0x0016
)

VOL_ERR_DEF(WRITE,
            "Write failed",
            0x0017
)

VOL_ERR_DEF(TMPOPEN,
            "Could not create temporary file",
            0x0018
)

VOL_ERR_DEF(NOTVOL,
            "Archive is either invalid or an unrecognized format",
            0x0020
)

VOL_ERR_DEF(UNSUPFORMAT,
            "Archive is of a recognized but unsupported format",
            0x0021
)

VOL_ERR_DEF(BROKEN,
            "Archive is either invalid or an unrecognized format",
            0x0022
)

VOL_ERR_DEF(BADHEADER,
            "A header within the archive did not contain the expected identifier",
            0x0023
)

VOL_ERR_DEF(EOF,
            "Premature end of file",
            0x0024
)
