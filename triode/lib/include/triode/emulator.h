//
// Created by jvigu on 9/12/2025.
//

#ifndef TRIODE_EMULATOR_H
#define TRIODE_EMULATOR_H

#include <fstream>
#include <vector>
#include <triode/cpu.h>
#include <triode/memory.h>

namespace triode {

    struct EmulatorTestBench;

    class Emulator {
    public:
        explicit Emulator(const size_t memory_size) : memory_(memory_size), cpu_() {}

        void load(const std::vector<arch::Tryte>& program) {
            memory_.write_buffer(0, program); // Load program at address 0
        }

        void load_from_tbin(std::ifstream& tbin) {
            if (!tbin.is_open()) {
                throw std::runtime_error("Emulator::load_from_tbin: tbin file is not open");
            }

            tbin.seekg(0, std::ios::end);
            const std::streampos size_in_bytes = tbin.tellg();
            tbin.seekg(0, std::ios::beg);

            std::vector<arch::Tryte> buffer;
            // let's do math for reserve. In this encoded long long format. Each word is 8 bytes
            // A word is 4 trytes. So Bytes / 4 gets us our amount of trytes.
            buffer.reserve(size_in_bytes / 4);

            while (tbin) {
                int64_t value;
                tbin.read(reinterpret_cast<char*>(&value), sizeof(value));
                if (tbin.gcount() == sizeof(value)) {
                    const arch::Word x{value};
                    buffer.push_back(x.trytes_[0]);
                    buffer.push_back(x.trytes_[1]);
                    buffer.push_back(x.trytes_[2]);
                    buffer.push_back(x.trytes_[3]);
                }
            }
            memory_.write_buffer(0, buffer);
        }

        void run() {
            cpu_.reset();
            while(!cpu_.is_halted()) {
                cpu_.cycle(memory_);
            }
        }

        /**
         * @brief Provides access to the CPU component.
         * @return A mutable reference to the CPU.
         */
        [[nodiscard]] CPU& get_cpu() { return cpu_; }

        /**
         * @brief Provides access to the Memory component.
         * @return A mutable reference to the Memory.
         */
        [[nodiscard]] Memory& get_memory() { return memory_; }
    private:
        Memory memory_;
        CPU cpu_;
        friend EmulatorTestBench;
        friend struct EmulatorTestBench;
    };
}

#endif //TRIODE_EMULATOR_H