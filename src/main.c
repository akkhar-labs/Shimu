/* 
 * main.c - The Binary Loader and Entry Point for Shimu Simulator
 * Updated: Integrated with modular Memory Management system.
 * Architect: Rahat Hasan | Akkhar Labs
 */

#include "shimu.h"
#include "cpu.h"
#include "memory.h"
#include "utils/formatting.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    printf("--- শিমু (Shimu) সিমুলেটর v১.০ ---\n");
    printf("স্থাপত্য: নির্দেশিকা-১৬ | আর্কিটেক্ট: রাহাত হাসান\n\n");

    // ১. কমান্ড লাইন আর্গুমেন্ট চেক করা
    if (argc < 2) {
        printf("ব্যবহার: shimu <ফাইল_পাথ.ব>\n");
        return 1;
    }

    // ২. মেমোরি এবং ওকোড ব্যবস্থা প্রস্তুত করা
    init_memory();

    // ৩. বাইনারি (.ব) ফাইল ওপেন করা
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        printf("ভুল: ফাইলটি খোলা সম্ভব হয়নি! পাথ চেক করুন।\n");
        return 1;
    }

    // ৪. আউটপুট লগ ফাইল ওপেন করা (ইনপুট ফাইলের পাশে)
    char output_path[1024];
    const char *last_slash = strrchr(argv[1], '/');
    if (last_slash) {
        size_t dir_len = last_slash - argv[1] + 1;
        snprintf(output_path, sizeof(output_path), "%.*sসিমুলেটর_ফলাফল.txt", (int)dir_len, argv[1]);
    } else {
        snprintf(output_path, sizeof(output_path), "%s", "সিমুলেটর_ফলাফল.txt");
    }
    FILE *log_file = fopen(output_path, "w");
    if (!log_file) {
        printf("ভুল: আউটপুট লগ ফাইল তৈরি করা যায়নি।\n");
        fclose(file);
        return 1;
    }

    // ৫. ফাইল থেকে ভার্চুয়াল মেমোরিতে র-বাইনারি লোড করা
    // সরাসরি মেমোরি মডিউলের ইন্টারনাল বাফারে রিড করা হচ্ছে
    size_t bytes_read = fread(get_memory_ptr(), 1, MEMORY_SIZE, file);
    printf("সিস্টেম: %zu বাইট মেমোরিতে লোড করা হয়েছে।\n", bytes_read);
    fclose(file);

    // লগে প্রাথমিক তথ্য লেখা
    fprintf(log_file, "--- নির্দেশিকা-১৬ সিমুলেশন রিপোর্ট ---\n");
    fprintf(log_file, "আর্কিটেক্ট: রাহাত হাসান | অক্ষর ল্যাবস\n\n");

    // ৬. সিপিইউ রিসেট এবং শুরু করা
    CPU cpu;
    init_cpu(&cpu);

    printf("সিস্টেম: এক্সিকিউশন শুরু হচ্ছে... ফলাফল '%s' ফাইলে দেখুন।\n", output_path);
    
    // ৭. সিমুলেশন লুপ (Fetch-Decode-Execute Cycle)
    while (!cpu.is_halted) {
        // মেমোরি মডিউল এখন ইন্টারনালভাবে হ্যান্ডেল হবে
        step_cpu(&cpu, log_file);
        
        if (cpu.pc >= MEMORY_SIZE) {
            fprintf(log_file, "\nভুল: মেমোরি আউট-অফ-বাউন্ড এরর!\n");
            break;
        }
    }

    // ৮. চূড়ান্ত অবস্থা লগে দেখানো
    fprintf(log_file, "\nসিস্টেম: এক্সিকিউশন সফলভাবে সম্পন্ন হয়েছে।\n");
    dump_cpu_state(&cpu, log_file);

    fclose(log_file);
    printf("---------------------------------------\n");
    printf("সফল: সিমুলেশন রিপোর্ট তৈরি হয়েছে।\n");

    return 0;
}