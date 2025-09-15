//
// Created by jvigu on 9/14/2025.
//
#include <chrono>
#include <numeric>
#include <triode/emulator.h>
int main() {
    // for what we are doing here calculated that we only need 4 words.
    triode::Emulator emulator{4 * triode::arch::TRYTES_IN_WORD};
    const std::string filename = "add_two_and_two.tbin";
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    emulator.load_from_tbin(file);
    const int num_iterations = 100; // Number of runs to average
    std::vector<double> results;
    results.reserve(num_iterations);

    for (int i = 0; i < num_iterations; ++i) {
        // Run a "warm-up" iteration to prime caches and mitigate initial overhead
        if (i == 0) {
            emulator.run();
        }

        const auto begin = std::chrono::steady_clock::now();
        emulator.run();
        const auto end = std::chrono::steady_clock::now();
        const std::chrono::duration<double, std::micro> diff = end - begin;
        results.push_back(diff.count());
    }

    // Calculate statistics
    double sum = std::accumulate(results.begin(), results.end(), 0.0);
    double average = sum / num_iterations;
    std::sort(results.begin(), results.end());
    double median = results[num_iterations / 2];

    std::cout << "Average time: " << average << " microseconds" << std::endl;
    std::cout << "Median time: " << median << " microseconds" << std::endl;    return 0;
}