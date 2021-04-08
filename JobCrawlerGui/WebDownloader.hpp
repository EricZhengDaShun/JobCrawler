#pragma once

#include <QObject>
#include <QWebEnginePage>
#include <qurl.h>
#include <qstring.h>

class WebDownloader : public QObject
{
    Q_OBJECT

public:
    explicit WebDownloader(QObject* parent = nullptr);
    ~WebDownloader() override;
    WebDownloader(const WebDownloader&) = delete;
    WebDownloader& operator=(const WebDownloader&) = delete;

    void load(const std::wstring& url);

signals:
    void downloadHTMLFinished(WebDownloader& webDownloader, QUrl url, QString html);

private slots:
    void downloadFinished(bool isOk);

private:
    QWebEnginePage page;
};

