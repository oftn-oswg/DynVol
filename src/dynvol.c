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

//TODO: Some sort of internal error handler.

VOL vol_load(const gchar* path)
{
	g_log_set_handler(G_LOG_DOMAIN, G_LOG_LEVEL_MASK | G_LOG_FLAG_RECURSION | G_LOG_FLAG_FATAL, logfunc, NULL);
	GError *error = NULL;
	GIOStatus ret = G_IO_STATUS_NORMAL;
	struct volume* handle;

	log_info("Loading volume %s.", path);

	log_debug("Allocating memory for volume data container.");
	handle=g_malloc(sizeof(struct volume));

	//initialize variables
	handle->path = NULL;
	handle->volio = NULL;
	handle->files = NULL;
	handle->open = FALSE;
	handle->writable = FALSE;
	memset(&handle->header.ident, 0, sizeof(gchar[4]));
	handle->header.val = 0;
	memset(&handle->footer.unknown_vstr.header.ident, 0, sizeof(gchar[4]));
	handle->footer.unknown_vstr.header.val = 0;
	handle->footer.unknown_vstr.data = NULL;
	memset(&handle->footer.unknown_vval.header.ident, 0, sizeof(gchar[4]));
	handle->footer.unknown_vval.header.val = 0;
	handle->footer.unknown_vval.data = NULL;
	memset(&handle->footer.filenames.header.ident, 0, sizeof(gchar[4]));
	handle->footer.filenames.header.val = 0;
	handle->footer.filenames.data = NULL;
	memset(&handle->footer.fileprops.header.ident, 0, sizeof(gchar[4]));
	handle->footer.fileprops.header.val = 0;
	handle->footer.fileprops.data = NULL;

	handle->path = g_strdup(path);

	//We're just going to make the assumption that the file isn't going to change between testing and use.
	if (!g_file_test (path, G_FILE_TEST_EXISTS))
	{
		log_error("Volumme %s does not exist. Creation of new volumes not yet supported.", path);
		return NULL;
	} else if (!g_file_test (path, G_FILE_TEST_IS_REGULAR)) {
		log_error("Cannot open %s.", path);
		return NULL;
	}
	log_debug("Attempting to open %s", path);
	handle->volio = g_io_channel_new_file(path, "r", &error);
	//Just to be safe, we will only open the file for writing when we're actually writing to it.
	//I found out the hard way if you screw something up just right while the file is open for writing, bad things happen.
	//We don't want bad things to happen.
	//This archive format is broken enough as it is...
	if (!handle->volio) {
		log_error("Failed opening %s\n\t%s", path, error->message);
	}
	handle->open = TRUE;
	log_debug("Setting encoding to NULL.");
	ret = g_io_channel_set_encoding(handle->volio, NULL, &error);
	if (ret != G_IO_STATUS_NORMAL)
	{
		log_warning("Failed setting encoding for %s\n\t%s", path, error->message);
	}
	vol_getmetadata((gpointer)handle);
	//vol_parsemetadata(handle);
	return (gpointer)handle;
}


void vol_unload(VOL handle)
{
	struct volume* vhnd = handle;
	log_info("Unoading volume %s.", vhnd->path);
	GIOStatus ret = G_IO_STATUS_NORMAL;
	GError *error = NULL;
	if (vhnd->open)
	{
		log_debug("Closing file.");
		if (vhnd->writable)
			ret = g_io_channel_shutdown(vhnd->volio, TRUE, &error);
		else
			ret = g_io_channel_shutdown(vhnd->volio, FALSE, &error);
		if (ret != G_IO_STATUS_NORMAL)
			log_error("Failed closing file.\n\t%s", error->message);
		vhnd->open = FALSE;
	}
	log_debug("Freeing memory.");
	log_debug("Freeing unknown_vstr.");
	if (vhnd->footer.unknown_vstr.data != NULL)
		g_ptr_array_free(vhnd->footer.unknown_vstr.data, TRUE);
	log_debug("Freeing unknown_vval.");
	if (vhnd->footer.unknown_vval.data != NULL)
		g_free(g_array_free(vhnd->footer.unknown_vval.data, TRUE));
	log_debug("Freeing filenames.");
	if (vhnd->footer.filenames.data != NULL)
		g_ptr_array_free(vhnd->footer.filenames.data, TRUE); //TODO: Figure out why this generates an error when linked symbolicly.
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

void vol_getheader(VOL handle, struct header *header, const guint32 offset)
{
	struct volume* vhnd = handle;
	log_info("Fetching header");
	readinto(vhnd->volio, offset, (gsize)sizeof(struct header), (gpointer)header);
	log_debug("Got header:");
	log_debug("\tIDstring: %c%c%c%c", header->ident[0], header->ident[1], header->ident[2], header->ident[3]);
	log_debug("\tValue: 0x%x", header->val);
}

void vol_getfooter(VOL handle)
{
	struct volume* vhnd = handle;
	log_info("Fetching volume footer.");
	guint32 offset = vhnd->header.val;

	//TODO: figure out what these are for
	//TODO: figure out if content format varies from the other arrays
	offset = vol_getvstr(handle, &vhnd->footer.unknown_vstr, offset);
	offset = vol_getvval(handle, &vhnd->footer.unknown_vval, offset);


	offset = vol_getfilenames(handle, &vhnd->footer.filenames, offset);
	offset = vol_getfileprops(handle, &vhnd->footer.fileprops, offset);

}

void vol_getmetadata(VOL handle)
{
	struct volume* vhnd = handle;
	log_info("Fetching volume metadata.");
	vol_getheader(handle, &vhnd->header, 0);

	//Header verification
	if (memcmp(" VOL", vhnd->header.ident, 4) == 0) {
		log_debug("Magic number recognized. Archive is Starsiege volume.");
	} else if (memcmp("PVOL", vhnd->header.ident, 4) == 0) {
		log_debug("Magic number recognized. Archive is Tribes 1 volume.");
		log_critical("Currently, only Starsiege volumes are supported.");
	} else if (memcmp("VOLN", vhnd->header.ident, 4) == 0) {
		log_debug("Magic number recognized. Archive is Earthsiege/Earthsiege 2 volume.");
		log_critical("Currently, only Starsiege volumes are supported.");
	} else {
		log_critical("Magic number not recognized.");
	}

	vol_getfooter(handle);
}

guint32 vol_getvstr(VOL handle, struct vols *vstrarr, const guint32 offset)
{
	//gets header only
	struct volume* vhnd = handle;
	log_info("Fetching string array at 0x%x.", offset);
	log_debug("Scraping array header.");
	vol_getheader(handle, &vstrarr->header, offset);
	//Header verification
	if (memcmp("vols", vstrarr->header.ident, 4)) {
		log_critical("Array identity string not recognized.");
	}
	log_info("Skipping contents of unknown array.");
	vstrarr->data = NULL;
	return (offset+vstrarr->header.val+sizeof(struct header));
}

guint32 vol_getvval(VOL handle, struct volv *vvalarr, const guint32 offset)
{
	//gets header only
	struct volume* vhnd = handle;
	log_info("Fetching value set array at 0x%x.", offset);
	log_debug("Scraping array header.");
	vol_getheader(handle, &vvalarr->header, offset);
	//Header verification
	if (memcmp("voli", vvalarr->header.ident, 4)) {
		log_critical("Array identity string not recognized.");
	}
	log_info("Skipping contents of unknown array.");
	vvalarr->data = NULL;
	return (offset+vvalarr->header.val+sizeof(struct header));
}

guint32 vol_getfilenames(VOL handle, struct vols *fnarr, const guint32 offset)
{
	struct volume* vhnd = handle;
	log_info("Fetching filenames from array.");
	GError *error = NULL;
	GIOStatus ret1 = G_IO_STATUS_NORMAL;
	guint32 ret, os;
	guint8 bite = 0x00;
	ret = os = offset;
	gint i, j = 0;
	log_debug("Scraping array header.");
	vol_getheader(handle, &fnarr->header, os);
	//Header verification
	if (memcmp("vols", fnarr->header.ident, 4)) {
		log_critical("Array identity string not recognized.");
	}
	//vslist_init(&vstrarr->data);TODO
	ret+=sizeof(struct header);
	if (fnarr->header.val != 0)
	{
		os = ret;
		fnarr->data = g_ptr_array_new_with_free_func(g_free);
		log_debug("Fetching array data.");
		for (i = 0; i < fnarr->header.val; i++)
		{
			if (j == 0)
				log_debug("Seeking to end of string.");
			j++;
			bite = readbyte(vhnd->volio, (guint64)(os+j));
			if (bite == 0x00)
			{
				j++;
				log_debug("String at offset 0x%x indexed. Size is %d. Pulling now.", os, j);
				gchar *data;
				data = readpart(vhnd->volio, (guint64)os, sizeof(gchar)*j);
				log_debug("Pulled string: %s.", data);
				i++;
				os+=j;
				j=0;
				log_debug("Adding string to array.");
				g_ptr_array_add(fnarr->data, (gpointer)data);
				//TODO: construct array of file struct
			} else if (bite == 0x5C) {
				//TODO: For windows, warn about forwardslashes (and any other invalid characters)
				log_warning("An internal path string may contain backslashes.");
			}
			bite = 0x00;
		}
		guint* k;
		k = g_malloc0(sizeof(guint));
		log_info("Filename array created.");
		g_ptr_array_foreach(fnarr->data, printfiles, (gpointer)k);
		g_free(k);
	} else {
		log_message("Array is empty. (No files in volume?)");
		fnarr->data = NULL;
	}
	ret+=fnarr->header.val;
	return ret;
}

guint32 vol_getfileprops(VOL handle, struct volv *attrarr, const guint32 offset)
{
	struct volume* vhnd = handle;
	log_info("Fetching file properties array.");
	guint32 ret, os;
	ret = os = offset;
	gint i;
	log_debug("Scraping array header.");
	vol_getheader(handle, &attrarr->header, offset);
	//Header verification
	if (memcmp("voli", attrarr->header.ident, 4)) {
		log_critical("Array identity string not recognized.");
	}
	ret+=sizeof(struct header);
	if (attrarr->header.val != 0)
	{
		os = ret;
		attrarr->data = g_array_new(FALSE, TRUE, sizeof(struct vval));
		log_debug("Fetching array data.");
		for (i = 0; i < attrarr->header.val; i+=(sizeof(struct vval)))
		{
			struct vval *propset = g_malloc0(sizeof(struct vval));
			log_debug("Pulling value set.");
			readinto(vhnd->volio, (guint64)(os+i), (gsize)sizeof(struct vval), (gpointer)propset);
			log_info("Values pulled:");
			log_info("\t32-bit value 1:\t0x%x\t(unknown)", propset->field_1);
			log_info("\t32-bit value 2:\t0x%x\t(filename array offset)", propset->field_2);
			log_info("\t32-bit value 3:\t0x%x\t(file VBLK offset)", propset->field_3);
			log_info("\t32-bit value 4:\t0x%x\t(uncompressed filesize)", propset->field_4);
			log_info("\t8-bit value:   \t0x%x\t\t(compression)", propset->endcap);
			//TODO: Update file struct array
			//TODO: Get VBLK headers for each file
			g_array_append_val(attrarr->data, (*propset));
			g_free(propset);
		}
	} else {
		log_message("Array is empty. (No files in volume?)");
		attrarr->data = NULL;
	}
	ret+=attrarr->header.val;
	return ret;
}