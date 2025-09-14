//
// Created by jvigu on 9/12/2025.
//

#ifndef TRIODE_MEMORY_H
#define TRIODE_MEMORY_H
#include <triode/arch/word.h>

namespace triode {

    struct EmulatorTestBench;

    class Memory {
    public:
        /**
         * @brief Makes a new memory obj. This is just a vector for now.
         * @param size size in trytes, remember a tryte is 6 bytes as each trit is a byte.
         */
        explicit Memory(const size_t size) : ram_(size, arch::Tryte{0}) {}

        /**
         * @brief Reads a single Word (4 trytes) from a specific memory address.
         */
        [[nodiscard]] arch::Word read_word(size_t address) const;

        /**
         * @brief Writes a single Word (4 trytes) to a specific memory address.
         */
        void write_word(size_t address, const arch::Word& word);

        /**
         * @brief A utility to write a block of trytes into memory.
         * @param buffer A vector of trytes to write.
         * @param address The starting address in memory to write the buffer.
         * @throws std::out_of_range if the write would go past the end of memory.
         */
        void write_buffer(size_t address, const std::vector<arch::Tryte>& buffer);

        [[nodiscard]] size_t size() const;
    private:
        std::vector<arch::Tryte> ram_;

        friend struct EmulatorTestBench;
    };

}


#endif //TRIODE_MEMORY_H