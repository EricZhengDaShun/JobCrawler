#include "WebDownloader.hpp"


WebDownloader::WebDownloader(QObject* parent)
    : QObject(parent)
{
    QObject::connect(&page, SIGNAL(loadFinished(bool)), this, SLOT(downloadFinished(bool)));
}

WebDownloader::~WebDownloader()
{

}

void WebDownloader::load(const std::wstring& url)
{
    page.load(QUrl(QString::fromStdWString(url)));
    return;
}

void WebDownloader::downloadFinished(bool isOk)
{
    if (!isOk) {
        emit downloadHTMLFinished(*this, page.url(), QString(""));
        return;
    }

    page.toHtml([&](QString html) {
        emit downloadHTMLFinished(*this, page.url(), html);
    });

    return;
}