#ifndef TRIODE_TYPES_H
#define TRIODE_TYPES_H

#include <array>
#include <cstdint>
#include <iostream>
#include <triode/arch/utils.h>
#include <triode/arch/trit.h>

// NOTE: All types here are generated in Little-Endian.

namespace triode::arch {
    constexpr size_t TRITS_IN_TRYTE = 6;
    constexpr int TRYTE_MAX = (pow3(TRITS_IN_TRYTE) - 1)/2;
    constexpr int TRYTE_MIN = -TRYTE_MAX;

    struct Tryte {
        std::array<Trit, TRITS_IN_TRYTE> trits_{};

        Tryte() = default;

        Tryte(std::initializer_list<int>);

        // Constructs a tryte from an array of trits. The array must be exactly TRITS_IN_TRYTE long.
        Tryte(const std::array<Trit, TRITS_IN_TRYTE> array) : trits_{array} {}; // NOLINT
        // Constructs a tryte from a decimal value. See limits TRYTE_MAX and TRYTE_MIN.
        Tryte(int); //NOLINT

        Tryte(const Tryte& other) = default;
        Tryte(Tryte&& other) noexcept = default;
        Tryte& operator=(const Tryte& other) = default;
        Tryte& operator=(Tryte&& other) noexcept = default;
        ~Tryte() = default;

        [[nodiscard]] int to_int() const;

        [[nodiscard]] Tryte negate() const;

        // No lint here as Trit is the carry. In some cases like Tryte + Tryte, we don't need the carry.
        //NOLINTNEXTLINE
        Trit full_add(const Tryte& other, const Trit carry_in = Trit::ZERO);

        auto operator<=>(const Tryte& rhs) const -> std::strong_ordering;
        auto operator==(const Tryte& rhs) const -> bool;
        auto operator!=(const Tryte& rhs) const -> bool;

        auto operator+=(const Tryte &rhs) -> Tryte&;

        auto operator-=(const Tryte& rhs) -> Tryte&;
    };


    auto operator+(const Tryte& lhs, const Tryte& rhs) -> Tryte;

    auto operator-(const Tryte& tryte) -> Tryte;

    std::ostream& operator<<(std::ostream& os, Tryte tryte);
}
#endif //TRIODE_TYPES_H