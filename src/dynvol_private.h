/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 OFTN Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __DYNVOL_PRIVATE_H__
#define __DYNVOL_PRIVATE_H__

#include <glib.h>
#include <gio/gio.h>
#include "dynvol.h"

#define volstruct struct __attribute__((__packed__))

typedef enum {
    VFMT_STARSIEGE= 0,
    VFMT_TRIBES,
    VFMT_UNSUPPORTED,
    VFMT_LAST
} VFormat;

volstruct header {
    gchar ident[4];
    guint32 val;
};

volstruct VBLK {
    struct header header;
    gchar *data;
};

volstruct vols {
    struct header header;
    guint32 offset;
    GPtrArray *data;
};

volstruct volv {
    struct header header;
    guint32 offset;
    GArray *data;
};

volstruct vval {
    guint32 field_1;
    guint32 field_2;
    guint32 field_3;
    guint32 field_4;
    guint8 endcap;
};

volstruct vfile {
    gchar *path;
    gchar *path_canonical;
    gchar *name;
    gchar *dir;
    guint32 size;
    gint32 packed_size;
    gboolean compressed;
    guint32 n_offset;
    guint32 b_offset;
    struct VBLK data;
};

volstruct footer {
    struct vols unknown_vstr;
    struct volv unknown_vval;
    struct vols filenames;
    struct volv fileprops;
};

volstruct volio {
    GFile *identifier;
    GFileInputStream *readstream;
};


/* This is VOL on the outside. */
volstruct volume {
    gchar *path;
    struct volio volio;
    GPtrArray *files;
    gboolean open;
    gboolean writable;
    struct header header;
    struct footer footer;
    VFormat format;
    VErrcode error;
};

/* Fetches metadata and stores it inside vol */
VErrcode vol_getmetadata(vol_t handle);

/* Fetches volume footer and stores it inside vol */
VErrcode vol_getfooter(vol_t handle);

/* Fetches a header */
VErrcode vol_getheader(vol_t handle, struct header *header,
                       const guint32 offset);

/* Functions for skipping the first two arrays in the footer
 * Probably temporary
 */
VErrcode vol_getvstr(vol_t handle);
VErrcode vol_getvval(vol_t handle);

/* Functions for fetching file metadata */
VErrcode vol_getfilenames(vol_t handle);
VErrcode vol_getfileprops(vol_t handle);

/* Function to free files array */
void vol_filesarray_free(gpointer file);

#endif
