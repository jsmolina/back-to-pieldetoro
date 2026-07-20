#include <stdint.h>
#include <string.h>
#include "allegro/base.h"

#define XOR_SALT 0x5A3C9E7B1F4D2A8BULL

const char BASE32_ALPHABET[] = "ABCDEFGHJKMNPQRSTUVWXYZ23456789";

typedef struct {
    uint8_t level;   // 6 bits (0-63)
    uint8_t lifes;   // 4 bits (0-15)
    uint16_t money; // 10 bits (0-1023)
    uint32_t score;  // 18 bits (0-262143)
} GameState;

// Simple function to generate an 8-bit checksum
uint8_t calculate_checksum(uint64_t datos) {
    uint8_t crc = 0;
    for (int i = 0; i < 8; i++) {
        crc ^= (datos >> (i * 8)) & 0xFF;
    }
    return crc;
}

void generate_pass(int level, int lifes, int money, int score, char* out_pass) {
    // 1. package the data into a 64-bit integer
    uint64_t empaquetado = 0;
    GameState estado = { .level = level, .lifes = lifes, .money = money, .score = score };
    empaquetado |= ((uint64_t)(estado.level  & 0x3F))  << 0;  // bits 0-5
    empaquetado |= ((uint64_t)(estado.lifes  & 0x0F))  << 6;  // bits 6-9
    empaquetado |= ((uint64_t)(estado.money & 0x3FF)) << 10; // bits 10-19
    empaquetado |= ((uint64_t)(estado.score  & 0x3FFFF))<< 20; // bits 20-37

    // 2. Calculate checksum of the original data and put it in bits 38-45
    uint8_t crc = calculate_checksum(empaquetado);
    empaquetado |= ((uint64_t)crc) << 38;

    // Total bits used: 46 bits.
    // In Base32 (5 bits per character), we need 10 characters (50 bits capacity).

    // 3. Obfuscate with XOR so it's not linear
    empaquetado ^= XOR_SALT;

    // 4. Encode to alphanumeric Base32
    for (int i = 0; i < 10; i++) {
        int indice_alfabeto = (empaquetado >> (i * 5)) & 0x1F; // Extraer de 5 en 5 bits
        out_pass[i] = BASE32_ALPHABET[indice_alfabeto];
    }
    out_pass[10] = '\0'; // End of string
}

// Decode the password and validate that it has not been tampered with
int load_pass(const char* pass, int *level, int *lifes, int *money, int *score) {
    uint64_t empaquetado = 0;

    // 1. Reconstruct the number from Base32
    for (int i = 0; i < 10; i++) {
        char c = pass[i];
        char* pos = strchr(BASE32_ALPHABET, c);
        if (!pos) return FALSE; // Invalid character detected
        
        uint64_t valor = pos - BASE32_ALPHABET;
        empaquetado |= (valor << (i * 5));
    }

    // 2. Undo the XOR
    empaquetado ^= XOR_SALT;

    // 3. Extract the stored checksum and clear those bits for recalculation
    uint8_t crc_guardado = (empaquetado >> 38) & 0xFF;
    uint64_t datos_limpios = empaquetado & ((1ULL << 38) - 1); // Solo los primeros 38 bits

    // 4. Validate integrity
    if (calculate_checksum(datos_limpios) != crc_guardado) {
        return FALSE; // The password was tampered with or is incorrectly written!
    }

    // 5. Unpack the data back into the structure
    *level  = (datos_limpios >> 0)  & 0x3F;
    *lifes  = (datos_limpios >> 6)  & 0x0F;
    *money = (datos_limpios >> 10) & 0x3FF;
    *score  = (datos_limpios >> 20) & 0x3FFFF;

    return TRUE;
}
