#include <karm-ui/anim.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>

#include "sidenav.h"

namespace Hideo {

Ui::Child sidenav(Ui::Children children) {
    return Ui::vflow(8, children) |
           Ui::spacing(8) |
           Ui::vscroll() |
           Ui::minSize({198, Ui::UNCONSTRAINED});
}

Ui::Child sidenavTree(Mdi::Icon icon, String title, Ui::Slot child) {
    return Ui::state(true, [=, child = std::move(child)](bool state, auto bind) {
        return Ui::vflow(
            Ui::button(
                bind(not state),
                Ui::ButtonStyle::subtle(),
                Ui::hflow(
                    Ui::empty(8),
                    Ui::icon(icon, 18),
                    Ui::empty(12),
                    Ui::labelMedium(title) |
                        Ui::vcenter() |
                        Ui::grow(),
                    Ui::icon(state ? Mdi::CHEVRON_UP : Mdi::CHEVRON_DOWN, 18)) |
                    Ui::spacing({0, 8, 12, 8})),

            state
                ? child() |
                      Ui::spacing({32, 0, 0, 0}) |
                      Ui::slideIn(Ui::SlideFrom::TOP)
                : Ui::empty());
    });
}

Ui::Child sidenavItem(bool selected, Ui::OnPress onPress, Mdi::Icon icon, String title) {
    auto buttonStyle = Ui::ButtonStyle::regular();

    buttonStyle.idleStyle = {
        .borderRadius = 4,
        .backgroundPaint = selected ? Ui::GRAY900 : Gfx::ALPHA,
    };

    auto indicator = box(Ui::BoxStyle{
                             .borderRadius = 99,
                             .backgroundPaint = selected ? Ui::ACCENT600 : Gfx::ALPHA,
                         },
                         Ui::empty(2));

    return Ui::button(
        std::move(onPress),
        buttonStyle,
        Ui::spacing(
            {0, 8, 12, 8},
            hflow(
                indicator,
                Ui::empty(8),
                Ui::icon(icon, 18),
                Ui::empty(12),
                Ui::labelMedium(title) | Ui::center())));
}

} // namespace Hideo
