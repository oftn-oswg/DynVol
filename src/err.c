#include "dynvol.h"
#include "dynvol_private.h"
#include <glib.h>

VErr get_vol_error(VOL handle)
{
	VErr ret;
	struct volume* vhnd = handle;

	ret.code = vhnd->error;

	//might be some memory leaks here, not sure
	switch(vhnd->error){
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
			//we may be able to provide more information here
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

	}

	return ret;
}