#include "MenuListWidget.hpp"

#include <qcursor.h>

MenuListWidget::MenuListWidget(QWidget* parent)
    : QListWidget(parent)
{
    itemMenu = new QMenu(this);
    deleteItemAction = new QAction(tr("Delete"), this);
    QObject::connect(deleteItemAction, SIGNAL(triggered(bool)), this, SLOT(deleteCurrentItem()));
    itemMenu->addAction(deleteItemAction);
}

void MenuListWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if (this->itemAt(mapFromGlobal(QCursor::pos())) != nullptr) {
        itemMenu->exec(QCursor::pos());
    }

    return;
}

void MenuListWidget::deleteCurrentItem()
{
    this->model()->removeRow(row(currentItem()));
    return;
}