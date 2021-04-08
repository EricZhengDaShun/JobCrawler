#include <iostream>
#include <locale>

#include <QtWidgets/QApplication>
#include <QObject>

#include "WebDownloader.hpp"
#include "Receiver.hpp"

int main(int argc, char *argv[])
{
    std::locale::global(std::locale("zh_TW.UTF-8"));
    std::ios::sync_with_stdio(false);

    QApplication a(argc, argv);

    WebDownloader webDownloader;
    Receiver receiver;
    QObject::connect(&webDownloader, SIGNAL(downloadHTMLFinished(WebDownloader&, QUrl, QString))
    , &receiver, SLOT(downloadHTMLFinished(WebDownloader&, QUrl, QString)));

    webDownloader.load(L"https://www.104.com.tw/jobs/main/");
    return a.exec();
}
