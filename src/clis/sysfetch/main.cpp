#include <karm-cli/cursor.h>
#include <karm-cli/style.h>
#include <karm-sys/entry.h>
#include <karm-sys/info.h>

static Str const ART = R"(
        ___
       /\  \
      /::\  \
     /:/\ \  \
    _\:\~\ \  \
   /\ \:\ \ \__\
   \:\ \:\ \/__/
    \:\ \:\__\
     \:\/:/  /
      \::/  /
       \/__/
)";

static Str const BLOCK = "███";

namespace Sysfetch {

auto title(Str text) {
    return Io::format("{}{}", Cli::Cmd::forward(19), Cli::styled(text, Cli::style().bold()));
}

Res<> dumpUserInfo() {
    auto userinfo = try$(Sys::userinfo());
    Sys::println("{}: {} {}", title("User"), userinfo.name);
    Sys::println("{}: {} {}", title("Home"), userinfo.home);
    Sys::println("{}: {} {}", title("Shell"), userinfo.shell);
    return Ok();
}

Res<> dumpSysInfo() {
    auto sysinfo = try$(Sys::sysinfo());
    Sys::println("{}: {} {}", title("System"), sysinfo.sysName, sysinfo.sysVersion);
    Sys::println("{}: {} {}", title("Kernel"), sysinfo.kernelName, sysinfo.kernelVersion);
    Sys::println("{}: {}", title("Hostname"), sysinfo.hostname);
    Sys::println("{}: {}", title("Form factor"), Sys::useFormFactor() == Sys::FormFactor::DESKTOP ? "🖥️" : "📱");
    return Ok();
}

Res<> dumpMemInfo() {
    auto meminfo = try$(Sys::meminfo());
    Sys::println("{}: {} / {}", title("Memory"), meminfo.physicalUsed, meminfo.physicalTotal);
    Sys::println("{}: {} / {}", title("Swap"), meminfo.swapUsed, meminfo.swapTotal);
    return Ok();
}

Res<> dumpCpusInfo() {
    auto cpusinfo = try$(Sys::cpusinfo());

    for (auto &cpu : cpusinfo) {
        Sys::println("{}: {} {} {} MHz", title("CPU"), cpu.name, cpu.brand, cpu.freq);
    }

    return Ok();
}

Res<> testAnsi() {
    Sys::print("{}", Cli::Cmd::forward(19));
    for (auto const c : Cli::DARK_COLORS) {
        Sys::print("{}", Cli::styled(BLOCK, c));
    }

    Sys::println("");

    Sys::print("{}", Cli::Cmd::forward(19));
    for (auto const c : Cli::LIGHT_COLORS) {
        Sys::print("{}", Cli::styled(BLOCK, c));
    }

    Sys::println("");

    return Ok();
}

} // namespace Sysfetch

Res<> entryPoint(Sys::Ctx &) {
    Res<> res = Ok();

    Sys::println("{}", Cli::styled(ART, Cli::BLUE));
    Sys::print("{}", Cli::Cmd::up(12));

    if (not(res = Sysfetch::dumpUserInfo())) {
        Sys::errln("{}: {}", Sysfetch::title("User"), Cli::styled(res.none().msg(), Cli::RED_LIGHT));
    }

    if (not(res = Sysfetch::dumpSysInfo())) {
        Sys::errln("{} {}", Sysfetch::title("System"), Cli::styled(res.none().msg(), Cli::RED_LIGHT));
    }

    if (not(res = Sysfetch::dumpMemInfo())) {
        Sys::errln("{}: {}", Sysfetch::title("Memory"), Cli::styled(res.none().msg(), Cli::RED_LIGHT));
    }

    if (not(res = Sysfetch::dumpCpusInfo())) {
        Sys::errln("{}: {}", Sysfetch::title("CPUs"), Cli::styled(res.none().msg(), Cli::RED_LIGHT));
    }

    Sys::println("");
    res = Sysfetch::testAnsi();
    Sys::println("");

    return res;
}
