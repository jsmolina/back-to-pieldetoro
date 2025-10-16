#ifndef DAT_MANAGER
#define DAT_MANAGER
#include <allegro.h>

extern DATAFILE *dat_file;
// unpacks dat file
DATAFILE * extract_data();
// cleanups data
void cleanup_data();

#endif
