

#pragma once

#include <memory>


namespace Quirk::QkT {

    template<typename T>
    class View {
    public:
        // ==== Constructors ======================================================

        constexpr View()       noexcept = default;
        constexpr View(T* ptr) noexcept : m_Ptr(ptr) {}


        // ==== Smart Pointer Conversions (Runtime only) ==========================
        // These cannot be constexpr because .get() is not constexpr

        View(const std::unique_ptr<T>& ptr) noexcept : m_Ptr(ptr.get()) {}
        View(const std::shared_ptr<T>& ptr) noexcept : m_Ptr(ptr.get()) {}


        // ==== Deleted constructors for temporaries ==============================

        View(std::unique_ptr<T>&&) noexcept = delete;
        View(std::shared_ptr<T>&&) noexcept = delete;


        // ==== Upcasting/Const-Converting Constructor ============================

        template<typename U>
        requires(!std::is_const_v<T>&& std::is_convertible_v<U*, T*>)
        constexpr View(View<U>& other) noexcept : m_Ptr(other.Get()) {}

        // Constructor for converting from other CONST views
        // Example: const View<Derived>&  -->  View<const Base>
        template<typename U>
        requires(std::is_convertible_v<const U*, T*>)
        constexpr View(const View<U>& other) noexcept : m_Ptr(other.Get()) {}


        // ==== Pointer Semantics =================================================

        constexpr       T* operator->()       noexcept { return m_Ptr; }
        constexpr const T* operator->() const noexcept { return m_Ptr; }

        constexpr       T& operator*()       noexcept { return *m_Ptr; }
        constexpr const T& operator*() const noexcept { return *m_Ptr; }


        // ==== Getters and Validity Checks =======================================

        constexpr       T* Get()       noexcept { return m_Ptr; }
        constexpr const T* Get() const noexcept { return m_Ptr; }

        constexpr      operator bool()            const noexcept { return m_Ptr != nullptr; }
        constexpr bool operator==(std::nullptr_t) const noexcept { return m_Ptr == nullptr; }

    private:
        T* m_Ptr{ nullptr };
    };


    // ==== Explicit Downcasting Function =========================================

    // For downcasting from a non-const source to a non-const view
    template<typename Derived, typename Base>
    requires std::is_base_of_v<Base, Derived>
    constexpr View<Derived> ViewDowncast(View<Base>& base_view) noexcept {
        return View<Derived>(static_cast<Derived*>(base_view.Get()));
    }

    // For downcasting from a const source to a const view
    template<typename Derived, typename Base>
    requires std::is_base_of_v<Base, Derived>
    constexpr View<const Derived> ViewDowncast(const View<Base>& base_view) noexcept {
        return View<const Derived>(static_cast<const Derived*>(base_view.Get()));
    }

} // namespace Quirk::QkT
