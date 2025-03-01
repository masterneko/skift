#pragma once

#include "error.h"
#include "std.h"
#include "var.h"

namespace Karm {

template <typename T = None>
struct Ok {
    T inner;

    template <typename... Args>
    always_inline constexpr Ok(Args &&...args)
        : inner(std::forward<Args>(args)...) {}

    operator bool() const = delete;

    always_inline auto operator<=>(Ok const &) const
        requires Meta::Comparable<T>
    = default;

    T take() {
        return std::move(inner);
    }
};

template <typename... Args>
Ok(Args &&...) -> Ok<Meta::RemoveConstVolatileRef<Args>...>;

template <typename V = None, typename E = Error>
struct [[nodiscard]] Res {
    using Inner = Var<Ok<V>, E>;

    Inner _inner;

    always_inline constexpr Res(Ok<V> const &ok)
        : _inner(ok) {}

    always_inline constexpr Res(Ok<V> &&ok)
        : _inner(std::move(ok)) {}

    template <typename U>
    always_inline constexpr Res(Ok<U> ok)
        : _inner(Ok<V>{ok.inner}) {}

    always_inline constexpr Res(E err)
        : _inner(err) {}

    always_inline constexpr Res(None)
        : _inner(E{}) {}

    template <typename U>
    always_inline constexpr Res(Res<U, E> other)
        : _inner(other._inner) {}

    always_inline constexpr explicit operator bool() const {
        return _inner.template is<Ok<V>>();
    }

    always_inline constexpr bool has() const {
        return _inner.template is<Ok<V>>();
    }

    always_inline constexpr E const &none() const {
        if (not _inner.template is<E>())
            panic("none() called on an ok");
        return _inner.template unwrap<E>();
    }

    always_inline constexpr V &unwrap(char const *msg = "unwraping an error") {
        if (not _inner.template is<Ok<V>>())
            panic(msg);
        return _inner.template unwrap<Ok<V>>().inner;
    }

    always_inline constexpr V const &unwrap(char const *msg = "unwraping an error") const {
        if (not _inner.template is<Ok<V>>())
            panic(msg);
        return _inner.template unwrap<Ok<V>>().inner;
    }

    always_inline constexpr V take() {
        if (not _inner.template is<Ok<V>>())
            panic("take() called on an error");
        return _inner.template unwrap<Ok<V>>().take();
    }

    template <typename U>
    always_inline constexpr Res<V, U> mapErr(auto f) {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>();
        return f(_inner.template unwrap<E>());
    }

    template <typename U>
    always_inline constexpr Res<V, U> mapErr() {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>();
        return U{};
    }

    template <typename U>
    always_inline constexpr Res<U, E> mapValue(auto f) {
        if (_inner.template is<Ok<V>>())
            return f(_inner.template unwrap<Ok<V>>().inner);
        return _inner.template unwrap<E>();
    }

    template <typename U>
    always_inline constexpr Res<U, E> mapValue() {
        if (_inner.template is<Ok<V>>())
            return _inner.template unwrap<Ok<V>>().inner;
        return _inner.template unwrap<E>();
    }

    always_inline auto operator<=>(Res const &) const
        requires Meta::Comparable<Inner>
    = default;
};

static_assert(Tryable<Res<isize, Error>>);

} // namespace Karm
