//
// Created by jvigu on 9/12/2025.
//

#include <triode/memory.h>

namespace triode {
    arch::Word Memory::read_word(const size_t address) const {
        return arch::Word(ram_[address], ram_[address + 1], ram_[address + 2], ram_[address + 3]);
    }

    void Memory::write_word(const size_t address, const arch::Word &word) {
        ram_[address] = word.trytes_[0];
        ram_[address + 1] = word.trytes_[1];
        ram_[address + 2] = word.trytes_[2];
        ram_[address + 3] = word.trytes_[3];
    }

    void Memory::write_buffer(const size_t address, const std::vector<arch::Tryte> &buffer) {
        for (int i = 0; i < buffer.size(); i++) {
            ram_[address + i] = buffer[i];
        }
    }

    size_t Memory::size() const {
        return ram_.size();
    }
}
