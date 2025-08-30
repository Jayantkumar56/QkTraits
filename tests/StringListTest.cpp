

#include <QkTraits/StringList.h>
#include <gtest/gtest.h>

#include <string>
#include <tuple>
#include <vector>


namespace Quirk::QkT {

    // =============================================================================
    // Helper Functors
    // =============================================================================

    struct StringCollector {
        std::vector<std::string>& collected;
        template<StringLiteral S>
        void operator()() {
            collected.push_back(std::string(S.View()));
        }
    };

    struct StringLengthSummer {
        size_t& total;
        template<StringLiteral S>
        void operator()(size_t base) {
            total += S.Size() + base;
        }
    };

    struct TupleFactory {
        template<StringLiteral... Ss>
        constexpr auto operator()() const {
            return std::tuple{ Ss... }; // tuple of StringLiterals
        }
    };

    struct ArgForwarder {
        template<StringLiteral... Ss>
        auto operator()(int& count) const {
            count = sizeof...(Ss);
            return std::string("Processed");
        }
    };

    // Meta-func: Identity transform
    template<StringLiteral S>
    struct Identity {
        static constexpr auto Value = S;
    };

    // Predicate: keep only strings starting with 'A'
    template<StringLiteral S>
    struct StartsWithA {
        static constexpr bool Value = (S[0] == 'A');
    };


    // =============================================================================
    // Test Cases
    // =============================================================================

    TEST(StringListTest, FrontAndBack) {
        {
            using L = StringList<"Hello", "World">;
            static_assert(L::Front == StringLiteral{ "Hello" });
            static_assert(L::Back  == StringLiteral{ "World" });
        }
        {
            using Single = StringList<"One">;
            static_assert(Single::Front == StringLiteral{ "One" });
            static_assert(Single::Back  == StringLiteral{ "One" });
        }
        {
            using Empty = StringList<>;
            static_assert(Empty::Front == kInvalidVal);
            static_assert(Empty::Back  == kInvalidVal);
        }
        ASSERT_TRUE(true);
    }

    TEST(StringListTest, Get) {
        using L = StringList<"Red", "Green", "Blue">;

        static_assert(L::Get<0>  == StringLiteral{ "Red" });
        static_assert(L::Get<2>  == StringLiteral{ "Blue" });
        static_assert(L::Get<99> == kInvalidVal);

        ASSERT_TRUE(true);
    }

    TEST(StringListTest, SizeAndFlags) {
        {
            using L = StringList<"A", "B", "C">;
            static_assert(L::Size == 3);
            static_assert(!L::IsEmpty);
        }
        {
            using Empty = StringList<>;
            static_assert(Empty::Size == 0);
            static_assert(Empty::IsEmpty);
        }
        {
            using Dups = StringList<"A", "B", "A">;
            static_assert(Dups::HasDuplicates);
        }
        {
            using Mixed = StringList<"A", "B">;
            static_assert(Mixed::IsHomogeneous);
        }
        ASSERT_TRUE(true);
    }

    TEST(StringListTest, ContainsAndIndexOf) {
        using L = StringList<"Alpha", "Beta", "Gamma">;

        static_assert(L::Contains<"Beta">);
        static_assert(!L::Contains<"Delta">);

        static_assert(L::IndexOf<"Alpha"> == 0);
        static_assert(L::IndexOf<"Gamma"> == 2);
        static_assert(L::IndexOf<"Delta"> == NPos);

        ASSERT_TRUE(true);
    }

    TEST(StringListTest, ForEachCollects) {
        using L = StringList<"Foo", "Bar">;

        std::vector<std::string> collected;
        StringCollector collector{ collected };
        L::ForEach(collector);

        ASSERT_EQ(collected, (std::vector<std::string>{"Foo", "Bar"}));
    }

    TEST(StringListTest, ForEachForwardsArgs) {
        using L = StringList<"Hi", "Yo">;

        size_t total = 0;
        StringLengthSummer summer{ total };
        L::ForEach(summer, 1);
        // ("Hi".size()+1) + ("Yo".size()+1) = (2+1)+(2+1)=6

        ASSERT_EQ(total, 6);
    }

    TEST(StringListTest, InvokeWithStringsExpandedCreatesTuple) {
        using L = StringList<"Dog", "Cat">;
        auto tup = L::InvokeWithStringsExpanded(TupleFactory{});

        ASSERT_EQ(std::get<0>(tup).View(), "Dog");
        ASSERT_EQ(std::get<1>(tup).View(), "Cat");
    }

    TEST(StringListTest, InvokeWithStringsExpandedForwardsArgs) {
        using L = StringList<"One", "Two", "Three">;
        int  count = 0;
        auto result = L::InvokeWithStringsExpanded(ArgForwarder{}, count);

        ASSERT_EQ(count, 3);
        ASSERT_EQ(result, "Processed");
    }

    TEST(StringListTest, PushAndPop) {
        using Base = StringList<"B", "C">;
        using Front = PushFrontStr_T<Base, "A">;

        static_assert(std::is_same_v<Front, StringList<"A", "B", "C">>);
        using Back = PushBackStr_T<Base, "D">;

        static_assert(std::is_same_v<Back, StringList<"B", "C", "D">>);
        using PopF = PopFrontStr_T<Base>;

        static_assert(std::is_same_v<PopF, StringList<"C">>);
        using PopB = PopBackStr_T<Base>;

        static_assert(std::is_same_v<PopB, StringList<"B">>);

        ASSERT_TRUE(true);
    }

    TEST(StringListTest, Concat) {
        using A = StringList<"A", "B">;
        using B = StringList<"C">;
        using AB = ConcatStr_T<A, B>;

        static_assert(std::is_same_v<AB, StringList<"A", "B", "C">>);

        ASSERT_TRUE(true);
    }

    TEST(StringListTest, TransformIdentity) {
        using L = StringList<"Hello", "World">;
        using T = TransformStr_T<L, Identity>;

        static_assert(std::is_same_v<T, L>);
    }

    TEST(StringListTest, Filter) {
        using L = StringList<"Alpha", "Beta", "Apple">;
        using Filtered = FilterStr_T<L, StartsWithA>;

        static_assert(std::is_same_v<Filtered, StringList<"Alpha", "Apple">>);
    }

    TEST(StringListTest, Reverse) {
        using L = StringList<"A", "B", "C">;
        using R = ReverseStr_T<L>;

        static_assert(std::is_same_v<R, StringList<"C", "B", "A">>);
    }

} // namespace Quirk::QkT
