/* Dynvol - Dynamix Volume Toolkit
 * Copyright (C) 2014 SwooshyCueb
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include <glib.h>
#include "dynvol.h"
#include "dynvol_private.h"
#include "util.h"
#include "logging.h"
#include "io.h"
#include <gio/gio.h>

VOL vol_load(const gchar* path)
{
	GError *error = NULL;
	GIOStatus ret = G_IO_STATUS_NORMAL;
	struct volume* handle;

	log_info("Loading volume %s.", path);

	log_debug("Allocating memory for volume data container.");
	handle=g_malloc(sizeof(struct volume));
	log_todo("Figure out how to return errors from vol_load");

	//initialize variables
	handle->path = NULL;
	handle->volio.identifier = NULL;
	handle->volio.readstream = NULL;
	handle->files = NULL;
	handle->open = FALSE;
	handle->writable = FALSE;
	handle->error = VERR_OK;
	memset(&handle->header.ident, 0, sizeof(gchar[4]));
	handle->header.val = 0;
	memset(&handle->footer.unknown_vstr.header.ident, 0, sizeof(gchar[4]));
	handle->footer.unknown_vstr.header.val = 0;
	handle->footer.unknown_vstr.offset = 0;
	handle->footer.unknown_vstr.data = NULL;
	memset(&handle->footer.unknown_vval.header.ident, 0, sizeof(gchar[4]));
	handle->footer.unknown_vval.header.val = 0;
	handle->footer.unknown_vval.offset = 0;
	handle->footer.unknown_vval.data = NULL;
	memset(&handle->footer.filenames.header.ident, 0, sizeof(gchar[4]));
	handle->footer.filenames.header.val = 0;
	handle->footer.filenames.offset = 0;
	handle->footer.filenames.data = NULL;
	memset(&handle->footer.fileprops.header.ident, 0, sizeof(gchar[4]));
	handle->footer.fileprops.header.val = 0;
	handle->footer.fileprops.offset = 0;
	handle->footer.fileprops.data = NULL;

	handle->path = g_strdup(path);

	//We're just going to make the assumption that the file isn't going to change between testing and use.
	if (!g_file_test (path, G_FILE_TEST_EXISTS))
	{
		log_critical("Volumme %s does not exist. Creation of new volumes not yet supported.", path);
		handle->error = VERR_FILE_NOT_FOUND;
		return (gpointer)handle;
	} else if (!g_file_test (path, G_FILE_TEST_IS_REGULAR)) {
		log_error("Cannot open %s.", path);
		handle->error = VERR_UNKNOWN;
		return (gpointer)handle;
	}
	log_debug("Attempting to open %s", path);

	//Just to be safe, we will only open the file for writing when we're actually writing to it.
	//I found out the hard way if you screw something up just right while the file is open for writing, bad things happen.
	//We don't want bad things to happen.
	//This archive format is broken enough as it is...

	handle->volio.identifier = g_file_new_for_path(path);
	handle->volio.readstream = g_file_read(handle->volio.identifier, NULL, &error);
	if (!handle->volio.readstream) {
		log_error("Failed opening %s\n\t%s", path, error->message);
		handle->error = VERR_UNKNOWN;
		return (gpointer)handle;
	}
	handle->open = TRUE;

	//Code left over from using glib gio
	/* log_debug("Setting encoding to NULL.");
	ret = g_io_channel_set_encoding(handle->volio, NULL, &error);
	if (ret != G_IO_STATUS_NORMAL)
	{
		log_warning("Failed setting encoding for %s\n\t%s", path, error->message);
	} */

	handle->error = vol_getmetadata((gpointer)handle);
	//vol_parsemetadata(handle);
	return (gpointer)handle;
}


void vol_unload(VOL handle)
{
	log_todo("Figure out how to return errors from vol_unload");
	struct volume* vhnd = handle;
	log_info("Unoading volume %s.", vhnd->path);
	GIOStatus ret = G_IO_STATUS_NORMAL;
	GError *error = NULL;

	if (!(g_input_stream_close(vhnd->volio.readstream, NULL, &error)))
	{
		log_critical("Couldn't close input stream:\t%s", error->message);
	}

	log_debug("Freeing readstream");
	g_object_unref(vhnd->volio.readstream);
	log_debug("Freeing file identifier");
	g_object_unref(vhnd->volio.identifier);

	log_debug("Freeing memory.");
	log_debug("Freeing unknown_vstr.");
	if (vhnd->footer.unknown_vstr.data != NULL)
		g_ptr_array_free(vhnd->footer.unknown_vstr.data, TRUE);
	log_debug("Freeing unknown_vval.");
	if (vhnd->footer.unknown_vval.data != NULL)
		g_free(g_array_free(vhnd->footer.unknown_vval.data, TRUE));
	log_debug("Freeing filenames.");
	if (vhnd->footer.filenames.data != NULL)
		g_ptr_array_free(vhnd->footer.filenames.data, TRUE);
	log_debug("Freeing fileprops.");
	if (vhnd->footer.fileprops.data != NULL)
		g_free(g_array_free(vhnd->footer.fileprops.data, TRUE));
	log_debug("Freeing files.");
	if (vhnd->files != NULL)
		g_free(g_array_free(vhnd->files, TRUE));
	log_debug("Freeing path.");
	g_free(vhnd->path);
	log_debug("Freeing handle.");
	g_free(vhnd);

}

VErrcode vol_getheader(VOL handle, struct header *header, const guint32 offset)
{
	struct volume* vhnd = handle;
	log_info("Fetching header");
	VErrcode err = readinto(&vhnd->volio, offset, (gsize)sizeof(struct header), (gpointer)header);
	log_debug("Got header:");
	log_debug("\tIDstring: %c%c%c%c", header->ident[0], header->ident[1], header->ident[2], header->ident[3]);
	log_debug("\tValue: 0x%x", header->val);
	return err;
}

VErrcode vol_getfooter(VOL handle)
{
	struct volume* vhnd = handle;
	log_info("Fetching volume footer.");
	vhnd->footer.unknown_vstr.offset = vhnd->header.val;

	//TODO: figure out what these are for
	//TODO: figure out if content format varies from the other arrays
	VErrcode err = vol_getvstr(handle);
	if (err)
		return err;
	err = vol_getvval(handle);
	if (err)
		return err;


	err = vol_getfilenames(handle);
	if (err)
		return err;
	return vol_getfileprops(handle);
}

VErrcode vol_getmetadata(VOL handle)
{
	struct volume* vhnd = handle;
	log_info("Fetching volume metadata.");
	VErrcode err = vol_getheader(handle, &vhnd->header, 0);
	if (err)
		return err;

	//Header verification
	if (memcmp((gchar[]){' ', 'V', 'O', 'L'}, vhnd->header.ident, 4) == 0) {
		log_debug("Magic number recognized. Archive is Starsiege volume.");
	} else if (memcmp("PVOL", vhnd->header.ident, 4) == 0) {
		log_debug("Magic number recognized. Archive is Tribes 1 volume.");
		log_critical("Currently, only Starsiege volumes are supported.");
		return VERR_UNSUPPORTED_ARCHIVE;
	} else if (memcmp("VOLN", vhnd->header.ident, 4) == 0) {
		log_debug("Magic number recognized. Archive is Earthsiege/Earthsiege 2 volume.");
		log_critical("Currently, only Starsiege volumes are supported.");
		return VERR_UNSUPPORTED_ARCHIVE;
	} else {
		log_critical("Magic number not recognized.");
		return VERR_ARCHIVE_UNRECOGNIZED;
	}

	return vol_getfooter(handle);
}

VErrcode vol_getvstr(VOL handle)
{
	//gets header only
	struct volume* vhnd = handle;
	log_info("Fetching string array.");
	log_debug("Scraping array header.");
	VErrcode err = vol_getheader(handle, &vhnd->footer.unknown_vstr.header, vhnd->footer.unknown_vstr.offset);
	if (err)
		return err;
	//Header verification
	//There might be a memory leak here
	if (memcmp("vols", vhnd->footer.unknown_vstr.header.ident, 4)) {
		log_warning("Array identity string not recognized.");
		//Not setting error here, at least not yet
	}
	log_info("Skipping contents of unknown array.");
	vhnd->footer.unknown_vstr.data = NULL;
	vhnd->footer.unknown_vval.offset = vhnd->footer.unknown_vstr.offset+sizeof(struct header)+vhnd->footer.unknown_vstr.header.val;
	return err;
}

VErrcode vol_getvval(VOL handle)
{
	//gets header only
	struct volume* vhnd = handle;
	log_info("Fetching value set array.");
	log_debug("Scraping array header.");
	VErrcode err = vol_getheader(handle, &vhnd->footer.unknown_vval.header, vhnd->footer.unknown_vval.offset);
	if (err)
		return err;
	//Header verification
	if (memcmp("voli", vhnd->footer.unknown_vval.header.ident, 4)) {
		log_critical("Array identity string not recognized.");
		//Not setting error here, at least not yet
	}
	log_info("Skipping contents of unknown array.");
	vhnd->footer.unknown_vval.data = NULL;
	vhnd->footer.filenames.offset = vhnd->footer.unknown_vval.offset+sizeof(struct header)+vhnd->footer.unknown_vval.header.val;
	return err;
}

VErrcode vol_getfilenames(VOL handle)
{
	struct volume* vhnd = handle;
	log_info("Fetching filenames from array.");
	guint64 os;
	os = vhnd->footer.filenames.offset;
	gint i, j = 0;
	guint8 bite;
	log_debug("Scraping array header.");
	VErrcode err = vol_getheader(handle, &vhnd->footer.filenames.header, vhnd->footer.filenames.offset);
	if (err)
		return err;
	//Header verification
	if (memcmp("vols", vhnd->footer.filenames.header.ident, 4)) {
		log_critical("Array identity string not recognized.");
		return VERR_BROKEN_ARCHIVE;
	}
	if (vhnd->footer.filenames.header.val != 0)
	{
		os+=sizeof(struct header);
		vhnd->footer.filenames.data = g_ptr_array_new_with_free_func(g_free);
		log_debug("Fetching array data.");
		for (i = 0; i < vhnd->footer.filenames.header.val; i++)
		{
			if (j == 0)
				log_debug("Seeking to end of string.");
			j++;
			readbyte(&vhnd->volio, (guint64)(os+j), &bite);
			if (bite == 0x00)
			{
				j++;
				log_debug("String at offset 0x%lx indexed. Size is %d. Pulling now.", os, j);
				gchar *data;
				data = readpart(&vhnd->volio, (guint64)os, sizeof(gchar)*j);
				log_debug("Pulled string: %s.", data);
				i++;
				os+=j;
				j=0;
				log_debug("Adding string to array.");
				g_ptr_array_add(vhnd->footer.filenames.data, (gpointer)data);
				log_todo("Construct file struct array");
			} else if (bite == 0x5C) {
				log_todo("Warn windows users about invalid characters and forward slashes");
				log_warning("An internal path string may contain backslashes.");
			}
		}
		guint* k;
		k = g_malloc0(sizeof(guint));
		log_info("Filename array created.");
		g_ptr_array_foreach(vhnd->footer.filenames.data, printfiles, (gpointer)k);
		g_free(k);
	} else {
		log_message("Array is empty. (No files in volume?)");
		vhnd->footer.filenames.data = NULL;
	}
	vhnd->footer.fileprops.offset=vhnd->footer.filenames.header.val+vhnd->footer.filenames.offset+sizeof(struct header);
	return err;
}

VErrcode vol_getfileprops(VOL handle)
{
	struct volume* vhnd = handle;
	log_info("Fetching file properties array.");
	guint64 os;
	os = vhnd->footer.fileprops.offset;
	if (os % 4)
	{
		log_info("Offset may be inaccurate.");
		os=((os/4)+1)*4;
		log_info("Offset is 0x%x. Offset should be 0x%x. Correcting.", vhnd->footer.fileprops.offset, os);
		vhnd->footer.fileprops.offset = os;
		// It seems that the array length value given by the filename array isn't always accurate,
		// or it doesn't account for any padding between arrays.
		// Array headers in the footer always start on an offset that's a multiple of four,
		// so this is how we're compenasting for the time being.
		// The array size values may need another look. If this isn't a fluke, we should implement a better
		// way of locating the beginnings of footer arrays.
	}
	gint i;
	log_debug("Scraping array header.");
	VErrcode err = vol_getheader(handle, &vhnd->footer.fileprops.header, vhnd->footer.fileprops.offset);
	if (err)
		return err;
	//Header verification
	if (memcmp("voli", vhnd->footer.fileprops.header.ident, 4)) {
		log_critical("Array identity string not recognized.");
	}
	if (vhnd->footer.fileprops.header.val != 0)
	{
		os +=sizeof(struct header);
		vhnd->footer.fileprops.data = g_array_new(FALSE, TRUE, sizeof(struct vval));
		log_debug("Fetching array data.");
		for (i = 0; i < vhnd->footer.fileprops.header.val; i+=(sizeof(struct vval)))
		{
			struct vval *propset = g_malloc0(sizeof(struct vval));
			log_debug("Pulling value set.");
			err = readinto(&vhnd->volio, (guint64)(os+i), (gsize)sizeof(struct vval), (gpointer)propset);
			if (err)
				return err;
			log_info("Values pulled:");
			log_info("\t32-bit value 1:\t0x%x\t(unknown)", propset->field_1);
			log_info("\t32-bit value 2:\t0x%x\t(filename array offset)", propset->field_2);
			log_info("\t32-bit value 3:\t0x%x\t(file VBLK offset)", propset->field_3);
			log_info("\t32-bit value 4:\t0x%x\t(uncompressed filesize)", propset->field_4);
			log_info("\t8-bit value:   \t0x%x\t\t(compression)", propset->endcap);
			log_todo("Update file struct array");
			log_todo(" Get VBLK headers for each file");
			g_array_append_val(vhnd->footer.fileprops.data, (*propset));
			g_free(propset);
		}
	} else {
		log_message("Array is empty. (No files in volume?)");
		vhnd->footer.fileprops.data = NULL;
	}
	return err;
}

struct volfilelist vol_get_filelist(VOL handle)
{
	struct volume* vhnd = handle;
	struct volfilelist filelist;
	guint i;
	gchar *filename;
	filelist.filelist = g_malloc(vhnd->footer.filenames.data->len * sizeof(gchar*));
	filelist.len = vhnd->footer.filenames.data->len;
	for(i = 0; i < vhnd->footer.filenames.data->len; i++)
	{
		filelist.filelist[i] = g_strdup((gchar*)g_ptr_array_index(vhnd->footer.filenames.data,i));
	}
	return filelist;
}

void vol_set_debug(guint mask)
{
	g_log_set_handler(G_LOG_DOMAIN, mask, logfunc, NULL);
	log_todo("Set default logging level.");
	log_todo("Potentially handle logging level per volume?");
}