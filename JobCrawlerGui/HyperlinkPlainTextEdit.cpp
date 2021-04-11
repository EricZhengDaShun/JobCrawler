#include "HyperlinkPlainTextEdit.hpp"
#include <qdesktopservices.h>

HyperlinkPlainTextEdit::HyperlinkPlainTextEdit(QWidget* parent) 
    : QPlainTextEdit(parent)
{

}

void HyperlinkPlainTextEdit::mouseDoubleClickEvent(QMouseEvent* mouseEvent)
{
    QString link = (mouseEvent->button() & Qt::LeftButton) ? anchorAt(mouseEvent->pos()) :
        QString();
    if (!link.isEmpty()) {
        QDesktopServices::openUrl(QUrl(link));
        return;
    }
    QPlainTextEdit::mousePressEvent(mouseEvent);

    return;
}

