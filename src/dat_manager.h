#ifndef DAT_MANAGER
#define DAT_MANAGER
/**
 * @file dat_manager.h
 * @brief Header for data file management using Allegro's DATAFILE.
 *
 * Declares external reference to the loaded DATAFILE and functions to
 * extract and clean up data from a .dat resource file.
 *
 * - `extern DATAFILE *dat_file;` Reference to the loaded data file.
 * - `DATAFILE *extract_data();` Loads and unpacks the .dat file.
 * - Cleanup functions should be implemented to release resources.
 */
#include <allegro.h>
#include "video_statics.h"


extern DATAFILE *dat_file;
// unpacks dat file
DATAFILE * extract_data();
DATAFILE * obtain_videodata(char * video_id);
// cleanups data

#endif
