#include <hjert-core/arch.h>
#include <karm-base/lock.h>
#include <karm-sys/chan.h>

#include <karm-logger/_embed.h>

namespace Karm::Logger::_Embed {

static Lock _lock;

void loggerLock() {
    _lock.acquire();
}

void loggerUnlock() {
    _lock.release();
}

Io::TextWriter &loggerOut() {
    return Hjert::Arch::loggerOut();
}

} // namespace Karm::Logger::_Embed
