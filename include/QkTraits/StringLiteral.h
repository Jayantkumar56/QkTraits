

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

        constexpr const char*      Data() const noexcept { return value;           }
        constexpr std::string_view View() const noexcept { return { value, N -1 }; }
        constexpr size_t           Size() const noexcept { return N - 1;           }

        constexpr char operator[](size_t i) const noexcept {
            assert(i < N);
            return value[i]; 
        }

        constexpr bool operator==(const StringLiteral& other) const noexcept {
            for (size_t i = 0; i < N; ++i) {
                if (value[i] != other.value[i])
                    return false;
            }

            return true;
        }
    };

    template<size_t N>
    StringLiteral(const char(&str)[N]) -> StringLiteral<N>;

}
