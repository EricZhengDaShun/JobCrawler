#include "ConfigureLoader.hpp"

using namespace JobCrawler;

#include "nlohmann/json.hpp"
#include "FileHelper.hpp"

ConfigureLoader::ConfigureLoader()
{

}

void ConfigureLoader::setFileName(const std::wstring& fileName)
{
    configureFileName = fileName;
    return;
}

std::wstring ConfigureLoader::getFileName() const
{
    return configureFileName;
}

void ConfigureLoader::parse()
{
    const std::string configureRawData = FileHelper::readAll(configureFileName);

    using namespace nlohmann;
    json rootJson = json::parse(configureRawData);


    return;
}