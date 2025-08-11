

#include <QkTraits/TypeList.h>
#include <QkTraits/PointerTraits.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <tuple>


namespace Quirk::QkT {

    // =============================================================================
    // Test Fixtures and Helper Structs
    // =============================================================================

    // A simple functor to test ForEach by collecting type names
    struct TypeNameCollector {
        std::vector<std::string>& m_Names;

        template<typename T>
        void operator()() {
            m_Names.push_back(typeid(T).name());
        }
    };

    // A functor to test ForEach with argument forwarding
    struct TypeSizeSummer {
        int m_Multiplier;
        size_t& m_TotalSize;

        template<typename T>
        void operator()(int initialValue) {
            m_TotalSize += (sizeof(T) * m_Multiplier) + initialValue;
        }
    };

    // A functor to test InvokeWithTypesExpanded by creating a tuple
    struct TupleFactory {
        template<typename... Ts>
        constexpr auto operator()() const {
            return std::tuple<Ts...>();
        }
    };

    // A functor to test InvokeWithTypesExpanded with argument forwarding
    struct ArgForwarder {
        template<typename... Ts>
        auto operator()(int& value) const {
            value = sizeof...(Ts);
            return std::string("Processed");
        }
    };


    // =============================================================================
    // Test Cases
    // =============================================================================

    // === Accessor Tests ==================================

    TEST(TypeListTest, Front) {
        // Standard non-empty list
        {
            using List1 = TypeList<int, double, char>;
            static_assert(std::is_same_v<List1::Front, int>, "Front should get the first type.");
        }

        // Single element list
        {
            using List2 = TypeList<bool>;
            static_assert(std::is_same_v<List2::Front, bool>, "Front should work on a single-element list.");
        }

        // Empty list
        {
            using EmptyList = TypeList<>;
            static_assert(std::is_same_v<EmptyList::Front, InvalidType>, "Front of an empty list should be InvalidType.");
        }

        ASSERT_TRUE(true);
    }

    TEST(TypeListTest, Back) {
        // Standard non-empty list
        {
            using List1 = TypeList<int, double, char>;
            static_assert(std::is_same_v<List1::Back, char>, "Back should get the last type.");
        }

        // SSingle element list
        {
            using List2 = TypeList<bool>;
            static_assert(std::is_same_v<List2::Back, bool>, "Back should work on a single-element list.");
        }

        // Empty list
        {
            using EmptyList = TypeList<>;
            static_assert(std::is_same_v<EmptyList::Back, InvalidType>, "Back of an empty list should be InvalidType.");
        }

        ASSERT_TRUE(true);
    }

    TEST(TypeListTest, Get) {
        using MyTypes = TypeList<int, char, float, std::string>;

        // Check types from the beginning, middle, and end
        static_assert(std::is_same_v<MyTypes::Get<0>, int>);
        static_assert(std::is_same_v<MyTypes::Get<1>, char>);
        static_assert(std::is_same_v<MyTypes::Get<3>, std::string>);

        // The following line should fail to compile, which is the correct behavior.
        // using OutOfBounds = MyTypes::Get<4>;

        ASSERT_TRUE(true);
    }

    TEST(TypeListTest, FirstNonVoid) {
        // Finds the first valid type after skipping voids
        {
            using List1 = TypeList<void, void, int, char>;
            static_assert(std::is_same_v<List1::FirstNonVoid, int>);
        }

        // The first type is already valid
        {
            using List2 = TypeList<char, void, int>;
            static_assert(std::is_same_v<List2::FirstNonVoid, char>);
        }

        // List contains only void types
        {
            using AllVoid = TypeList<void, void, void>;
            static_assert(std::is_same_v<AllVoid::FirstNonVoid, InvalidType>);
        }

        // Empty list
        {
            using EmptyList = TypeList<>;
            static_assert(std::is_same_v<EmptyList::FirstNonVoid, InvalidType>);
        }

        ASSERT_TRUE(true);
    }


    // === Compile-Time Value Tests ==================================

    TEST(TypeListTest, SizeAndIsEmpty) {
        // Non-empty list
        {
            using List1 = TypeList<int, double, char>;
            static_assert(List1::Size == 3, "Size should be 3.");
            static_assert(!List1::IsEmpty, "IsEmpty should be false.");
        }

        // Single element list
        {
            using List2 = TypeList<bool>;
            static_assert(List2::Size == 1, "Size should be 1.");
            static_assert(!List2::IsEmpty, "IsEmpty should be false.");
        }

        // Empty list
        {
            using EmptyList = TypeList<>;
            static_assert(EmptyList::Size == 0, "Size of an empty list should be 0.");
            static_assert(EmptyList::IsEmpty, "IsEmpty should be true for an empty list.");
        }

        ASSERT_TRUE(true);
    }

    TEST(TypeListTest, HasDuplicates) {
        // List with duplicates
        {
            using List1 = TypeList<int, char, double, int>;
            static_assert(List1::HasDuplicates, "Should detect duplicates.");
        }

        // List without duplicates
        {
            using List2 = TypeList<int, char, double, float>;
            static_assert(!List2::HasDuplicates, "Should not find duplicates in a unique list.");
        }

        // List with multiple different duplicates
        {
            using List3 = TypeList<int, char, int, char>;
            static_assert(List3::HasDuplicates, "Should detect multiple sets of duplicates.");
        }

        // Empty and single-element lists
        {
            static_assert(!TypeList<>::HasDuplicates, "Empty list has no duplicates.");
            static_assert(!TypeList<int>::HasDuplicates, "Single-element list has no duplicates.");
        }

        ASSERT_TRUE(true);
    }

    TEST(TypeListTest, Contains) {
        using MyTypes = TypeList<int, char, float>;

        // Types that are in the list
        {
            static_assert(MyTypes::Contains<int>, "Should find existing type 'int'.");
            static_assert(MyTypes::Contains<float>, "Should find existing type 'float'.");
        }

        // Type that is not in the list
        static_assert(!MyTypes::Contains<double>, "Should not find non-existent type 'double'.");

        // Check on an empty list
        {
            using EmptyList = TypeList<>;
            static_assert(!EmptyList::Contains<int>, "Empty list should not contain any type.");
        }

        ASSERT_TRUE(true);
    }

    TEST(TypeListTest, IndexOf) {
        using MyTypes = TypeList<int, char, float, char>;

        // Scenario: Find types in the list
        {
            static_assert(MyTypes::IndexOf<int> == 0);
            static_assert(MyTypes::IndexOf<float> == 2);
        }

        // Scenario: Find the first occurrence of a duplicate type
        static_assert(MyTypes::IndexOf<char> == 1, "IndexOf should find the first occurrence.");

        // Scenario: Type is not found
        constexpr size_t notFound = static_cast<size_t>(-1);
        static_assert(MyTypes::IndexOf<double> == notFound, "IndexOf should return -1 for non-existent types.");

        // Scenario: Check on an empty list
        {
            using EmptyList = TypeList<>;
            static_assert(EmptyList::IndexOf<int> == notFound, "IndexOf on an empty list should return -1.");
        }

        ASSERT_TRUE(true);
    }


    // === ForEach Tests ==================================

    TEST(TypeListTest, ForEachOnEmptyList) {
        using EmptyList = TypeList<>;
        bool wasCalled = false;

        auto functor = [&](auto&&...) { wasCalled = true; };
        EmptyList::ForEach(functor);

        ASSERT_FALSE(wasCalled);
    }

    TEST(TypeListTest, ForEachCollectsTypeNames) {
        using MyTypes = TypeList<int, double, char>;
        std::vector<std::string> collectedNames;

        TypeNameCollector collector{ collectedNames };
        MyTypes::ForEach(collector);

        ASSERT_EQ(collectedNames.size(), 3                     );
        ASSERT_EQ(collectedNames[0],     typeid(int).name()    );
        ASSERT_EQ(collectedNames[1],     typeid(double).name() );
        ASSERT_EQ(collectedNames[2],     typeid(char).name()   );
    }

    TEST(TypeListTest, ForEachForwardsArguments) {
        using  MyTypes   = TypeList<int, char>; // sizeof(int)=4, sizeof(char)=1
        size_t totalSize = 0;

        constexpr int multiplier   = 2;
        constexpr int initialValue = 10;

        TypeSizeSummer summer{ multiplier, totalSize };
        MyTypes::ForEach(summer, initialValue);

        // Expect: (sizeof(int)*2 + 10) + (sizeof(char)*2 + 10)
        //       = (4*2 + 10) + (1*2 + 10) = 18 + 12 = 30
        size_t expectedSize = (sizeof(int) * multiplier + initialValue) + (sizeof(char) * multiplier + initialValue);
        ASSERT_EQ(totalSize, expectedSize);
    }


    // === InvokeWithTypesExpanded Tests ==================

    TEST(TypeListTest, InvokeWithTypesExpandedCreatesTuple) {
        using MyTypes = TypeList<int, std::string, bool>;

        auto resultTuple = MyTypes::InvokeWithTypesExpanded(TupleFactory{});

        using ExpectedTupleType = std::tuple<int, std::string, bool>;
        static_assert(std::is_same_v<decltype(resultTuple), ExpectedTupleType>);

        // Runtime check to be safe
        ASSERT_EQ(std::tuple_size_v<decltype(resultTuple)>, 3);
    }

    TEST(TypeListTest, InvokeWithTypesExpandedForwardsArguments) {
        using MyTypes   = TypeList<int, double, char>;
        int   typeCount = 0;

        ArgForwarder forwarder;
        auto result = MyTypes::InvokeWithTypesExpanded(forwarder, typeCount);

        ASSERT_EQ(typeCount, 3);
        ASSERT_EQ(result, "Processed");
    }

    TEST(TypeListTest, ConstexprInvokeWithTypesExpanded) {
        using MyTypes = TypeList<int, long, short>;

        // This test primarily checks if the code compiles in a constexpr context.
        // The result of the factory is evaluated at compile time.

        constexpr auto resultTuple = MyTypes::InvokeWithTypesExpanded(TupleFactory{});

        static_assert(std::tuple_size_v<decltype(resultTuple)> == 3);

        // Since resultTuple is constexpr thus we would get const T from std::tuple_element_t
        // Thus to compare underlying type, cv qualifiers must be removed.
        static_assert(std::is_same_v<std::remove_cv_t<std::tuple_element_t<1, decltype(resultTuple)>>, long>);

        // Dummy runtime assert to ensure the test runs
        ASSERT_TRUE(true);
    }


    // === Manipulation Tests ==================================

    TEST(TypeListTest, PushFront) {
        // Pushing to a non-empty list
        {
            using List1 = TypeList<char, double>;
            using Result1 = PushFront_T<List1, int>;
            static_assert(std::is_same_v<Result1, TypeList<int, char, double>>);
        }

        // Pushing to an empty list
        {
            using EmptyList = TypeList<>;
            using Result2 = PushFront_T<EmptyList, int>;
            static_assert(std::is_same_v<Result2, TypeList<int>>);
        }

        // This should fail to compile because the first argument is not a TypeList
        // using Invalid = PushFront_T<int, float>;

        ASSERT_TRUE(true);
    }

    TEST(TypeListTest, PushBack) {
        // Pushing to a non-empty list
        {
            using List1 = TypeList<char, double>;
            using Result1 = PushBack_T<List1, int>;
            static_assert(std::is_same_v<Result1, TypeList<char, double, int>>);
        }

        // Pushing to an empty list
        {
            using EmptyList = TypeList<>;
            using Result2 = PushBack_T<EmptyList, int>;
            static_assert(std::is_same_v<Result2, TypeList<int>>);
        }

        // This should fail to compile because the first argument is not a TypeList
        // using Invalid = PushBack_T<int, float>;

        ASSERT_TRUE(true);
    }

    TEST(TypeListTest, PopFront) {
        // Popping from a multi-element list
        {
            using List1 = TypeList<int, char, double>;
            using Result1 = PopFront_T<List1>;
            static_assert(std::is_same_v<Result1, TypeList<char, double>>);
        }

        // Popping from a single-element list
        {
            using List2 = TypeList<int>;
            using Result2 = PopFront_T<List2>;
            static_assert(std::is_same_v<Result2, TypeList<>>);
        }

        // This should fail to compile because you cannot pop from an empty list
        // using Invalid = PopFront_T<TypeList<>>;

        ASSERT_TRUE(true);
    }

    TEST(TypeListTest, PopBack) {
        // Popping from a multi-element list
        {
            using List1 = TypeList<int, char, double>;
            using Result1 = PopBack_T<List1>;
            static_assert(std::is_same_v<Result1, TypeList<int, char>>);
        }

        // Popping from a single-element list
        {
            using List2 = TypeList<int>;
            using Result2 = PopBack_T<List2>;
            static_assert(std::is_same_v<Result2, TypeList<>>);
        }

        // Popping from an empty list
        {
            using EmptyList = TypeList<>;
            using Result3 = PopBack_T<EmptyList>;
            static_assert(std::is_same_v<Result3, TypeList<>>, "PopBack on an empty list should result in an empty list.");
        }

        ASSERT_TRUE(true);
    }

    // === Algorithm Tests ==================================

    TEST(TypeListTest, Concat) {
        using ListA = TypeList<int, char>;
        using ListB = TypeList<float, double>;

        // Concatenating two non-empty lists
        {
            using Result1 = Concat_T<ListA, ListB>;
            static_assert(std::is_same_v<Result1, TypeList<int, char, float, double>>);
        }

        // Concatenating with an empty list on the right
        {
            using Result2 = Concat_T<ListA, TypeList<>>;
            static_assert(std::is_same_v<Result2, ListA>);
        }

        // Concatenating with an empty list on the left
        {
            using Result3 = Concat_T<TypeList<>, ListB>;
            static_assert(std::is_same_v<Result3, ListB>);
        }

        // Concatenating two empty lists
        {
            using Result4 = Concat_T<TypeList<>, TypeList<>>;
            static_assert(std::is_same_v<Result4, TypeList<>>);
        }

        // This should fail to compile because the second argument is not a TypeList
        // using Invalid = Concat_T<ListA, float>;

        ASSERT_TRUE(true);
    }

    // A helper meta-function for the Transform test
    template<typename T>
    struct AddPointer {
        using Type = T*;
    };

    // Helper that adapts std::add_const to our ::Type convention
    template<typename T>
    struct AddConstAdapter {
        using Type = std::add_const_t<T>; // Uses the std _t alias for cleanliness
    };

    TEST(TypeListTest, Transform) {
        using MyTypes = TypeList<int, char, double>;

        // Transforming a non-empty list
        {
            using PtrTypes = Transform_T<MyTypes, AddPointer>;
            static_assert(std::is_same_v<PtrTypes, TypeList<int*, char*, double*>>);
        }

        // Transforming using a standard library trait via the adapter
        {
            using ConstTypes = Transform_T<MyTypes, AddConstAdapter>;
            static_assert(std::is_same_v<ConstTypes, TypeList<const int, const char, const double>>);
        }

        // Transforming an empty list
        {
            using EmptyTransformed = Transform_T<TypeList<>, AddPointer>;
            static_assert(std::is_same_v<EmptyTransformed, TypeList<>>);
        }

        ASSERT_TRUE(true);
    }


    TEST(TypeListTest, Filter) {
        using MixedTypes = TypeList<int, char*, double, long*>;

        // Filtering a mixed list
        {
            using PointerOnly = Filter_T<MixedTypes, IsPointer>;
            static_assert(std::is_same_v<PointerOnly, TypeList<char*, long*>>);
        }

        // Filtering an empty list
        {
            using EmptyFiltered = Filter_T<TypeList<>, IsPointer>;
            static_assert(std::is_same_v<EmptyFiltered, TypeList<>>);
        }

        ASSERT_TRUE(true);
    }

} // namespace Quirk::QkT
