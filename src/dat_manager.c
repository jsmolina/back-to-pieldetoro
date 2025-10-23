#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "dat_manager.h"


DATAFILE *dat_file;

#define MAX_FILENAME_LEN 16

typedef struct {
    char filename[MAX_FILENAME_LEN];
    long long filesize;
} FileHeader;

static void rotar_paleta() {
    PALETTE pal;
    get_palette(pal);

    for (int i = 200; i < 240; i++) {
        int nuevo_indice = (i + 1) % 256;
        RGB color_temp = pal[i];
        pal[i] = pal[nuevo_indice];
        pal[nuevo_indice] = color_temp;
    }

    set_palette(pal);
}
END_OF_FUNCTION(rotar_paleta)


DATAFILE * extract_data() {

    //install_int(rotar_paleta, 100);
    install_int_ex(rotar_paleta, BPS_TO_TIMER(40));
    dat_file = load_datafile("datos.dat");
 
    remove_int(rotar_paleta);
    return dat_file;
    //fclose(input);
}
