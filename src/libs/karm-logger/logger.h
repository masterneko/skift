#pragma once

#include <karm-base/loc.h>
#include <karm-cli/style.h>
#include <karm-io/fmt.h>

#include "_embed.h"

namespace Karm {

struct Level {
    isize value;
    char const *name;
    Cli::Style style;
};

struct Format {
    Str str;
    Loc loc;

    Format(char const *str, Loc loc = Loc::current())
        : str(str), loc(loc) {
    }

    Format(Str str, Loc loc = Loc::current())
        : str(str), loc(loc) {
    }
};

static constexpr Level PRINT = {-1, "print", Cli::BLUE};
static constexpr Level DEBUG = {0, "debug", Cli::BLUE};
static constexpr Level INFO = {1, "info ", Cli::GREEN};
static constexpr Level WARNING = {2, "warn ", Cli::YELLOW};
static constexpr Level ERROR = {3, "error", Cli::RED};
static constexpr Level FATAL = {4, "fatal", Cli::style(Cli::RED).bold()};

inline void _log(Level level, Format format, Io::_Args &args) {
    Logger::_Embed::loggerLock();

    if (level.value != -1) {
        Io::format(Logger::_Embed::loggerOut(), "{} ", Cli::styled(level.name, level.style)).unwrap();
        Io::format(Logger::_Embed::loggerOut(), "{}{}:{}: ", Cli::reset().fg(Cli::GRAY_DARK), format.loc.file, format.loc.line).unwrap();
    }

    Io::format(Logger::_Embed::loggerOut(), "{}", Cli::reset()).unwrap();
    Io::_format(Logger::_Embed::loggerOut(), format.str, args).unwrap();
    Io::format(Logger::_Embed::loggerOut(), "{}\n", Cli::reset()).unwrap();
    Logger::_Embed::loggerOut().flush().unwrap();

    Logger::_Embed::loggerUnlock();
}

template <typename... Args>
inline void logPrint(Format format, Args &&...va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(PRINT, format, args);
}

template <typename... Args>
inline void logDebug(Format format, Args &&...va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(DEBUG, format, args);
}

template <typename... Args>
inline void logInfo(Format format, Args &&...va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(INFO, format, args);
}

template <typename... Args>
inline void logWarn(Format format, Args &&...va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(WARNING, format, args);
}

inline void logTodo(Loc loc = Loc::current()) {
    logWarn("todo: {}", loc.func);
}

template <typename... Args>
inline void logError(Format format, Args &&...va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(ERROR, format, args);
}

template <typename... Args>
[[noreturn]] inline void logFatal(Format format, Args &&...va) {
    Io::Args<Args...> args{std::forward<Args>(va)...};
    _log(FATAL, format, args);
    panic("fatal error occured, see logs");
}

} // namespace Karm
