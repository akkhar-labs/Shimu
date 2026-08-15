/* 
 * formatting.h - আউটপুট ফরম্যাটিং ইঞ্জিনের প্রোটোটাইপ (ফাইল সাপোর্ট সহ)
 * আর্কিটেক্ট: রাহাত হাসান | অক্ষর ল্যাবস
 */

#ifndef FORMATTING_H
#define FORMATTING_H

#include <stdint.h>
#include <stdio.h> // FILE স্ট্রাকচারের জন্য প্রয়োজন

/* --- ফাংশন প্রোটোটাইপসমূহ --- */

/**
 * একটি ইন্টিজার মানকে বাংলা সংখ্যায় রূপান্তর করে নির্দিষ্ট স্ট্রিমে (stdout বা ফাইলে) লেখে।
 */
void fprint_bengali_int(FILE *stream, int value);

/**
 * একটি ১৬-বিট মানকে আপনার 'ষোড়শিক' পদ্ধতিতে রূপান্তর করে নির্দিষ্ট স্ট্রিমে লেখে।
 */
void fprint_bengali_hex(FILE *stream, uint16_t value);

/**
 * মেমোরি অ্যাড্রেস প্রিন্ট করার জন্য বিশেষ ফরম্যাট।
 */
void fprint_bengali_address(FILE *stream, uint16_t addr);

#endif // FORMATTING_H