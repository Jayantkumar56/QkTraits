

#include <QkTraits/PointerTraits.h>
#include <gtest/gtest.h>

#include <string>


namespace Quirk::QkT {

    // =============================================================================
    // Helper Structs
    // =============================================================================

    struct TestBase { virtual ~TestBase() = default; int baseValue = 1; };
    struct TestDerived : TestBase { int derivedValue = 2; };


    // =============================================================================
    // Test Cases
    // =============================================================================

    // === Is...Pointer / Concepts Tests ==================

    TEST(PointerTraitsTest, IsSmartPointer) {
        static_assert( SmartPointer<std::unique_ptr<int>>           );
        static_assert( SmartPointer<std::shared_ptr<char>>          );
        static_assert( SmartPointer<const std::unique_ptr<double>&> );

        static_assert( !SmartPointer<int>  );
        static_assert( !SmartPointer<int*> );

        SUCCEED();
    }

    TEST(PointerTraitsTest, IsRawPointer) {
        static_assert( RawPointer<int*>                );
        static_assert( RawPointer<const char*>         );
        static_assert( RawPointer<TestBase* const>     );
        static_assert( RawPointer<const TestDerived*&> );

        static_assert( !RawPointer<int>                  );
        static_assert( !RawPointer<std::unique_ptr<int>> );

        SUCCEED();
    }

    TEST(PointerTraitsTest, IsPointer) {
        static_assert( Pointer<int*>                         );
        static_assert( Pointer<std::unique_ptr<int>>         );
        static_assert( Pointer<const std::shared_ptr<char>&> );

        static_assert( !Pointer<int>      );
        static_assert( !Pointer<TestBase> );

        SUCCEED();
    }


    // === PointingType Tests =============================

    TEST(PointerTraitsTest, PointingType) {
        static_assert( std::is_same_v<PointingType_T<int*>, int>                                  );
        static_assert( std::is_same_v<PointingType_T<const char*>, const char>                    );
        static_assert( std::is_same_v<PointingType_T<std::unique_ptr<std::string>>, std::string>  );
        static_assert( std::is_same_v<PointingType_T<const std::shared_ptr<TestBase>&>, TestBase> );

        SUCCEED();
    }

} // namespace Quirk::QkT
