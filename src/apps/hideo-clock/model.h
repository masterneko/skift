#pragma once

#include <karm-ui/reducer.h>

namespace Hideo::Clock {

enum struct Page {
    ALARM,
    CLOCK,
    TIMER,
    STOPWATCH,

    _DEFAULT = ALARM,
};

static inline Str toStr(Page page) {
    switch (page) {
    case Page::CLOCK:
        return "Clock";
    case Page::STOPWATCH:
        return "Stopwatch";
    case Page::TIMER:
        return "Timer";
    case Page::ALARM:
        return "Alarm";
    default:
        return "Unknown";
    }
}

struct State {
    Page page = Page::_DEFAULT;
};

using Action = Var<Page>;

void reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Hideo::Clock
