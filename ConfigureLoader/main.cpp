#include <iostream>
#include <string>
#include <locale>
#include <exception>
#include <vector>

#include "FileHelper.hpp"

#include "ConfigureLoader.hpp"

int main()
{
    std::locale::global(std::locale("zh_TW.UTF-8"));
    std::ios::sync_with_stdio(false);
    try {
        using namespace JobCrawler;

        const std::wstring configureFileName{ L"setting.json" };

        ConfigureLoader configureLoader;
        configureLoader.setFileName(configureFileName);
        configureLoader.parse();

        std::cin.ignore();
        return 0;

    } catch (std::exception& e) {
        std::cout << "exception:" << std::endl;
        std::cout << e.what() << std::endl;
        std::cin.ignore();
        return 0;
    }
}