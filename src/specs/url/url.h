#pragma once

// https://url.spec.whatwg.org/

#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/expr.h>
#include <karm-io/fmt.h>
#include <karm-io/sscan.h>

namespace Url {

/* --- Path ----------------------------------------------------------------- */

struct Path {
    static constexpr auto SEP = '/';

    bool rooted = false;
    Vec<String> _parts;

    static Path parse(Io::SScan &s, bool inUrl = false, bool stopAtWhitespace = false);

    static Path parse(Str str, bool inUrl = false, bool stopAtWhitespace = false);

    void normalize();

    Str basename() const;

    Path join(Path const &other) const;

    Path join(Str other) const;

    void append(Str part) {
        _parts.pushBack(part);
    }

    Path parent(usize n = 0) const;

    Res<usize> write(Io::TextWriter &writer) const;

    String str() const;

    auto iter() const {
        return ::iter(_parts);
    }

    auto operator[](usize i) const {
        return _parts[i];
    }

    auto len() const {
        return _parts.len();
    }

    auto operator<=>(Path const &) const = default;

    Str sufix() const {
        if (not _parts.len())
            return "";
        auto dotIndex = lastIndexOf(last(_parts), '.');
        if (not dotIndex.has())
            return "";
        return next(last(_parts), *dotIndex + 1);
    }
};

/* --- Url ------------------------------------------------------------------ */

struct Url {
    String scheme;
    String authority;
    String host;
    Opt<usize> port;
    Path path;
    String query;
    String fragment;

    static Url parse(Io::SScan &s);

    static Url parse(Str str);

    static bool isUrl(Str str);

    bool rooted() const {
        return path.rooted;
    }

    Url join(Path const &other) const;

    Url join(Str other) const;

    Str basename() const;

    void append(Str part) {
        path.append(part);
    }

    Url parent(usize n = 0) const;

    Res<usize> write(Io::TextWriter &writer) const;

    String str() const;

    auto iter() const {
        return path.iter();
    }

    auto len() const {
        return path.len();
    }

    auto operator<=>(Url const &) const = default;
};

Res<Url> parseUrlOrPath(Str str);

} // namespace Url

inline auto operator""_path(char const *str, usize len) {
    return Url::Path::parse({str, len});
}

inline auto operator""_url(char const *str, usize len) {
    return Url::Url::parse({str, len});
}

inline auto operator/(Url::Url const &url, Url::Path const &path) {
    return url.join(path);
}

inline auto operator/(Url::Url const &url, Str path) {
    return url.join(path);
}

inline auto operator/(Url::Path const &path, Url::Path const &other) {
    return path.join(other);
}

inline auto operator/(Url::Path const &path, Str other) {
    return path.join(other);
}

template <>
struct Karm::Io::Formatter<Url::Path> {
    Res<usize> format(Io::TextWriter &writer, Url::Path const &path) {
        return path.write(writer);
    }
};

template <>
struct Karm::Io::Formatter<Url::Url> {
    Res<usize> format(Io::TextWriter &writer, Url::Url const &url) {
        return url.write(writer);
    }
};
