

#include <QkTraits/StringLiteral.h>
#include <gtest/gtest.h>


namespace Quirk::QkT {

	class StringLiteralTest : public ::testing::Test {};

    TEST_F(StringLiteralTest, ConstructionAndSize) {
        // Test construction and size from standard string literal.
        {
            constexpr StringLiteral hello = "hello";
            const int expectedHelloSize = 5;

            static_assert(hello.Size() == 5, "Size should be 5 at compile-time.");
            static_assert(std::is_same_v<decltype(hello), const StringLiteral<6>>, "Type should be const StringLiteral<6>.");

            ASSERT_EQ(hello.Size(), expectedHelloSize);
        }

        // Test construction and size from empty string literal.
        {
            constexpr StringLiteral empty = "";

            static_assert(empty.Size() == 0, "Empty string size should be 0.");

            ASSERT_EQ(empty.Size(), 0);
        }
    }

    TEST_F(StringLiteralTest, DataAccess) {
        constexpr StringLiteral testStr = "abc";

        // Test the Data() method
        {
            ASSERT_STREQ(testStr.Data(), "abc");
        }

        // Test the character access operator[]
        {
            // COMPILE-TIME CHECKS
            static_assert(testStr[0] == 'a');
            static_assert(testStr[1] == 'b');
            static_assert(testStr[2] == 'c');
            static_assert(testStr[3] == '\0', "The character at index N-1 should be the null terminator.");

            // RUNTIME CHECKS
            ASSERT_EQ(testStr[0], 'a');
            ASSERT_EQ(testStr[1], 'b');
            ASSERT_EQ(testStr[2], 'c');
            ASSERT_EQ(testStr[2], 'c');
        }
    }

    TEST_F(StringLiteralTest, EqualityComparison) {
        constexpr StringLiteral str1 = "world";
        constexpr StringLiteral str2 = "world";
        constexpr StringLiteral str3 = "other";

        // Test for equality
        {
            static_assert(str1 == str2, "str1 and str2 should be equal at compile-time.");
            ASSERT_TRUE(str1 == str2);
        }

        // Test for inequality
        {
            static_assert(!(str1 == str3), "str1 and str3 should not be equal at compile-time.");
            ASSERT_FALSE(str1 == str3);
        }

        // Note: 
        //     An attempt to compare StringLiterals of different sizes,
        //     like `StringLiteral("a") == StringLiteral("ab")`, will correctly
        //     fail to compile because they are different types.
    }

    TEST_F(StringLiteralTest, ViewConversion) {
        constexpr static StringLiteral    hello   = "hello";
        constexpr        std::string_view strView = hello.View();

        // Test for complete view on the original string literal
        ASSERT_EQ(strView, "hello");
    }

} // namespace Quirk::QkT
