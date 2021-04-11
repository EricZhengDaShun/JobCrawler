#pragma once

#include <qobject.h>
#include <qplaintextedit.h>

class HyperlinkPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit HyperlinkPlainTextEdit(QWidget* parent = nullptr);
    void mouseDoubleClickEvent(QMouseEvent* mouseEvent) override;
};

