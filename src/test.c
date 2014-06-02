
#include "dynvol.h"

int main(int argc, char** argv) {
	VOL volhandle;
	volhandle = vol_load("/home/swooshy/devstuff/ss/vol/Starsiege/samples/set6/foldertest1.vol");
	vol_unload(volhandle);
	return 0;

}