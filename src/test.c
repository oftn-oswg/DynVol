
#include "dynvol.h"
#include <stdio.h>

int main(int argc, char** argv) {
	VOL volhandle;
	volhandle = vol_load("/home/swooshy/devstuff/ss/vol/Starsiege/samples/set6/foldertest1.vol");
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