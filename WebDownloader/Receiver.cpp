#include "Receiver.hpp"

#include <iostream>

Receiver::Receiver(QObject* parent)
    : QObject(parent)
{

}


void Receiver::downloadHTMLFinished(WebDownloader& webDownloader, QUrl url, QString html)
{
    std::wcout << L"===========HTML=================" << std::endl;
    std::wcout << url.toString().toStdWString() << std::endl;
    std::wcout << html.toStdWString() << std::endl;
    std::wcout.flush();
    return;
}