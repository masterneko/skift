#pragma once

#include <libio/Path.h>
#include <libutils/Vector.h>
#include <libwidget/model/TableModel.h>

#include <libfilepicker/model/DirectoryInfo.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

class DirectoryListing : public Widget::TableModel
{
private:
    RefPtr<Navigation> _navigation;
    Vector<DirectoryInfo> _directories{};
    OwnPtr<Async::Observer<Navigation>> _observer;

public:
    DirectoryListing(RefPtr<Navigation> navigation);

    int rows() override;

    int columns() override;

    String header(int column) override;

    Widget::Variant data(int row, int column) override;

    void update() override;

    const DirectoryInfo &info(int index) const;
};

} // namespace FilePicker