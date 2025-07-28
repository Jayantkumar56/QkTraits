

#include <QkTraits/View.h>
#include <gtest/gtest.h>


namespace Quirk::QkT {

    // =============================================================================
    // Test Fixtures and Helper Structs
    // =============================================================================

    // A simple struct for basic tests
    struct TestObject {
        int value = 10;
        std::string name = "Test";
    };

    // Test fixture for View tests
    class ViewTest : public ::testing::Test {
    protected:
        TestObject m_Obj;
    };

    // Helper structs for inheritance tests
    struct Base {
        virtual ~Base() = default;
        int BaseValue = 100;
    };

    struct Derived : public Base {
        int DerivedValue = 200;
    };



    // =============================================================================
    // Test Cases
    // =============================================================================


    // === Basic Construction and Validity ================

    TEST_F(ViewTest, DefaultConstructionIsNull) {
        View<TestObject> view;

        ASSERT_FALSE(view);
        ASSERT_EQ(view, nullptr);
    }

    TEST_F(ViewTest, ConstructionFromRawPointer) {
        View<TestObject> view(&m_Obj);

        ASSERT_TRUE(view);
        ASSERT_NE(view, nullptr);
        ASSERT_EQ(view.Get(), &m_Obj);
    }


    // === Runtime Smart Pointer Construction =============

    TEST_F(ViewTest, ConstructionFromUniquePtr) {
        auto uPtr = std::make_unique<TestObject>();
        View<TestObject> view(uPtr);

        ASSERT_EQ(view.Get(), uPtr.get());
    }

    TEST_F(ViewTest, ConstructionFromSharedPtr) {
        auto sPtr = std::make_shared<TestObject>();
        View<TestObject> view(sPtr);

        ASSERT_EQ(view.Get(), sPtr.get());
    }


    // === Pointer Semantics ==============================

    TEST_F(ViewTest, OperatorArrowAndStar) {
        View<TestObject> view(&m_Obj);

        // Test getting value from arrow and star operator
        {
            ASSERT_EQ(view->value, m_Obj.value);
            ASSERT_STREQ(view->name.c_str(), m_Obj.name.c_str());

            ASSERT_EQ((*view).value, m_Obj.value);
            ASSERT_STREQ((*view).name.c_str(), m_Obj.name.c_str());
        }

        // Test setting value from arrow and star operator
        {
            constexpr const int   modifiedValue = 99;
            constexpr const char* modifiedName  = "Modified";

            view->value = modifiedValue;
            ASSERT_EQ(m_Obj.value, modifiedValue);

            (*view).name = modifiedName;
            ASSERT_STREQ(m_Obj.name.c_str(), modifiedName);
        }
    }

    TEST_F(ViewTest, ConstViewCorrectlyAccessesConstObject) {
        const View<TestObject> constView(&m_Obj);

        ASSERT_EQ(constView->value,   m_Obj.value);    // operator->() const
        ASSERT_EQ((*constView).value, m_Obj.value);    // operator*()  const

        // The following block would fail to compile, which is correct:
        {
            //constexpr const int   modifiedValue = 99;
            //constexpr const char* modifiedName  = "Modified";
           
            //constView->value  = modifiedValue;
            //(*constView).name = modifiedName;
        }
    }


    // === Const Conversion ===============================

    TEST_F(ViewTest, MutableToConstViewConversion) {
        View<TestObject> mutableView(&m_Obj);
        View<const TestObject> constView = mutableView; // Implicit conversion

        ASSERT_TRUE(constView);
        ASSERT_EQ(constView.Get(), &m_Obj);

        ASSERT_EQ(constView->value,   m_Obj.value);    // operator->() const
        ASSERT_EQ((*constView).value, m_Obj.value);    // operator*()  const

        // The following block would fail to compile, which is correct:
        {
            //constexpr const int   modifiedValue = 99;
            //constexpr const char* modifiedName  = "Modified";

            //constView->value   = modifiedValue;
            //(*constView).value = modifiedName;
        }
    }


    // === Inheritance Conversions ========================

    TEST_F(ViewTest, UpcastingFromDerivedToBase) {
        Derived derivedObj;
        View<Derived> derivedView(&derivedObj);

        // Implicit conversion from View<Derived> to View<Base>
        View<Base> baseView = derivedView;

        // Test values from implicitly up casted view
        {
            ASSERT_TRUE(baseView);
            ASSERT_EQ(baseView->BaseValue, derivedView->BaseValue);
            ASSERT_EQ(baseView.Get(), &derivedObj);
        }
    }

    TEST_F(ViewTest, DowncastingWithStaticViewCast) {
        Derived derivedObj;
        Base* basePtr = &derivedObj;
        View<Base> baseView(basePtr);

        // Explicitly cast back down
        View<Derived> derivedView = ViewDowncast<Derived>(baseView);

        // Test values from explicitly down casted view
        {
            ASSERT_TRUE(derivedView);
            ASSERT_EQ(derivedView->DerivedValue, derivedObj.DerivedValue);
            ASSERT_EQ(derivedView.Get(), &derivedObj);
        }
    }

    // === Constexpr Tests ================================

    // Global static object to test constexpr functionality,
    constexpr static Derived g_DerivedObj;

    TEST_F(ViewTest, ConstexprConstruction) {
        static constexpr View<const Derived> view(&g_DerivedObj);

        static_assert(view);
        static_assert(view.Get() == &g_DerivedObj);

        ASSERT_TRUE(view);
    }

    TEST_F(ViewTest, ConstexprConversion) {
        static constexpr View<const Derived> derivedView(&g_DerivedObj);
        static constexpr View<const Base> baseView = &g_DerivedObj;          // Constexpr upcasting

        static_assert(baseView.Get() == &g_DerivedObj);
        ASSERT_TRUE(baseView);
    }

    TEST_F(ViewTest, ConstexprStaticViewCast) {
        static constexpr View<const Base>    baseView(&g_DerivedObj);
        static constexpr View<const Derived> derivedView = ViewDowncast<const Derived>(baseView);

        static_assert(derivedView.Get() == &g_DerivedObj);
        ASSERT_TRUE(derivedView);
    }

} // namespace Quirk::QkT
