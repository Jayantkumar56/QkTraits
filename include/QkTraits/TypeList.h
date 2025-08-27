

// SPDX-License-Identifier: MIT

#pragma once

#include "TypeTraits.h"

#include <utility>
#include <tuple>
#include <array>


namespace Quirk::QkT {

    namespace Internal {
        template<typename... Types>
        struct HasDuplicatesImpl;

        template<typename Type1, typename... List>
        struct HasDuplicatesImpl<Type1, List...> {
            static constexpr bool Value = (std::is_same_v<Type1, List> || ...) || (HasDuplicatesImpl<List...>::Value);
        };

        template<typename Type>
        struct HasDuplicatesImpl<Type> : FalseType {};

        template<>
        struct HasDuplicatesImpl<> : FalseType {};


        template<typename ...Types>
        struct FirstNonVoidImpl {
            using Type = InvalidType;
        };

        template<typename Type1, typename ...Types>
        struct FirstNonVoidImpl<Type1, Types...> {
            using Type = std::conditional_t<
                            !std::is_void_v<Type1>, 
                            Type1,
                            typename FirstNonVoidImpl<Types...>::Type
                        >;
        };


        template<typename ...Types>
        struct FrontImpl {
            using Type = InvalidType;
        };

        template<typename Type1, typename ...Types>
        struct FrontImpl<Type1, Types...> {
            using Type = Type1;
        };


        template<typename ...Types>
        struct BackImpl {
            using Type = InvalidType;
        };

        template<typename Type1, typename ...Types>
        struct BackImpl<Type1, Types...> {
            using Type = std::conditional_t<
                             sizeof...(Types) == 0,
                             Type1,
                             typename BackImpl<Types...>::Type
                         >;
        };
    } // namespace Internal


    template<typename ...Types>
    struct TypeList {
        // set to InvalidType if only void types are in the list or list is empty
        using FirstNonVoid = typename Internal::FirstNonVoidImpl<Types...>::Type;

        // set to InvalidType if list is empty
        using Front = typename Internal::FrontImpl<Types...>::Type;

        // set to InvalidType if list is empty
        using Back = typename Internal::BackImpl<Types...>::Type;

        template<size_t Index>
        using Get = std::tuple_element_t<Index, std::tuple<Types...>>;



        constexpr static size_t Size    = sizeof...(Types);
        constexpr static bool   IsEmpty = (Size == 0);
        constexpr static bool   HasDuplicates = Internal::HasDuplicatesImpl<Types...>::Value;

        template<typename T>
        constexpr static bool Contains = (std::is_same_v<T, Types> || ...);

        static constexpr std::size_t NPos = static_cast<std::size_t>(-1);

        // if found returns first occurence of a type in the list
        // if not found returns NPos
        template <typename T>
        static constexpr std::size_t IndexOf = [] {
            constexpr std::array<bool, sizeof...(Types)> matches{ std::is_same_v<T, Types>... };

            for (std::size_t i = 0; i < matches.size(); ++i) {
                if (matches[i]) 
                    return i;         // found
            }

            return NPos;              // not found
        }();



        template<typename Function, typename ...Args>
        constexpr static void ForEach(Function&& func, Args&& ...args) {
            (func.template operator() <Types> (args...), ...);
        }

        template<typename Function, typename ...Args>
        constexpr static decltype(auto) InvokeWithTypesExpanded(Function&& func, Args&& ...args) {
            return std::forward<Function>(func).template operator() <Types...> (std::forward<Args>(args)...);
        }
    };


    template<typename List, typename Element>
    struct PushFront;

    template<typename Element, typename ...Elements>
    struct PushFront<TypeList<Elements...>, Element> {
        using Type = TypeList<Element, Elements...>;
    };

    template<typename List, typename Element>
    using PushFront_T = typename PushFront<List, Element>::Type;



    template<typename List, typename Element>
    struct PushBack;

    template<typename Element, typename ...Elements>
    struct PushBack<TypeList<Elements...>, Element> {
        using Type = TypeList<Elements..., Element>;
    };

    template<typename List, typename Element>
    using PushBack_T = typename PushBack<List, Element>::Type;



    template<typename List>
    struct PopFront;

    template<typename Head, typename... Tail>
    struct PopFront<TypeList<Head, Tail...>> {
        using Type = TypeList<Tail...>;
    };

    template<typename List>
    using PopFront_T = typename PopFront<List>::Type;



    namespace Internal {
        template<typename List>
        struct PopBackImpl;

        template<typename Last>
        struct PopBackImpl<TypeList<Last>> {
            using Type = TypeList<>;
        };

        template<typename Head, typename... Tail>
        struct PopBackImpl<TypeList<Head, Tail...>> {
            using Type = PushFront_T<typename PopBackImpl<TypeList<Tail...>>::Type, Head>;
        };

        template<>
        struct PopBackImpl<TypeList<>> {
            using Type = TypeList<>;
        };
    } // namespace Internal


    template<typename List>
    struct PopBack;

    template<typename... Elements>
    struct PopBack<TypeList<Elements...>> {
        using Type = typename Internal::PopBackImpl<TypeList<Elements...>>::Type;
    };

    template<typename List>
    using PopBack_T = typename PopBack<List>::Type;



    template<typename ListA, typename ListB>
    struct Concat;

    template<typename... TypesA, typename... TypesB>
    struct Concat<TypeList<TypesA...>, TypeList<TypesB...>> {
        using Type = TypeList<TypesA..., TypesB...>;
    };

    template<typename ListA, typename ListB>
    using Concat_T = typename Concat<ListA, ListB>::Type;



    template<typename List, template<typename> typename MetaFunc>
    struct Transform;

    template<template<typename> typename MetaFunc, typename ...Elements>
    struct Transform<TypeList<Elements...>, MetaFunc> {
        using Type = TypeList<typename MetaFunc<Elements>::Type...>;
    };

    template<typename List, template<typename> typename MetaFunc>
    using Transform_T = typename Transform<List, MetaFunc>::Type;



    namespace Internal {
        template<typename List, template<typename> typename Predicate>
        struct FilterImpl;

        template<template<typename> typename Predicate, typename Head, typename... Tail>
        struct FilterImpl<TypeList<Head, Tail...>, Predicate> {
            using Next = typename FilterImpl<TypeList<Tail...>, Predicate>::Type;

            using Type = std::conditional_t<
                             Predicate<Head>::Value,
                             Concat_T<TypeList<Head>, Next>,   // If true, keep the Head
                             Next                              // If false, discard the Head
                         >;
        };

        template<template<typename> typename Predicate>
        struct FilterImpl<TypeList<>, Predicate> {
            using Type = TypeList<>;
        };
    } // namespace Internal


    template<typename List, template<typename> typename Predicate>
    struct Filter {
        using Type = typename Internal::FilterImpl<List, Predicate>::Type;
    };

    template<typename List, template<typename> typename Predicate>
    using Filter_T = typename Filter<List, Predicate>::Type;

} // namespace Quirk::QkT
