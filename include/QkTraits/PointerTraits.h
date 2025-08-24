

// SPDX-License-Identifier: MIT

#pragma once

#include "TypeTraits.h"

#include <memory>


namespace Quirk::QkT {

    namespace internal {
        template<typename T>
        struct IsSmartPointerImpl : FalseType {};

        template<typename T>
        struct IsSmartPointerImpl<std::shared_ptr<T>> : TrueType {};

        template<typename T>
        struct IsSmartPointerImpl<std::unique_ptr<T>> : TrueType {};
    }

    template<typename T>
    struct IsSmartPointer : internal::IsSmartPointerImpl<std::remove_cvref_t<T>> {};

    template<typename T>
    constexpr bool IsSmartPointer_V = IsSmartPointer<T>::Value;

    template<typename T>                                                     // Concept
    concept SmartPointer = IsSmartPointer_V<T>;



    template<typename T>
    struct IsRawPointer { static constexpr bool Value = std::is_pointer_v<std::remove_cvref_t<T>>; };

    template<typename T>
    constexpr bool IsRawPointer_V = IsRawPointer<T>::Value;

    template<typename T>                                                     // Concept
    concept RawPointer = IsRawPointer_V<T>;



    template<typename T>
    struct IsPointer { static constexpr bool Value = IsRawPointer_V<T> || IsSmartPointer_V<T>; };

    template<typename T>
    constexpr bool IsPointer_V = IsPointer<T>::Value;

    template<typename T>                                                     // Concept
    concept Pointer = IsPointer_V<T>;



    template<typename T>
    struct PointingType { using Type = std::remove_pointer_t<T>; };

    template<typename T>
    struct PointingType<std::shared_ptr<T>> { using Type = T; };

    template<typename T>
    struct PointingType<std::unique_ptr<T>> { using Type = T; };

    template<typename T>
    using PointingType_T = typename PointingType<std::remove_cvref_t<T>>::Type;

} // namespace Quirk::QkT
