

// SPDX-License-Identifier: MIT

#pragma once

#include "TypeTraits.h"


namespace Quirk::QkT {

    static constexpr size_t NPos = static_cast<size_t>(-1);



    struct InvalidVal {
        constexpr bool operator==(const InvalidVal&) const = default;
    };

    inline constexpr InvalidVal kInvalidVal{};      // canonical InvalidVal instance

    template<auto Value>
    inline constexpr bool IsInvalidVal_V = std::is_same_v<std::remove_cv_t<decltype(Value)>, InvalidVal>;

    template<auto Value>
    concept ValidValue = (!IsInvalidVal_V<Value>);



    template<auto Value1, auto Value2>
    struct EqualVal {
        static constexpr bool Value = [] {
            if constexpr (std::is_same_v<decltype(Value1), decltype(Value2)>) {
                return Value1 == Value2;
            }
            else {
                return false;
            }
        }();
    };

    template<auto Value1, auto Value2>
    inline constexpr bool EqualVal_V = EqualVal<Value1, Value2>::Value;

}
