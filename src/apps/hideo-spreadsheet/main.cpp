#include <hideo-base/row.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "app.h"

namespace Hideo::Spreadsheet {

Ui::Child formula() {
    return Ui::box(
        {
            .borderRadius = 4,
            .borderWidth = 1,
            .backgroundPaint = Ui::GRAY800,
        },
        Ui::hflow(
            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FUNCTION),
            Ui::grow(NONE)));
}

Ui::Child toolbar(State const &state) {
    return Ui::toolbar(
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FILE),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FOLDER),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE_PLUS),
        Ui::separator(),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::UNDO),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::REDO),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_PAINT),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_CLEAR),
        Ui::separator(),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FILTER),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::SIGMA),
        Ui::separator(),
        Ui::empty(4),
        formula() | Ui::grow(),
        Ui::empty(4),
        Ui::button(Model::bind<ToggleProperties>(), state.propertiesVisible ? Ui::ButtonStyle::regular() : Ui::ButtonStyle::subtle(), Mdi::BRUSH_VARIANT));
}

Ui::Child properties() {
    return Ui::vscroll(
        Ui::vflow(
            Hideo::titleRow("Text Properties"),

            Hideo::colorRow(Gfx::RED, Ui::IGNORE<Gfx::Color>, "Color"),

            Hideo::row(
                NONE,
                "Format",
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_BOLD),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_ITALIC),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_UNDERLINE),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_STRIKETHROUGH))),

            Hideo::row(
                NONE,
                "Wrapping",
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::WRAP),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::WRAP_DISABLED))),

            Hideo::row(
                NONE,
                "Horizontal Align",
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Model::bind<UpdateStyleHalign>(Align::START), Ui::ButtonStyle::subtle(), Mdi::ALIGN_HORIZONTAL_LEFT),
                    Ui::button(Model::bind<UpdateStyleHalign>(Align::CENTER), Ui::ButtonStyle::subtle(), Mdi::ALIGN_HORIZONTAL_CENTER),
                    Ui::button(Model::bind<UpdateStyleHalign>(Align::END), Ui::ButtonStyle::subtle(), Mdi::ALIGN_HORIZONTAL_RIGHT))),

            Hideo::row(
                NONE,
                "Vertical Align",
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Model::bind<UpdateStyleValign>(Align::START), Ui::ButtonStyle::subtle(), Mdi::ALIGN_VERTICAL_TOP),
                    Ui::button(Model::bind<UpdateStyleValign>(Align::CENTER), Ui::ButtonStyle::subtle(), Mdi::ALIGN_VERTICAL_CENTER),
                    Ui::button(Model::bind<UpdateStyleValign>(Align::END), Ui::ButtonStyle::subtle(), Mdi::ALIGN_VERTICAL_BOTTOM))),

            Ui::separator(),

            Hideo::titleRow("Cell Properties"),
            Hideo::colorRow(Gfx::RED, Ui::IGNORE<Gfx::Color>, "Background Color"),
            Hideo::colorRow(Gfx::RED, Ui::IGNORE<Gfx::Color>, "Border Color"),

            Hideo::row(
                NONE,
                "Borders",
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_ALL),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_NONE),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_VERTICAL),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_HORIZONTAL))),

            Hideo::row(
                NONE,
                "",
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_TOP),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_BOTTOM),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_LEFT),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_RIGHT))),

            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::TABLE_MERGE_CELLS, "Merge Cells"),

            Ui::separator(),

            Hideo::titleRow("Sheet Properties")));
}

Ui::Child tabItem(State const &state, Sheet const &sheet, usize index) {
    return Ui::button(
        Model::bind<SwitchSheet>(index),
        state.active == index
            ? Ui::ButtonStyle::secondary()
            : Ui::ButtonStyle::subtle(),
        sheet.name);
}

Ui::Child tabs(State const &state) {
    return Ui::hflow(
        4,
        Ui::hflow(
            iter(state.book.sheets)
                .mapi([&](auto const &s, usize i) {
                    return tabItem(state, s, i);
                })
                .collect<Ui::Children>()),

        Ui::button(
            Ui::NOP,
            Ui::ButtonStyle::subtle(),
            Mdi::PLUS));
}

Ui::Child app() {
    return Ui::reducer<Model>({}, [](auto const &s) {
        auto tb = Ui::titlebar(Mdi::TABLE, "Spreadsheet", tabs(s));
        auto body = table(s) | Ui::grow();
        if (s.propertiesVisible) {
            body = hflow(body, Ui::separator(), properties());
        }

        return Ui::vflow(
                   tb,
                   toolbar(s),
                   body | Ui::grow()) |
               Ui::pinSize({800, 600}) | Ui::dialogLayer();
    });
}

} // namespace Hideo::Spreadsheet

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Spreadsheet::app());
}
