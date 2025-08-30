

// SPDX-License-Identifier: MIT

#pragma once

#include "ValueList.h"
#include "StringLiteral.h"


namespace Quirk::QkT {

    // Deligates to ValueList internally
	template<StringLiteral ...Strings>
	struct StringList {
	private:
		using ListImpl = ValueList<Strings...>;

    public:
        constexpr static size_t Size          = ListImpl::Size;
        constexpr static bool   IsEmpty       = ListImpl::IsEmpty;
        constexpr static bool   HasDuplicates = ListImpl::HasDuplicates;
        constexpr static bool   IsHomogeneous = ListImpl::IsHomogeneous;


        template<StringLiteral Key>
        constexpr static bool Contains = ListImpl::template Contains<Key>;

        // If found returns first occurence of a value in the list
        // If not found returns NPos
        template <StringLiteral Key>
        static constexpr size_t IndexOf = ListImpl::template IndexOf<Key>;



        // Set to InvalidVal{} if list is empty.
        static constexpr auto Front = ListImpl::Front;

        // Set to InvalidType{} if list is empty.
        static constexpr auto Back = ListImpl::Back;

        // Set to InvalidType{} if Index is out of bound of list.
        template<size_t Index>
        static constexpr auto Get = ListImpl::template Get<Index>;



        template<typename Function, typename ...Args>
        constexpr static void ForEach(Function&& func, Args&& ...args) {
            ListImpl::ForEach(std::forward<Function>(func), std::forward<Args>(args)...);
        }

        template<typename Function, typename ...Args>
        constexpr static decltype(auto) InvokeWithStringsExpanded(Function&& func, Args&& ...args) {
            return ListImpl::InvokeWithValuesExpanded(std::forward<Function>(func), std::forward<Args>(args)...);
        }
	};


    
    template<typename List>
    static constexpr auto ToTupleStr = std::tuple<>{};

    template<StringLiteral ...Strings>
    static constexpr auto ToTupleStr<StringList<Strings...>> = std::tuple<>{ Strings... };



    template<typename List, auto String>
    struct PushFrontStr;

    template<StringLiteral String, StringLiteral ...Elements>
    struct PushFrontStr<StringList<Elements...>, String> {
        using Type = StringList<String, Elements...>;
    };

    template<typename List, StringLiteral String>
    using PushFrontStr_T = typename PushFrontStr<List, String>::Type;



    template<typename List, StringLiteral String>
    struct PushBackStr;

    template<StringLiteral String, StringLiteral ...Elements>
    struct PushBackStr<StringList<Elements...>, String> {
        using Type = StringList<Elements..., String>;
    };

    template<typename List, StringLiteral String>
    using PushBackStr_T = typename PushBackStr<List, String>::Type;



    template<typename List>
    struct PopFrontStr;

    template<StringLiteral Head, StringLiteral... Tail>
    struct PopFrontStr<StringList<Head, Tail...>> {
        using Type = StringList<Tail...>;
    };

    template<typename List>
    using PopFrontStr_T = typename PopFrontStr<List>::Type;



    namespace Internal {
        template<typename List>
        struct PopBackStrImpl;

        template<StringLiteral Last>
        struct PopBackStrImpl<StringList<Last>> {
            using Type = StringList<>;
        };

        template<StringLiteral Head, StringLiteral... Tail>
        struct PopBackStrImpl<StringList<Head, Tail...>> {
            using Type = PushFrontStr_T<typename PopBackStrImpl<StringList<Tail...>>::Type, Head>;
        };

        template<>
        struct PopBackStrImpl<StringList<>> {
            using Type = StringList<>;
        };
    } // namespace Internal

    template<typename List>
    struct PopBackStr {
        using Type = typename Internal::PopBackStrImpl<List>::Type;
    };

    template<typename List>
    using PopBackStr_T = typename PopBackStr<List>::Type;

    

    template<typename ListA, typename ListB>
    struct ConcatStr;

    template<StringLiteral... StringsA, StringLiteral... StringsB>
    struct ConcatStr<StringList<StringsA...>, StringList<StringsB...>> {
        using Type = StringList<StringsA..., StringsB...>;
    };

    template<typename ListA, typename ListB>
    using ConcatStr_T = typename ConcatStr<ListA, ListB>::Type;



    template<typename List, template<StringLiteral> typename MetaFunc>
    struct TransformStr;

    template<template<StringLiteral> typename MetaFunc, StringLiteral ...Strings>
    struct TransformStr<StringList<Strings...>, MetaFunc> {
        using Type = StringList<MetaFunc<Strings>::Value...>;
    };

    template<typename List, template<StringLiteral> typename MetaFunc>
    using TransformStr_T = typename TransformStr<List, MetaFunc>::Type;



    namespace Internal {
        template<typename List, template<StringLiteral> typename Predicate>
        struct FilterStrImpl;

        template<template<StringLiteral> typename Predicate, StringLiteral Head, StringLiteral... Tail>
        struct FilterStrImpl<StringList<Head, Tail...>, Predicate> {
            using Next = typename FilterStrImpl<StringList<Tail...>, Predicate>::Type;

            using Type = std::conditional_t<
                Predicate<Head>::Value,
                ConcatStr_T<StringList<Head>, Next>,   // If true, keep the Head
                Next                               // If false, discard the Head
            >;
        };

        template<template<StringLiteral> typename Predicate>
        struct FilterStrImpl<StringList<>, Predicate> {
            using Type = StringList<>;
        };
    } // namespace Internal

    template<typename List, template<StringLiteral> typename Predicate>
    struct FilterStr {
        using Type = typename Internal::FilterStrImpl<List, Predicate>::Type;
    };

    template<typename List, template<StringLiteral> typename Predicate>
    using FilterStr_T = typename FilterStr<List, Predicate>::Type;



    template<typename List>
    struct ReverseStr;

    template<>
    struct ReverseStr<StringList<>> {
        using Type = StringList<>;
    };

    template<auto Head, auto... Tail>
    struct ReverseStr<StringList<Head, Tail...>> {
        // Recursively reverse the tail and then push the head to the back
        using Type = PushBackStr_T<typename ReverseStr<StringList<Tail...>>::Type, Head>;
    };

    template<typename List>
    using ReverseStr_T = typename ReverseStr<List>::Type;

}
