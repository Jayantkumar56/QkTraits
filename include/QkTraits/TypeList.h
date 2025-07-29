

#pragma once

#include <utility>


namespace Quirk::QkT {

    template<typename ...Types>
    struct TypeList {
        template<typename Function, typename ...Args>
        constexpr static void ForEach(Function&& func, Args&& ...args) {
            (func.template operator() <Types> (args...), ...);
        }

        template<typename Function, typename ...Args>
        constexpr static decltype(auto) InvokeWithTypesExpanded(Function&& func, Args&& ...args) {
            return std::forward<Function>(func).template operator() <Types...> (std::forward<Args>(args)...);
        }
    };

} // namespace Quirk::QkT
