# QkTraits

A modern **C++ header-only library** for type traits and utilities.  
It extends the standard `<type_traits>` library with additional utilities for **type lists, pointer traits, string literals, and generic views**.

### For a deeper look at the design choices behind QkTraits, you can check out my blog posts:

[StingLiteral in Template Arguments.](https://jayantkumar56.github.io/Quirk-Blog/Blogs-And-Insights/Compile-Time-String.html)

[TypeList: Deep Dive into C++ Meta-Programming](https://jayantkumar56.github.io/Quirk-Blog/Blogs-And-Insights/TypeLists.html)

---

## Namespace

All public APIs are contained within the **`Quirk::QkT`** namespace.

```cpp
// TypeList
using myTypes = Quirk::QkT::TypeList<int, double, char>;

// StringLiteral
Quirk::QkT::StringLiteral strLiteral{"Hello"};
```

This emphasizes that QkTraits is part of the broader Quirk ecosystem, while remaining usable as a standalone library.

---

## Features
- **PointerTraits** – Utilities for working with raw and smart pointers.
- **TypeList** – Compile-time lists of types with transformation utilities.
- **TypeTraits** – General type trait extensions beyond the STL.
- **StringLiteral** – Compile-time string handling.
- **View** – Lightweight generic view abstraction.

---

## Integration

QkTraits is header-only. You can include it in your project however you prefer.  
If you use **CMake**, the recommended way is:

```cmake
# Add QkTraits to your project (adjust the path as needed)
add_subdirectory(external/QkTraits)

# Link your target to QkTraits
target_link_libraries(${PROJECT_NAME} PRIVATE QkTraits)
```

This sets up the include paths automatically.
(Currently it only exports headers, but additional features may be added in the future.)

If you don’t use CMake, simply add the include/ directory to your compiler’s include path and include the headers directly:

```cpp
#include <QkTraits/TypeList.h>
#include <QkTraits/PointerTraits.h>
#include <QkTraits/StringLiteral.h>
#include <QkTraits/View.h>
#include <QkTraits/TypeTraits.h>
```

---

## Example

### TypeList – Iterating Over Types

```cpp
#include <QkTraits/TypeList.h>
#include <iostream>
#include <typeinfo>

using MyTypes = Quirk::QkT::TypeList<int, double, char>;

int main() {
    MyTypes::ForEach([]<typename T>() {
        std::cout << "Type: " << typeid(T).name() << "\n";
    });
}
```

### TypeList – Expanding Into a Function

```cpp
#include <QkTraits/TypeList.h>
#include <iostream>

using MyTypes = Quirk::QkT::TypeList<int, double, char>;

int main() {
    int count = MyTypes::InvokeWithTypesExpanded(
        []<typename... Ts>() {
            return sizeof...(Ts);
        }
    );

    std::cout << "Number of types: " << count << "\n";
}
```

### StringLiteral — Using as NTTPs

```cpp
#include <QkTraits/StringLiteral.h>
#include <iostream>

int main() {
    auto print = []<Quirk::QkT::StringLiteral str>() {
        std::cout << str.View() << '\n'; // "Hello" known at compile time
    };

    // invoke with a string literal as NTTP
    print.template operator()<"Hello">();
    print.template operator()<"World">();
}
```

### ValueList — Compile-Time Lists of Constants

```cpp
#include <QkTraits/ValueList.h>
#include <iostream>

using Numbers = Quirk::QkT::ValueList<1, 2, 3, 5, 8>;

int main() {
    // Iterate over values
    Numbers::ForEach([]<auto V>() {
        std::cout << V << " ";   // prints: 1 2 3 5 8
    });

    // Access by index
    static_assert(Numbers::Get<2> == 3);

    // Check membership
    static_assert(Numbers::Contains<5>);

    // Transform into a tuple
    auto tuple = Quirk::QkT::ToTuple<Numbers>;
    static_assert(std::get<4>(tuple) == 8);
}
```


### StringList — Working with Compile-Time Strings

```cpp
#include <QkTraits/StringList.h>
#include <iostream>

// StringList of compile-time strings
using Words = Quirk::QkT::StringList<"Hello", "from", "QkTraits">;

int main() {
    // Iterate over strings
    Words::ForEach([]<auto Str>() {
        std::cout << Str.View() << " ";
        // prints: Hello from QkTraits
    });

    // Access by index
    static_assert(Words::Get<1> == "from");

    // Membership and index
    static_assert(Words::Contains<"QkTraits">);
    static_assert(Words::IndexOf<"Hello"> == 0);

    // Reverse list
    using Reversed = Quirk::QkT::ReverseStr_T<Words>;
}
```

### View — Lightweight Non-Owning Pointer Wrapper

```cpp
#include <QkTraits/View.h>
#include <memory>
#include <iostream>

struct Base { virtual void hello() { std::cout << "Base\n"; } };
struct Derived : Base { void hello() override { std::cout << "Derived\n"; } };

int main() {
    // Constructing from raw pointer
    Derived d;
    Quirk::QkT::View<Derived> view{&d};
    view->hello();                 // prints: Derived

    // Constructing from smart pointer (non-owning)
    auto ptr = std::make_unique<Derived>();
    Quirk::QkT::View<Base> base_view{ptr};
    base_view->hello();            // prints: Derived

    // Downcasting
    auto derived_view = Quirk::QkT::ViewDowncast<Derived>(base_view);
    derived_view->hello();         // prints: Derived
}
```

## License

This project is licensed under the MIT License.
