/**
 * Helpers used for binding method pointers to an instance if object.
 * Such bound function may be used as C-callback function.
 *
 * Adapted and extended solution described here:
 * http://p-nand-q.com/programming/cplusplus/using_member_functions_with_c_function_pointers.html
 *
 * TODO: add support for std::function
 *
 * @author mliszcz<liszcz.michal@gmail.com>
 * @license MIT
 */

#pragma once

#include <array>
#include <utility>

namespace invoker {

    constexpr int SLOTS = 10;

    /** Plain old C callback. */
    template<typename Ret, typename... Args>
    using FreeFnPtr = Ret (*)(Args...);

    /** C++ method. */
    template<typename Clazz, typename Ret, typename... Args>
    using MethodPtr = Ret (Clazz::*)(Args...);

    /**
     * Slot that stores plain function as a method bound to an class instance.
     * @param TId Slot index type.
     */
    template<typename TId, typename Clazz, typename Ret, typename... Args>
    class SlotBase {
    public:
        Clazz* clazz = nullptr;
        MethodPtr<Clazz, Ret, Args...> method = nullptr;
        FreeFnPtr<Ret, Args...> freefn = nullptr;

        SlotBase(FreeFnPtr<Ret, Args...> fp) : freefn(fp) { }

        static Ret staticInvoke(TId, Args...);
    };

    /**
     * Derived SlotBase. There are *many* specializations created
     * at compile time, each with diffrent Id.
     */
    template <int Id, typename Clazz, typename Ret, typename... Args>
    class IndexedSlot : public SlotBase<int, Clazz, Ret, Args...>  {
    public:
        IndexedSlot()
            : SlotBase<int, Clazz, Ret, Args...>
                (&IndexedSlot<Id, Clazz, Ret, Args...>::staticImpl) { }

        static Ret staticImpl(Args... args) {
            return SlotBase<int, Clazz, Ret, Args...>::staticInvoke(Id, args...);
        }
    };

    /**
     * Functor that wraps plain C function and its state.
     */
    template<typename Clazz, typename Ret, typename... Args>
    class BoundFnPtr {
    public:

        template<std::size_t... Indices>
        static constexpr std::array<SlotBase<int, Clazz, Ret, Args...>*, sizeof...(Indices)>
        createSlotArrayInner(std::index_sequence<Indices...>) {
            return {{ new IndexedSlot<Indices, Clazz, Ret, Args...>()... }};
        }

        template<std::size_t N>
        static constexpr std::array<SlotBase<int, Clazz, Ret, Args...>*, N>
        createSlotArray() {
            return createSlotArrayInner(std::make_index_sequence<N>{});
        }

        static std::array<SlotBase<int, Clazz, Ret, Args...>*, SLOTS> genericarray;

        BoundFnPtr(Clazz* clazz, MethodPtr<Clazz, Ret, Args...> method) {

            for (int i=0; i<SLOTS; ++i) {
                if (genericarray[i]->clazz == nullptr) {
                    genericarray[i]->clazz = clazz;
                    genericarray[i]->method = method;
                    index = i;
                    freefn = genericarray[i]->freefn;
                    return;
                }
            }
            throw std::runtime_error("No slot available");
        }

        virtual ~BoundFnPtr() {
            genericarray[index]->clazz = nullptr;
        }

        operator FreeFnPtr<Ret, Args...>() const {
            return freefn;
        }

    private:
        FreeFnPtr<Ret, Args...> freefn;
        int index;
    };

    template<typename Clazz, typename Ret, typename... Args>
    std::array<SlotBase<int, Clazz, Ret, Args...>*, SLOTS>
        BoundFnPtr<Clazz, Ret, Args...>::genericarray =
            BoundFnPtr<Clazz, Ret, Args...>::createSlotArray<SLOTS>();

    template<typename TId, typename Clazz, typename Ret, typename... Args>
    Ret SlotBase<TId, Clazz, Ret, Args...>::staticInvoke(TId id, Args... args) {
        auto x = BoundFnPtr<Clazz, Ret, Args...>::genericarray;
        return ((x[id]->clazz)->*(x[id]->method))(args...);
    }
}
