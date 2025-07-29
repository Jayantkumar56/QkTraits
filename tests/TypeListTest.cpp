

#include <QkTraits/TypeList.h>
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

} // namespace Quirk::QkT
