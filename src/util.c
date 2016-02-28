/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 Markus Kitsinger (SwooshyCueb) <root@swooshalicio.us>
 * Copyright (C) 2014 OFTN Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#include <string.h>
#include "util.h"
#include "logging.h"
#include "dynvol.h"
#include "dynvol_private.h"
#include <glib.h>


gchar* sanitizepath(const gchar* dirtypath)
{
    /* The easiest way I can think to do this is to split the string up into
     * its path components and store them in an array, then iterate through
     * it, taking care of . and .. pathnames, then reiterate to construct the
     * new path string.
     */
    log_moredebug("Sanitizing path \"%s\".", dirtypath);
    gchar *cleanpath, *component;
    guint c, i, cleanlen = 0, componentoffset = 0;
    GPtrArray *path = g_ptr_array_new_with_free_func(g_free);
    /* Split the string into an array. */
    for (i=0; i <= strlen(dirtypath); i++)
    {
        /* Check for baskslash or slash */
        if ((dirtypath[i] == 0x5C) || (dirtypath[i] == 0x2F) || (i == strlen(dirtypath)))
        {
            log_moredebug("Slash (or end of string) found at index %u.", i);
            /* Skip if at beginning of path or duplicate */
            if ((i == componentoffset) && (i != strlen(dirtypath)))
            {
                log_moredebug("Slash was duplicate or at beginning of string. Ignoring.");
                componentoffset++;
                continue;
            }
            /* Otherwise we've got a string. */
            log_moredebug("Found %u character long path component at index %u.",
                i - componentoffset, componentoffset);
            component = g_malloc0(sizeof(gchar)*(i - componentoffset));
            /* We probably don't need to copy character by character, but we
             * will for now.
             */
            for (c=componentoffset; c <= i-1; c++)
            {
                component[c-componentoffset] = dirtypath[c];
            }
            log_moredebug("Path component \"%s\" pulled.", component);
            g_ptr_array_add(path, (gpointer)component);
            componentoffset = i + 1;
        }

    }

    /* Now we handle any instances of "." or ".." */
    i = 0;
    c = 0; /* c will be our flag for whether or not we can remove ".." */
    while (i < path->len)
    {
        component = g_ptr_array_index(path, i);
        if (!g_strcmp0(component, "."))
        {
            log_moredebug("Removing path comonent %u, \"%s\".", i, component);
            /* Simply remove the component if it is "." */
            g_ptr_array_remove_index(path, i);
        } else if ((!g_strcmp0(component, "..")) && (c != 0)) {
            /* If the component is "..", remove it and the component before it,
             * but not if first componetnt, or all previous components are ".."
             */
            log_moredebug("Removing path comonent %u, \"%s\".", i, component);
            g_ptr_array_remove_index(path, i);
            i--;
            log_moredebug("Removing path comonent %u, \"%s\".", i, g_ptr_array_index(path, i));
            g_ptr_array_remove_index(path, i);
            c--;
        } else {
            cleanlen += strlen(component) + 1;
            i++;
            if (g_strcmp0(component, ".."))
                c++;
        }
    }

    log_moredebug("New path string length calculated to be %u.", cleanlen);

    /* Allocate our new path string */
    cleanpath = g_malloc0(sizeof(gchar)*cleanlen);

    /* Move component strings into our new path string */
    while(path->len != 0)
    {
        component = g_ptr_array_index(path, 0);
        log_moredebug("Adding path component \"%s\" to new path string.", component);
        c = g_strlcat(cleanpath, component, cleanlen);
        g_ptr_array_remove_index(path, 0);
        /* Add slash if not the last component */
        if (path->len != 0)
            c = g_strlcat(cleanpath, "/", cleanlen);
        log_moredebug("New path string ends at index %u.", c);
    }
    
    log_moredebug("Sanitized path is \"%s\"", cleanpath);
    return cleanpath;
}

/* Filename characters to certainly avoid:
 * Character(s)   Reason
 * /              Directory separator in most operating systems.
 * ?<>:*|"        Most Windows filesystems don't like these.
 * \              Directory separator in Windows.
 * 0x00           Mid-string null-bytes are just problematic.
 */

/* Filename characters to maybe avoid:
 * Character(s)   Reason
 * ^[]+;,         Some versions of FAT don't like these.
 * 0x01 - 0x1F    Some versions of FAT don't like these.
 * 0x7F           Some versions of FAT don't like these.
 */

/* Problematic directory names:
 * Name   Reason
 * .      Indicates current directory. We should strip this out.
 * ..     Indicates parent directory. We should strip this and the previous directory out.
 */

void printfiles(gpointer file, gpointer count)
{
    (*(guint*)count)++;
    struct vfile* vfile = (struct vfile*)file;
    log_info("\tFile %u: %s", (*(guint*)count), vfile->path);
    if (g_strcmp0(vfile->path, vfile->path_canonical))
        log_info("\t\t (%s)", vfile->path_canonical);
}
