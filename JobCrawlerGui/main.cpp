#include <iostream>
#include <locale>

#include "JobCrawlerGui.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    std::locale::global(std::locale("zh_TW.UTF-8"));
    std::ios::sync_with_stdio(false);

    QApplication a(argc, argv);
    JobCrawlerGui w;
    w.show();
    return a.exec();
}
