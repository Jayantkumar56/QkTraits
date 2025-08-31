

#include <QkTraits/StringLiteral.h>
#include <gtest/gtest.h>


namespace Quirk::QkT {

    TEST(StringLiteralTest, ConstructionAndSize) {
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

    TEST(StringLiteralTest, DataAccess) {
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

    TEST(StringLiteralTest, EqualityAndInequality) {
        constexpr StringLiteral str1 = "apple";  // N=6
        constexpr StringLiteral str2 = "apple";  // N=6
        constexpr StringLiteral str3 = "apric";  // N=6 (same size as "apple")

        // Compile-time checks
        static_assert(str1 == str2);
        static_assert(!(str1 != str2));
        static_assert(str1 != str3);

        // Runtime checks
        ASSERT_TRUE(str1 == str2);
        ASSERT_FALSE(str1 != str2);
        ASSERT_TRUE(str1 != str3);
    }

    TEST(StringLiteralTest, RelationalOperators) {
        constexpr StringLiteral a = "aaa";  // N=4
        constexpr StringLiteral b = "aab";  // N=4
        constexpr StringLiteral c = "aac";  // N=4

        // Compile-time
        static_assert(a < b);
        static_assert(b < c);
        static_assert(a <= b);
        static_assert(c > b);
        static_assert(c >= b);

        // Runtime
        ASSERT_TRUE(a < b);
        ASSERT_TRUE(b < c);
        ASSERT_TRUE(a <= b);
        ASSERT_TRUE(c > b);
        ASSERT_TRUE(c >= b);
    }

    TEST(StringLiteralTest, ViewConversion) {
        constexpr static StringLiteral    hello   = "hello";
        constexpr        std::string_view strView = hello.View();

        // Test for complete view on the original string literal
        ASSERT_EQ(strView, "hello");
    }

    TEST(StringLiteralTest, ConcatenationBasic) {
        constexpr StringLiteral hello = "Hello";
        constexpr StringLiteral world = "World";

        constexpr auto combined = hello + world;

        // Compile-time
        static_assert(combined.Size() == hello.Size() + world.Size());
        static_assert(combined.View() == "HelloWorld");

        // Runtime
        ASSERT_EQ(combined.Size(), 10);
        ASSERT_EQ(combined.View(), "HelloWorld");
        ASSERT_EQ(combined.value[combined.Size()], '\0'); // null terminator
    }

    TEST(StringLiteralTest, ConcatenationWithEmpty) {
        constexpr StringLiteral empty = "";
        constexpr StringLiteral text = "Text";

        constexpr auto r1 = empty + text;
        constexpr auto r2 = text + empty;

        static_assert(r1 == text);
        static_assert(r2 == text);

        ASSERT_EQ(r1.View(), "Text");
        ASSERT_EQ(r2.View(), "Text");
    }

    TEST(StringLiteralTest, MultipleConcatenations) {
        constexpr StringLiteral a = "A";
        constexpr StringLiteral b = "B";
        constexpr StringLiteral c = "C";

        constexpr auto abc = a + b + c;

        static_assert(abc.View() == "ABC");

        ASSERT_EQ(abc.View(), "ABC");
        ASSERT_EQ(abc.Size(), 3);
    }

} // namespace Quirk::QkT
