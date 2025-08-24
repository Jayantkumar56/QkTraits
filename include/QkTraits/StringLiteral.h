

// SPDX-License-Identifier: MIT

#pragma once

#include <string_view>
#include <cassert>


namespace Quirk::QkT {

    template <size_t N>
    struct StringLiteral {
        char value[N]{};

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

        constexpr bool operator==(const StringLiteral& other) const noexcept {
            // Checking size difference is not necessary 
            // since StringLiteral with different sizes are different types themselves.

            for (size_t i = 0; i < N; ++i) {
                if (value[i] != other.value[i])
                    return false;
            }

            return true;
        }
    };


    // Deduction guide for convenient StringLiteral object creation from standard string literal, ex:
    // StringLiteral str{"hello"};

    template<size_t N>
    StringLiteral(const char(&str)[N]) -> StringLiteral<N>;

} // namespace Quirk::QkT
