#pragma once

#include <karm-meta/callable.h>
#include <karm-meta/traits.h>

#include "error.h"
#include "inert.h"
#include "macros.h"
#include "std.h"
#include "try.h"

namespace Karm {

template <typename T>
struct Opt;

template <typename T>
struct [[nodiscard]] Opt {
    struct _Empty {};

    bool _present;
    union {
        _Empty _empty;
        T _value;
    };

    always_inline constexpr Opt()
        : _present(false), _empty() {}

    always_inline constexpr Opt(None)
        : _present(false), _empty() {}

    template <typename U = T>
    always_inline constexpr Opt(U &&value)
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::MoveConstructible<T, U>)
        : _present(true), _value(std::forward<U>(value)) {}

    always_inline constexpr Opt(Opt const &other)
        requires(Meta::CopyConstructible<T>)
        : _present(other._present) {
        if (_present)
            std::construct_at(&_value, other.unwrap());
    }

    always_inline constexpr Opt(Opt &&other) : _present(other._present) {
        if (_present)
            std::construct_at(&_value, other.take());
    }

    always_inline constexpr ~Opt() {
        clear();
    }

    always_inline constexpr Opt &operator=(None) {
        clear();
        return *this;
    }

    always_inline constexpr Opt &operator=(T const &value) {
        clear();
        _present = true;
        std::construct_at(&_value, value);
        return *this;
    }

    always_inline constexpr Opt &operator=(T &&value) {
        clear();
        _present = true;
        std::construct_at(&_value, std::move(value));
        return *this;
    }

    always_inline constexpr Opt &operator=(Opt const &other) {
        *this = Opt(other);
        return *this;
    }

    always_inline constexpr Opt &operator=(Opt &&other) {
        clear();
        if (other._present) {
            _present = true;
            std::construct_at(&_value, other.take());
        }
        return *this;
    }

    always_inline constexpr bool operator==(None) const {
        return not _present;
    }

    always_inline constexpr explicit operator bool() const {
        return _present;
    }

    always_inline constexpr bool has() const {
        return _present;
    }

    always_inline constexpr T *operator->() {
        if (not _present) {
            panic("Unwrapping None");
        }
        return &_value;
    }

    always_inline constexpr T &operator*() {
        if (not _present) {
            panic("Unwrapping None");
        }
        return _value;
    }

    always_inline constexpr T const *operator->() const {
        if (not _present) {
            panic("Unwrapping None");
        }
        return &_value;
    }

    always_inline constexpr T const &operator*() const {
        if (not _present) {
            panic("Unwrapping None");
        }
        return _value;
    }

    template <typename... Args>
    always_inline constexpr T &emplace(Args &&...args) {
        clear();
        _present = true;
        std::construct_at(&_value, std::forward<Args>(args)...);
        return _value;
    }

    always_inline constexpr void clear() {
        if (_present) {
            _value.~T();
            _present = false;
        }
    }

    always_inline constexpr None none() const {
        return NONE;
    }

    always_inline constexpr T &unwrap(char const *msg = "unwraping none") {
        if (not _present)
            panic(msg);
        return _value;
    }

    always_inline constexpr T const &unwrap(char const *msg = "unwraping none") const {
        if (not _present)
            panic(msg);
        return _value;
    }

    always_inline constexpr T take(char const *msg = "unwraping none") {
        if (not _present)
            panic(msg);
        T v = std::move(_value);
        clear();
        return v;
    }

    always_inline constexpr auto visit(auto visitor)
        -> decltype(visitor(_value)) {
        if (_present)
            return visitor(_value);
        return {};
    }

    always_inline constexpr auto visit(auto visitor) const
        -> decltype(visitor(_value)) {
        if (_present)
            return visitor(_value);
        return {};
    }

    // call operator
    template <typename... Args>
    always_inline constexpr auto operator()(Args &&...args) {
        using OptRet = Opt<Meta::Ret<T, Args...>>;

        if constexpr (Meta::Same<void, Meta::Ret<T, Args...>>) {
            // Handle void return type
            if (not _present) {
                return false;
            }
            _value(std::forward<Args>(args)...);
            return true;
        } else {
            // Handle non-void return type
            if (not _present) {
                return OptRet{NONE};
            }

            return OptRet{_value(std::forward<Args>(args)...)};
        }
    }

    // call operator
    template <typename... Args>
    always_inline constexpr auto operator()(Args &&...args) const {
        using OptRet = Opt<Meta::Ret<T, Args...>>;

        if constexpr (Meta::Same<void, Meta::Ret<T, Args...>>) {
            // Handle void return type
            if (not _present) {
                return false;
            }
            _value(std::forward<Args>(args)...);
            return true;
        } else {
            // Handle non-void return type
            if (not _present) {
                return OptRet{NONE};
            }

            return OptRet{_value(std::forward<Args>(args)...)};
        }
    }

    always_inline constexpr auto mapValue(auto f) -> Opt<decltype(f(unwrap()))> {
        if (_present) {
            return {f(unwrap())};
        }
        return {NONE};
    }

    always_inline constexpr std::partial_ordering operator<=>(Opt const &other) const {
        if constexpr (Meta::Comparable<T>)
            if (_present and other._present)
                return _value <=> other._value;
        return std::partial_ordering::unordered;
    }

    always_inline constexpr bool operator==(Opt const &other) const {
        if constexpr (Meta::Equatable<T>)
            if (_present and other._present)
                return _value == other._value;
        return false;
    }
};

static_assert(Tryable<Opt<isize>>);

} // namespace Karm
