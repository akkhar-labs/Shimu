/* 
 * formatting.c - Implementation of the stream-based Output Formatting Engine
 * Architect: Rahat Hasan | Akkhar Labs
 */

#include "utils/formatting.h"
#include <stdio.h>
#include <string.h>

// বাংলা সংখ্যার ম্যাপিং টেবিল
static const char* BENGALI_DIGITS[] = {
    "০", "১", "২", "৩", "৪", "৫", "৬", "৭", "৮", "৯"
};

// অক্ষর-হেক্স (Akkhar-Hex) আলফাবেট ম্যাপিং (১০-১৫)
static const char* BENGALI_HEX_ALPHA[] = {
    "ক", // 10 (A)
    "খ", // 11 (B)
    "গ", // 12 (C)
    "ঘ", // 13 (D)
    "ঙ", // 14 (E)
    "চ"  // 15 (F)
};

// ১. গাণিতিক মানকে বাংলা সংখ্যায় রূপান্তর করে স্ট্রিমে লেখা
void fprint_bengali_int(FILE *stream, int value) {
    if (value == 0) {
        fprintf(stream, "%s", BENGALI_DIGITS[0]);
        return;
    }

    if (value < 0) {
        fprintf(stream, "-");
        value = -value;
    }

    char buffer[32];
    sprintf(buffer, "%d", value);

    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] >= '0' && buffer[i] <= '9') {
            fprintf(stream, "%s", BENGALI_DIGITS[buffer[i] - '0']);
        }
    }
}

// ২. ১৬-বিট মানকে 'ষোড়শিক' পদ্ধতিতে রূপান্তর করে স্ট্রিমে লেখা
void fprint_bengali_hex(FILE *stream, uint16_t value) {
    fprintf(stream, "০ষ"); 

    uint8_t nibbles[4];
    nibbles[0] = (value >> 12) & 0xF;
    nibbles[1] = (value >> 8) & 0xF;
    nibbles[2] = (value >> 4) & 0xF;
    nibbles[3] = value & 0xF;

    int started = 0; 

    for (int i = 0; i < 4; i++) {
        uint8_t n = nibbles[i];

        if (!started && n == 0 && i < 3) continue;
        started = 1;

        if (n < 10) {
            fprintf(stream, "%s", BENGALI_DIGITS[n]);
        } else {
            fprintf(stream, "%s", BENGALI_HEX_ALPHA[n - 10]);
        }
    }
}

// ৩. মেমোরি অ্যাড্রেস প্রিন্ট (ফিক্সড ৪-ডিজিট ফরম্যাট)
void fprint_bengali_address(FILE *stream, uint16_t addr) {
    uint8_t nibbles[4];
    nibbles[0] = (addr >> 12) & 0xF;
    nibbles[1] = (addr >> 8) & 0xF;
    nibbles[2] = (addr >> 4) & 0xF;
    nibbles[3] = addr & 0xF;

    for (int i = 0; i < 4; i++) {
        uint8_t n = nibbles[i];
        if (n < 10) fprintf(stream, "%s", BENGALI_DIGITS[n]);
        else fprintf(stream, "%s", BENGALI_HEX_ALPHA[n - 10]);
    }
}