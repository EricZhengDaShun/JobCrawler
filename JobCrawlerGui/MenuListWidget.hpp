#pragma once

#include <qobject.h>
#include <qwidget.h>
#include <qlistwidget.h>
#include <qevent.h>
#include <qmenu.h>
#include <qaction.h>

class MenuListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit MenuListWidget(QWidget* parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

protected slots:
    void deleteCurrentItem();

private:
    QMenu* itemMenu;
    QAction* deleteItemAction;
};

