#pragma once

#include <QObject>
#include <qurl.h>
#include <qstring.h>

#include "WebDownloader.hpp"

class Receiver : public QObject
{
    Q_OBJECT

public:
    explicit Receiver(QObject* parent = nullptr);

protected slots:
    void downloadHTMLFinished(WebDownloader& webDownloader, QUrl url, QString html);
};

