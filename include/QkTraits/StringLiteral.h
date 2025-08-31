

// SPDX-License-Identifier: MIT

#pragma once

#include <string_view>
#include <cassert>


namespace Quirk::QkT {

    template <size_t N>
    struct StringLiteral {
        char value[N]{};

        // Default constructor: zero-initialize the buffer
        constexpr StringLiteral() noexcept : value{} {}

        // Construct from a string literal
        constexpr StringLiteral(const char(&str)[N]) noexcept {
            for (size_t i = 0; i < N; ++i)
                value[i] = str[i];
        }

        // ==== Data Acess Functions =================================================
        // Following c++ standard for size of string 
        // (i.e. neglecting the null terminator at the end)

        constexpr const char*      Data() const noexcept { return value;           }
        constexpr std::string_view View() const noexcept { return { value, N -1 }; }
        constexpr size_t           Size() const noexcept { return N - 1;           }

        constexpr char operator[](size_t idx) const noexcept {
            assert(idx < N);
            return value[idx];
        }

        // ==== Comparision Function =================================================
        constexpr auto operator<=>(const StringLiteral& other) const noexcept = default;

        // ==== Concatenation ======================================================
        template <size_t N2>
        constexpr auto operator+(const StringLiteral<N2>& rhs) const {
            constexpr size_t NewSize = N + N2 - 1; // drop one '\0'
            StringLiteral<NewSize> result{};       // zero-init

            // Copy lhs (without its null terminator)
            for (size_t i = 0; i < N - 1; ++i) {
                result.value[i] = value[i];
            }

            // Copy rhs (including its null terminator)
            for (size_t j = 0; j < N2; ++j) {
                result.value[(N - 1) + j] = rhs.value[j];
            }

            return result;
        }
    };


    // Deduction guide for convenient StringLiteral object creation from standard string literal, ex:
    // StringLiteral str{"hello"};

    template<size_t N>
    StringLiteral(const char(&str)[N]) -> StringLiteral<N>;

} // namespace Quirk::QkT
