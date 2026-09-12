#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "allegro/text.h"
#include "helpers.h"
#include "intro_statics.h"
#include "dat_manager.h"


DATAFILE *dat_file;
DATAFILE *intro_dat_file;



#define MAX_FILENAME_LEN 16

typedef struct {
    char filename[MAX_FILENAME_LEN];
    long long filesize;
} FileHeader;

static void rotar_paleta() {
    PALETTE pal;
    get_palette(pal);

    for (int i = 200; i < 240; i++) {
        int nuevo_indice = (i + 1);
        RGB color_temp = pal[i];
        pal[i] = pal[nuevo_indice];
        pal[nuevo_indice] = color_temp;
    }

    set_palette(pal);
}
END_OF_FUNCTION(rotar_paleta)


DATAFILE * extract_data() {
    intro_dat_file = load_datafile("intro.dat");
    #ifdef _WIN32
        stretch_blit(intro_dat_file[CARS2_BMP].dat, screen,
             0, 0, 320, 200,
             0, 0, WIN32_WIDTH, WIN32_HEIGHT);
    #else 
        blit(intro_dat_file[CARS2_BMP].dat, screen, 0, 0, 0, 0, 320, 200);
    #endif
    //install_int(rotar_paleta, 100);
    textprintf_ex(screen, font, 105, 184, 2, 15, "LOADING...");
    install_int_ex(rotar_paleta, BPS_TO_TIMER(40));
    dat_file = load_datafile("datos.dat");
 
    remove_int(rotar_paleta);
    textprintf_ex(screen, font, 105, 184, 40, 15, "PRESS SPACE");
    wait_for_space();
    return dat_file;
    //fclose(input);
}

DATAFILE * obtain_videodata(char * video_id) {
    DATAFILE *video_obj = load_datafile_object("video.dat", video_id);
    if (!video_obj) {
        allegro_message("Error: cannot load video data for %s\n", video_id);
        exit(1);
    }
    return video_obj;
}
