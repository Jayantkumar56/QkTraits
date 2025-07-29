

#include <QkTraits/TypeTraits.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <tuple>
#include <filesystem>


namespace Quirk::QkT {

    // =============================================================================
    // Test Cases
    // =============================================================================

    // === BoolConstant Tests =============================

    TEST(TypeTraitsTest, BoolConstantValues) {
        static_assert( TrueType::Value  == true  );
        static_assert( FalseType::Value == false );

        SUCCEED();
    }


    // === IsNonVoid Tests ================================

    TEST(TypeTraitsTest, IsNonVoid) {
        static_assert( IsNonVoid_V<int>         );
        static_assert( IsNonVoid_V<void*>       );
        static_assert( !IsNonVoid_V<void>       );
        static_assert( !IsNonVoid_V<const void> );

        SUCCEED();
    }


    // === AlwaysFalse Tests ==============================

    TEST(TypeTraitsTest, AlwaysFalse) {
        // Test that AlwaysFalse<T>::Value is always false,
        // which is useful inside other templates with static_assert.
        static_assert( AlwaysFalse_V<int>         == false );
        static_assert( AlwaysFalse_V<std::string> == false );

        SUCCEED();
    }


    // === RemoveAllWrapperTypes_T Tests ==================

    TEST(TypeTraitsTest, RemoveAllWrapperTypes) {
        // Basic types
        {
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<int>, int>        );
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<const int&>, int> );
        }

        // Pointer types
        {
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<int*>, int>             );
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<const int*>, int>       );
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<int* const>, int>       );
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<const int* const>, int> );
        }

        // Reference to pointer types
        {
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<int*&>, int>             );
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<const int*&>, int>       );
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<int* const&>, int>       );
            static_assert( std::is_same_v<RemoveAllWrapperTypes_T<const int* const&>, int> );
        }

        SUCCEED();
    }


    // === IsView / ViewType Tests ========================

    TEST(TypeTraitsTest, IsViewAndConcept) {
        // Positive cases
        {
            static_assert( IsView_V<View<int>>          );
            static_assert( ViewType<View<char>>         );
            static_assert( ViewType<const View<double>> );
        }

        // Negative cases
        {
            static_assert( !IsView_V<int>              );
            static_assert( !ViewType<int>              );
            static_assert( !ViewType<View<int>*>       );
            static_assert( !ViewType<std::vector<int>> );
        }

        SUCCEED();
    }


    // === Concept Tests ==================================

    namespace ConceptTestHelpers {
        // PathType
        static void AcceptsPath(const PathType auto&) {}
        struct NotAPath {};

        // TupleType
        static void AcceptsTuple(const TupleType auto&) {}
        struct NotATuple {};

        // ConditionTrait
        template<typename T>
        struct AlwaysTrueCondition : TrueType {};

        // A constrained trait that is only valid for class types.
        // This uses SFINAE to fail for non-class types.
        template<typename T, typename = std::enable_if_t<std::is_class_v<T>>>
        struct ClassOnlyCondition : TrueType {};

        // A struct that is NOT a trait because it lacks the ::Value member.
        template<typename T>
        struct NotACondition { using Type = T; };

        // Helper types for testing.
        struct MyTestClass {};
        using  MyTestInt = int;
    }

    TEST(TypeTraitsTest, Concepts) {
        using namespace ConceptTestHelpers;

        // PathType concept
        {
            AcceptsPath( std::filesystem::path("/")   );
            AcceptsPath( std::string("/path/to/file") );
            AcceptsPath( "/path/literal"              );

            // The following would fail to compile, which is correct:
            // ConceptTestHelpers::AcceptsPath(ConceptTestHelpers::NotAPath{});
        }

        // TupleType concept
        {
            AcceptsTuple( std::tuple<int, char>() );
            AcceptsTuple( std::pair<int, int>()   );

            // The following would fail to compile, which is correct:
            // ConceptTestHelpers::AcceptsTuple(ConceptTestHelpers::NotATuple{});
        }

        // ConditionTrait concept
        {
            static_assert( ConditionTrait<AlwaysTrueCondition, MyTestClass> );
            static_assert( ConditionTrait<AlwaysTrueCondition, MyTestInt>   );

            // A constrained trait should satisfy the concept for a valid type.
            static_assert(ConditionTrait<ClassOnlyCondition, MyTestClass>);


            // --- Negative Cases ---

            // A constrained trait should NOT satisfy the concept for an invalid type.
            //    This is the key test: the concept correctly evaluates to false because
            //    the `requires` clause fails when `ClassOnlyTrait<MyTestInt>` is not a
            //    valid instantiation.
            static_assert(!ConditionTrait<ClassOnlyCondition, MyTestInt>);

            // A type that is not a trait (lacks ::Value) should not satisfy the concept.
            static_assert( !ConditionTrait<NotACondition, MyTestClass> );
            static_assert( !ConditionTrait<NotACondition, MyTestInt>   );
        }

        SUCCEED();
    }

} // namespace Quirk::QkT
