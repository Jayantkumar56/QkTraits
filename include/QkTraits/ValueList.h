

// SPDX-License-Identifier: MIT

#pragma once

#include "ValueTraits.h"

#include <array>


namespace Quirk::QkT {

    namespace Internal {
        // Helper for HasDuplicates
        template<auto... Values>
        struct HasDuplicatesVImpl;

        template<auto V1, auto... List>
        struct HasDuplicatesVImpl<V1, List...> {
            static constexpr bool Value = (EqualVal_V<V1, List> || ...) || (HasDuplicatesVImpl<List...>::Value);
        };

        template<auto V>
        struct HasDuplicatesVImpl<V> : FalseType {};

        template<>
        struct HasDuplicatesVImpl<> : FalseType {};
    } // namespace Internal


    template<auto... Values>
    struct ValueList {
    private:
        // Use a tuple to store values of potentially different types.
        constexpr static auto AsTuple = std::tuple{ Values... };

    public:
        constexpr static size_t Size          = sizeof...(Values);
        constexpr static bool   IsEmpty       = (Size == 0);
        constexpr static bool   HasDuplicates = Internal::HasDuplicatesVImpl<Values...>::Value;



        template<auto Key>
        constexpr static bool Contains = (EqualVal_V<Key, Values> || ...);

        // If found returns first occurence of a value in the list
        // If not found returns NPos
        template <auto Key>
        static constexpr size_t IndexOf = []() {
            std::array<bool, Size> matches{ EqualVal_V<Key, Values>... };

            for (size_t i = 0; i < matches.size(); ++i) {
                if (matches[i])
                    return i;         // found
            }

            return NPos;              // not found
        }();



        // Set to InvalidVal{} if list is empty.
        static constexpr auto Front = [] {
            if constexpr (IsEmpty) {
                return kInvalidVal;
            }
            else {
                return std::get<0>(AsTuple);
            }
        }();

        // Set to InvalidType{} if list is empty.
        static constexpr auto Back = [] {
            if constexpr (IsEmpty) {
                return kInvalidVal;
            } 
            else {
                return std::get<Size - 1>(AsTuple);
            }
        }();
        
        // Set to InvalidType{} if Index is out of bound of list.
        template<size_t Index>
        static constexpr auto Get = [] {
            if constexpr (Index >= Size) {
                return kInvalidVal;
            }
            else {
                return std::get<Index>(AsTuple);
            }
        }();



        constexpr static bool IsHomogeneous = [] {
            if constexpr (Size <= 1) {
                return true; // trivially homogeneous
            }
            else {
                return (std::is_same_v<std::decay_t<decltype(Front)>, std::decay_t<decltype(Values)>> && ...);
            }
        }();



        template<typename Function, typename ...Args>
        constexpr static void ForEach(Function&& func, Args&& ...args) {
            (func.template operator() < Values > (args...), ...);
        }

        template<typename Function, typename ...Args>
        constexpr static decltype(auto) InvokeWithValuesExpanded(Function&& func, Args&& ...args) {
            return std::forward<Function>(func).template operator() < Values... > (std::forward<Args>(args)...);
        }
    };


    template<typename List>
    static constexpr auto ToTupleVal = std::tuple<>{};

    template<auto ...Values>
    static constexpr auto ToTupleVal<ValueList<Values...>> = std::tuple<>{Values...};



    template<typename List, auto Element>
    struct PushFrontVal;

    template<auto Element, auto ...Elements>
    struct PushFrontVal<ValueList<Elements...>, Element> {
        using Type = ValueList<Element, Elements...>;
    };

    template<typename List, auto Element>
    using PushFrontVal_T = typename PushFrontVal<List, Element>::Type;



    template<typename List, auto Element>
    struct PushBackVal;

    template<auto Element, auto ...Elements>
    struct PushBackVal<ValueList<Elements...>, Element> {
        using Type = ValueList<Elements..., Element>;
    };

    template<typename List, auto Element>
    using PushBackVal_T = typename PushBackVal<List, Element>::Type;



    template<typename List>
    struct PopFrontVal;

    template<auto Head, auto... Tail>
    struct PopFrontVal<ValueList<Head, Tail...>> {
        using Type = ValueList<Tail...>;
    };

    template<typename List>
    using PopFrontVal_T = typename PopFrontVal<List>::Type;



    namespace Internal {
        template<typename List>
        struct PopBackValImpl;

        template<auto Last>
        struct PopBackValImpl<ValueList<Last>> {
            using Type = ValueList<>;
        };

        template<auto Head, auto... Tail>
        struct PopBackValImpl<ValueList<Head, Tail...>> {
            using Type = PushFrontVal_T<typename PopBackValImpl<ValueList<Tail...>>::Type, Head>;
        };

        template<>
        struct PopBackValImpl<ValueList<>> {
            using Type = ValueList<>;
        };
    } // namespace Internal

    template<typename List>
    struct PopBackVal {
        using Type = typename Internal::PopBackValImpl<List>::Type;
    };

    template<typename List>
    using PopBackVal_T = typename PopBackVal<List>::Type;



    template<typename ListA, typename ListB>
    struct ConcatVal;

    template<auto... ValuesA, auto... ValuesB>
    struct ConcatVal<ValueList<ValuesA...>, ValueList<ValuesB...>> {
        using Type = ValueList<ValuesA..., ValuesB...>;
    };

    template<typename ListA, typename ListB>
    using ConcatVal_T = typename ConcatVal<ListA, ListB>::Type;



    template<typename List, template<auto> typename MetaFunc>
    struct TransformVal;

    template<template<auto> typename MetaFunc, auto ...Values>
    struct TransformVal<ValueList<Values...>, MetaFunc> {
        using Type = ValueList<MetaFunc<Values>::Value...>;
    };

    template<typename List, template<auto> typename MetaFunc>
    using TransformVal_T = typename TransformVal<List, MetaFunc>::Type;



    namespace Internal {
        template<typename List, template<auto> typename Predicate>
        struct FilterValImpl;

        template<template<auto> typename Predicate, auto Head, auto... Tail>
        struct FilterValImpl<ValueList<Head, Tail...>, Predicate> {
            using Next = typename FilterValImpl<ValueList<Tail...>, Predicate>::Type;

            using Type = std::conditional_t<
                             Predicate<Head>::Value,
                             ConcatVal_T<ValueList<Head>, Next>,   // If true, keep the Head
                             Next                               // If false, discard the Head
                         >;
        };

        template<template<auto> typename Predicate>
        struct FilterValImpl<ValueList<>, Predicate> {
            using Type = ValueList<>;
        };
    } // namespace Internal

    template<typename List, template<auto> typename Predicate>
    struct FilterVal {
        using Type = typename Internal::FilterValImpl<List, Predicate>::Type;
    };

    template<typename List, template<auto> typename Predicate>
    using FilterVal_T = typename FilterVal<List, Predicate>::Type;



    template<typename List>
    struct ReverseVal;

    template<>
    struct ReverseVal<ValueList<>> {
        using Type = ValueList<>;
    };

    template<auto Head, auto... Tail>
    struct ReverseVal<ValueList<Head, Tail...>> {
        // Recursively reverse the tail and then push the head to the back
        using Type = PushBackVal_T<typename ReverseVal<ValueList<Tail...>>::Type, Head>;
    };

    template<typename List>
    using ReverseVal_T = typename ReverseVal<List>::Type;

} // namespace Quirk::QkT
