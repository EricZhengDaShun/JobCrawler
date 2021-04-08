#include <iostream>
#include <locale>
#include <vector>
#include <memory>
#include <thread>

#include "JobCrawlerGui.hpp"
#include <QtWidgets/QApplication>
#include "WebDownloader.hpp"

int main(int argc, char *argv[])
{
    std::locale::global(std::locale("zh_TW.UTF-8"));
    std::ios::sync_with_stdio(false);

    QApplication a(argc, argv);

    static const int htmlCrawlerNum = std::thread::hardware_concurrency();
    std::vector<std::unique_ptr<WebDownloader>> webDownloaders;
    for (int count = 0; count < htmlCrawlerNum; ++count) {
        webDownloaders.push_back(std::make_unique<WebDownloader>());
    }

    JobCrawlerGui w(webDownloaders);
    w.show();
    return a.exec();
}
