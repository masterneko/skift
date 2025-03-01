#pragma once

#include <textbox/model.h>

#include "box.h"
#include "funcs.h"
#include "view.h"

namespace Karm::Ui {

template <typename T = None>
using OnChange = Meta::Cond<
    Meta::Same<T, None>,
    Opt<SharedFunc<void(Node &)>>,
    Opt<SharedFunc<void(Node &, T value)>>>;

template <typename T>
[[gnu::used]] static auto IGNORE(Ui::Node &, T) {}

/* --- Button --------------------------------------------------------------- */

struct MouseListener {
    enum MouseState {
        IDLE,
        HOVER,
        PRESS,
    };

    MouseState _state = IDLE;
    Math::Vec2i _pos = {0, 0};

    auto state() const {
        return _state;
    }

    bool isIdle() const {
        return _state == IDLE;
    }

    bool isHover() const {
        return _state == HOVER;
    }

    bool isPress() const {
        return _state == PRESS;
    }

    auto pos() const {
        return _pos;
    }

    bool listen(Node &node, Sys::Event &e) {
        bool result = false;
        MouseState state = _state;

        e.handle<Events::MouseEvent>([&](auto &m) {
            if (not node.bound().contains(m.pos)) {
                state = IDLE;
                return false;
            }

            if (state != PRESS) {
                state = HOVER;
            }

            _pos = m.pos - node.bound().topStart();

            if (m.type == Events::MouseEvent::PRESS) {
                state = PRESS;
                return true;
            }

            if (m.type == Events::MouseEvent::RELEASE) {
                state = HOVER;
                result = true;
                return true;
            }

            return false;
        });

        e.handle<Events::MouseLeaveEvent>([&](auto) {
            state = IDLE;
            return false;
        });

        if (state != _state) {
            _state = state;
            shouldRepaint(node);
        }

        return result;
    }
};

struct ButtonStyle {
    static constexpr isize RADIUS = 4;

    BoxStyle idleStyle;
    BoxStyle hoverStyle;
    BoxStyle pressStyle;
    BoxStyle disabledStyle = {
        .foregroundPaint = GRAY600,
    };

    static ButtonStyle none();

    static ButtonStyle regular(Gfx::ColorRamp ramp = GRAYS);

    static ButtonStyle secondary();

    static ButtonStyle primary();

    static ButtonStyle outline();

    static ButtonStyle subtle();

    static ButtonStyle destructive();

    ButtonStyle withRadius(Gfx::BorderRadius radius) const;

    ButtonStyle withForegroundPaint(Gfx::Paint paint) const;

    ButtonStyle withPadding(Layout::Spacingi spacing) const;

    ButtonStyle withMargin(Layout::Spacingi spacing) const;
};

using OnPress = Opt<Func<void(Node &)>>;

[[gnu::used]] static auto NOP(Ui::Node &) {}

Child button(OnPress onPress, ButtonStyle style, Child child);

inline auto button(OnPress onPress, ButtonStyle style) {
    return [onPress = std::move(onPress), style](Child child) mutable {
        return button(std::move(onPress), style, child);
    };
}

Child button(OnPress onPress, ButtonStyle style, Str t);

Child button(OnPress onPress, ButtonStyle style, Media::Icon i);

Child button(OnPress onPress, ButtonStyle style, Media::Icon i, Str t);

Child button(OnPress onPress, Child child);

inline auto button(OnPress onPress) {
    return [onPress = std::move(onPress)](Child child) mutable {
        return button(std::move(onPress), child);
    };
}

Child button(OnPress onPress, Str t);

Child button(OnPress onPress, Mdi::Icon i);

Child button(OnPress onPress, Mdi::Icon i, Str t);

/* --- Input ---------------------------------------------------------------- */

Child input(Strong<Textbox::Model> text, OnChange<> onChange, Gfx::TextStyle style);

Child input(Strong<Textbox::Model> text, OnChange<> onChange);

/* --- Toggle --------------------------------------------------------------- */

Child toggle(bool value, OnChange<bool> onChange);

/* --- Checkbox ------------------------------------------------------------- */

Child checkbox(bool value, OnChange<bool> onChange);

/* --- Radio ---------------------------------------------------------------- */

Child radio(bool value, OnChange<bool> onChange);

/* --- Slider --------------------------------------------------------------- */

Child slider(f64 value, OnChange<f64> onChange, Child child);

static inline auto slider(f64 value, OnChange<f64> onChange) {
    return [value, onChange = std::move(onChange)](Child child) mutable {
        return slider(value, std::move(onChange), std::move(child));
    };
}

/* --- Select --------------------------------------------------------------- */

/* --- Color ---------------------------------------------------------------- */

Child color(Gfx::Color color, OnChange<Gfx::Color> onChange);

/* --- Intent --------------------------------------------------------------- */

Child intent(Func<void(Node &, Sys::Event &e)> map, Child child);

static inline auto intent(Func<void(Node &, Sys::Event &e)> map) {
    return [map = std::move(map)](Child child) mutable {
        return intent(std::move(map), std::move(child));
    };
}

} // namespace Karm::Ui
