

#include <QkTraits/ValueList.h>
#include <gtest/gtest.h>

#include <array>
#include <tuple>
#include <string>
#include <type_traits>


namespace Quirk::QkT {

    // =============================================================================
    // Test Fixtures and Helper Structs
    // =============================================================================

    // A functor to test ForEach by collecting values
    struct ValueCollector {
        std::vector<int>& m_Values;

        template<auto V>
        void operator()() {
            m_Values.push_back(static_cast<int>(V));
        }
    };

    // A functor to test ForEach with argument forwarding
    struct ValueSummer {
        int m_Multiplier;
        int& m_Total;

        template<auto V>
        void operator()(int base) {
            m_Total += (static_cast<int>(V) * m_Multiplier) + base;
        }
    };

    // A functor to test InvokeWithValuesExpanded by creating a tuple
    struct TupleFactory {
        template<auto... Vs>
        constexpr auto operator()() const {
            return std::tuple<decltype(Vs)...>{Vs...};
        }
    };

    // A functor to test InvokeWithValuesExpanded with argument forwarding
    struct ArgForwarder {
        template<auto... Vs>
        auto operator()(int& count) const {
            count = sizeof...(Vs);
            return std::string("Processed");
        }
    };

    // Helper metafunction: square values
    template<auto V>
    struct Square {
        static constexpr auto Value = V * V;
    };

    // Helper predicate: keep even numbers
    template<auto V>
    struct IsEven {
        static constexpr bool Value = (V % 2 == 0);
    };


    // =============================================================================
    // Test Cases
    // =============================================================================

    // === Accessor Tests ==================================

    TEST(ValueListTest, Front) {
        {
            using List = ValueList<1, 2, 3>;
            static_assert(List::Front == 1);
        }
        {
            using List = ValueList<42>;
            static_assert(List::Front == 42);
        }
        {
            using List = ValueList<>;
            static_assert(List::Front == kInvalidVal);
        }
        ASSERT_TRUE(true);
    }

    TEST(ValueListTest, Back) {
        {
            using List = ValueList<1, 2, 3>;
            static_assert(List::Back == 3);
        }
        {
            using List = ValueList<99>;
            static_assert(List::Back == 99);
        }
        {
            using List = ValueList<>;
            static_assert(List::Back == kInvalidVal);
        }
        ASSERT_TRUE(true);
    }

    TEST(ValueListTest, Get) {
        using List = ValueList<10, 20, 30>;
        static_assert(List::Get<0> == 10);
        static_assert(List::Get<2> == 30);
        static_assert(List::Get<42> == kInvalidVal);
        ASSERT_TRUE(true);
    }


    // === Compile-Time Value Tests ==================================

    TEST(ValueListTest, SizeAndIsEmpty) {
        {
            using List = ValueList<1, 2, 3>;
            static_assert(List::Size == 3);
            static_assert(!List::IsEmpty);
        }
        {
            using List = ValueList<42>;
            static_assert(List::Size == 1);
            static_assert(!List::IsEmpty);
        }
        {
            using List = ValueList<>;
            static_assert(List::Size == 0);
            static_assert(List::IsEmpty);
        }
        ASSERT_TRUE(true);
    }

    TEST(ValueListTest, HasDuplicates) {
        {
            using List = ValueList<1, 2, 3, 1>;
            static_assert(List::HasDuplicates);
        }
        {
            using List = ValueList<1, 2, 3>;
            static_assert(!List::HasDuplicates);
        }
        {
            using List = ValueList<1, 1, 2, 2>;
            static_assert(List::HasDuplicates);
        }
        {
            using List = ValueList<>;
            static_assert(!List::HasDuplicates);
        }
        ASSERT_TRUE(true);
    }

    TEST(ValueListTest, IsHomogeneous) {
        {
            using List = ValueList<1, 2, 3>;
            static_assert(List::IsHomogeneous);
        }
        {
            using List = ValueList<'a', 'b'>;
            static_assert(List::IsHomogeneous);
        }
        {
            using List = ValueList<1, 'c'>;
            static_assert(!List::IsHomogeneous);
        }
        {
            using List = ValueList<>;
            static_assert(List::IsHomogeneous);
        }
        ASSERT_TRUE(true);
    }

    TEST(ValueListTest, ContainsAndIndexOf) {
        using List = ValueList<5, 10, 15>;
        static_assert(List::Contains<10>);
        static_assert(!List::Contains<99>);
        static_assert(List::IndexOf<5> == 0);
        static_assert(List::IndexOf<15> == 2);
        static_assert(List::IndexOf<99> == NPos);

        using EmptyList = ValueList<>;
        static_assert(!EmptyList::Contains<1>);
        static_assert(EmptyList::IndexOf<1> == NPos);
        ASSERT_TRUE(true);
    }


    // === ForEach Tests ==================================

    TEST(ValueListTest, ForEachOnEmptyList) {
        using Empty = ValueList<>;
        bool called = false;
        auto lambda = [&](auto&&...) { called = true; };
        Empty::ForEach(lambda);
        ASSERT_FALSE(called);
    }

    TEST(ValueListTest, ForEachCollectsValues) {
        using List = ValueList<1, 2, 3>;
        std::vector<int> collected;
        ValueCollector collector{ collected };
        List::ForEach(collector);
        ASSERT_EQ(collected, (std::vector<int>{1, 2, 3}));
    }

    TEST(ValueListTest, ForEachForwardsArguments) {
        using List = ValueList<2, 4>;
        int total = 0;
        ValueSummer summer{ 2, total };
        List::ForEach(summer, 10);
        // (2*2+10) + (4*2+10) = 14 + 18 = 32
        ASSERT_EQ(total, 32);
    }


    // === InvokeWithValuesExpanded Tests ==================

    TEST(ValueListTest, InvokeWithValuesExpandedCreatesTuple) {
        using List = ValueList<1, 2, 3>;
        auto result = List::InvokeWithValuesExpanded(TupleFactory{});
        static_assert(std::is_same_v<decltype(result), std::tuple<int, int, int>>);
        ASSERT_EQ(std::get<0>(result), 1);
        ASSERT_EQ(std::get<2>(result), 3);
    }

    TEST(ValueListTest, InvokeWithValuesExpandedForwardsArguments) {
        using List = ValueList<7, 8, 9>;
        int count = 0;
        ArgForwarder fwd;
        auto result = List::InvokeWithValuesExpanded(fwd, count);
        ASSERT_EQ(count, 3);
        ASSERT_EQ(result, "Processed");
    }

    TEST(ValueListTest, ConstexprInvokeWithValuesExpanded) {
        using List = ValueList<10, 20, 30>;
        constexpr auto tup = List::InvokeWithValuesExpanded(TupleFactory{});
        static_assert(std::tuple_size_v<decltype(tup)> == 3);
        static_assert(std::get<1>(tup) == 20);
        ASSERT_TRUE(true);
    }


    // === Manipulation Tests ==================================

    TEST(ValueListTest, PushFrontAndBack) {
        {
            using List = ValueList<2, 3>;
            using R1 = PushFrontVal_T<List, 1>;
            static_assert(std::is_same_v<R1, ValueList<1, 2, 3>>);
        }
        {
            using List = ValueList<1, 2>;
            using R2 = PushBackVal_T<List, 3>;
            static_assert(std::is_same_v<R2, ValueList<1, 2, 3>>);
        }
        {
            using Empty = ValueList<>;
            using R3 = PushFrontVal_T<Empty, 7>;
            using R4 = PushBackVal_T<Empty, 9>;
            static_assert(std::is_same_v<R3, ValueList<7>>);
            static_assert(std::is_same_v<R4, ValueList<9>>);
        }
        ASSERT_TRUE(true);
    }

    TEST(ValueListTest, PopFrontAndBack) {
        {
            using List = ValueList<1, 2, 3>;
            using R1 = PopFrontVal_T<List>;
            static_assert(std::is_same_v<R1, ValueList<2, 3>>);
        }
        {
            using List = ValueList<1, 2, 3>;
            using R2 = PopBackVal_T<List>;
            static_assert(std::is_same_v<R2, ValueList<1, 2>>);
        }
        {
            using List = ValueList<1>;
            using R3 = PopFrontVal_T<List>;
            using R4 = PopBackVal_T<List>;
            static_assert(std::is_same_v<R3, ValueList<>>);
            static_assert(std::is_same_v<R4, ValueList<>>);
        }
        {
            using Empty = ValueList<>;
            using R5 = PopBackVal_T<Empty>;
            static_assert(std::is_same_v<R5, ValueList<>>);
        }
        ASSERT_TRUE(true);
    }


    // === Algorithm Tests ==================================

    TEST(ValueListTest, Concat) {
        using A = ValueList<1, 2>;
        using B = ValueList<3, 4>;
        using R1 = ConcatVal_T<A, B>;
        static_assert(std::is_same_v<R1, ValueList<1, 2, 3, 4>>);

        using R2 = ConcatVal_T<A, ValueList<>>;
        static_assert(std::is_same_v<R2, A>);

        using R3 = ConcatVal_T<ValueList<>, B>;
        static_assert(std::is_same_v<R3, B>);

        using R4 = ConcatVal_T<ValueList<>, ValueList<>>;
        static_assert(std::is_same_v<R4, ValueList<>>);

        ASSERT_TRUE(true);
    }

    TEST(ValueListTest, Transform) {
        using List = ValueList<2, 3, 4>;
        using Squares = TransformVal_T<List, Square>;
        static_assert(std::is_same_v<Squares, ValueList<4, 9, 16>>);

        using Empty = ValueList<>;
        using EmptyT = TransformVal_T<Empty, Square>;
        static_assert(std::is_same_v<EmptyT, ValueList<>>);

        ASSERT_TRUE(true);
    }

    TEST(ValueListTest, Filter) {
        using List = ValueList<1, 2, 3, 4>;
        using Evens = FilterVal_T<List, IsEven>;
        static_assert(std::is_same_v<Evens, ValueList<2, 4>>);

        using Empty = ValueList<>;
        using EvensEmpty = FilterVal_T<Empty, IsEven>;
        static_assert(std::is_same_v<EvensEmpty, ValueList<>>);

        ASSERT_TRUE(true);
    }

    TEST(ValueListTest, Reverse) {
        {
            using List = ValueList<1, 2, 3>;
            using R = ReverseVal_T<List>;
            static_assert(std::is_same_v<R, ValueList<3, 2, 1>>);
        }
        {
            using List = ValueList<42>;
            using R = ReverseVal_T<List>;
            static_assert(std::is_same_v<R, ValueList<42>>);
        }
        {
            using Empty = ValueList<>;
            using R = ReverseVal_T<Empty>;
            static_assert(std::is_same_v<R, ValueList<>>);
        }
        ASSERT_TRUE(true);
    }

} // namespace Quirk::QkT
