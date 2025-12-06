

#include <QkTraits/FunctionTraits.h>
#include <gtest/gtest.h>

#include <tuple>
#include <type_traits>


namespace QkT {

    // =============================================================================
    // Test Fixtures and Helper Structs
    // =============================================================================

    // A struct with every possible permutation of member function qualifiers
    struct ComplexObject {
        // Basic
        void Normal(int, float) {}

        // CV Qualifiers
        void Const(int, float) const {}
        void Volatile(int, float) volatile {}
        void ConstVolatile(int, float) const volatile {}

        // Ref Qualifiers
        void LValue(int, float)& {}
        void RValue(int, float)&& {}
        void ConstLValue(int, float) const& {}

        // NoExcept variants
        void NormalNoExcept(int, float) noexcept {}
        void ConstNoExcept(int, float) const noexcept {}
    };

    // Functor object
    struct SimpleFunctor {
        int operator()(const std::string& s) { return 42; }
    };

    // Helper to make the assertions readable
    template <typename T1, typename T2>
    constexpr bool IsType = std::is_same_v<T1, T2>;


    // =============================================================================
    // Test Cases
    // =============================================================================

    // === Free Functions & Pointers ==========================

    void FreeFunction(int a, double b) { }
    int  FreeFunctionRet(char c)       { return 0; }

    TEST(FunctionTraitsTest, FreeFunctions) {
        // Test: void(int, double)
        using Traits = Function<decltype(FreeFunction)>;

        static_assert(IsType<Traits::ReturnT, void>);
        static_assert(IsType<Traits::ArgsT, TypeList<int, double>>);
    }

    TEST(FunctionTraitsTest, FreeFunctionPointers) {
        // Test: int(*)(char)
        using PtrType = decltype(&FreeFunctionRet);
        using Traits = Function<PtrType>;

        static_assert(IsType<Traits::ReturnT, int>);
        static_assert(IsType<Traits::ArgsT, TypeList<char>>);
    }

    TEST(FunctionTraitsTest, FunctionReferences) {
        // Test: int(&)(char)
        using RefType = int(&)(char);
        using Traits = Function<RefType>;

        static_assert(IsType<Traits::ReturnT, int>);
        static_assert(IsType<Traits::ArgsT, TypeList<char>>);
    }

    // === NoExcept Handling ==================================

    void NoExceptFunc(int) noexcept {}

    TEST(FunctionTraitsTest, NoExceptIsStripped) {
        // Traits are designed to treat (T...) and (T... noexcept) 
        // as having the same ReturnT and ArgsT.

        using NormalTraits   = Function<void(int)>;
        using NoExceptTraits = Function<decltype(NoExceptFunc)>;

        static_assert(IsType<NormalTraits::ArgsT, NoExceptTraits::ArgsT>);
        static_assert(IsType<NoExceptTraits::ReturnT, void>);
    }

    // === Member Functions (The Macro Tests) =================

    TEST(FunctionTraitsTest, MemberFunctionsBasic) {
        // Test: void ComplexObject::Normal(int, float)
        using Traits = Function<decltype(&ComplexObject::Normal)>;

        static_assert(IsType<Traits::ReturnT, void>);
        static_assert(IsType<Traits::ArgsT, TypeList<int, float>>);
    }

    TEST(FunctionTraitsTest, MemberFunctionsConstVolatile) {
        // The traits should strip 'const', 'volatile' from the function signature
        // and extract only the argument types.

        // Const
        using ConstTraits = Function<decltype(&ComplexObject::Const)>;
        static_assert(IsType<ConstTraits::ArgsT, TypeList<int, float>>);

        // Volatile
        using VolTraits = Function<decltype(&ComplexObject::Volatile)>;
        static_assert(IsType<VolTraits::ArgsT, TypeList<int, float>>);

        // Const Volatile
        using CVTraits = Function<decltype(&ComplexObject::ConstVolatile)>;
        static_assert(IsType<CVTraits::ArgsT, TypeList<int, float>>);
    }

    TEST(FunctionTraitsTest, MemberFunctionsRefQualifiers) {
        // L-Value Ref (&)
        using LRefTraits = Function<decltype(&ComplexObject::LValue)>;
        static_assert(IsType<LRefTraits::ArgsT, TypeList<int, float>>);

        // R-Value Ref (&&)
        using RRefTraits = Function<decltype(&ComplexObject::RValue)>;
        static_assert(IsType<RRefTraits::ArgsT, TypeList<int, float>>);

        // Const L-Value Ref (const &)
        using ConstLRefTraits = Function<decltype(&ComplexObject::ConstLValue)>;
        static_assert(IsType<ConstLRefTraits::ArgsT, TypeList<int, float>>);
    }

    TEST(FunctionTraitsTest, MemberFunctionsNoExceptCombinations) {
        // Ensure the macro expanded to handle 'const noexcept', etc.

        // Normal NoExcept
        using Traits = Function<decltype(&ComplexObject::NormalNoExcept)>;
        static_assert(IsType<Traits::ArgsT, TypeList<int, float>>);

        // Const NoExcept
        using CTraits = Function<decltype(&ComplexObject::ConstNoExcept)>;
        static_assert(IsType<CTraits::ArgsT, TypeList<int, float>>);
    }

    // === Functors and Lambdas ===============================

    TEST(FunctionTraitsTest, StandardLambdas) {
        auto lambda = [](int x, int y) -> bool { return x > y; };
        using Traits = Function<decltype(lambda)>;

        static_assert(IsType<Traits::ReturnT, bool>);
        static_assert(IsType<Traits::ArgsT, TypeList<int, int>>);
    }

    TEST(FunctionTraitsTest, MutableLambdas) {
        int counter = 0;
        // Mutable lambdas have a non-const operator()
        auto mutableLambda = [counter](float f) mutable -> int { return 0; };

        using Traits = Function<decltype(mutableLambda)>;

        static_assert(IsType<Traits::ReturnT, int>);
        static_assert(IsType<Traits::ArgsT, TypeList<float>>);
    }

    TEST(FunctionTraitsTest, CustomFunctors) {
        SimpleFunctor functor;
        using Traits = Function<SimpleFunctor>;

        static_assert(IsType<Traits::ReturnT, int>);
        // Note: The argument is 'const std::string&', not just 'std::string'
        static_assert(IsType<Traits::ArgsT, TypeList<const std::string&>>);
    }

    // === Edge Cases =========================================

    TEST(FunctionTraitsTest, EmptyArguments) {
        auto emptyLambda = []() {};
        using Traits = Function<decltype(emptyLambda)>;

        static_assert(IsType<Traits::ReturnT, void>);
        static_assert(IsType<Traits::ArgsT, TypeList<>>); // Empty List
    }

} // namespace QkT
