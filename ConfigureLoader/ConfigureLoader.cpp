#include "ConfigureLoader.hpp"

using namespace JobCrawler;

#include "nlohmann/json.hpp"
#include "FileHelper.hpp"
#include "Convert.hpp"


namespace {
    using namespace nlohmann;

    json findNode(const json& rootJson, const std::string& nodeName) 
    {
        if (rootJson.find(nodeName) == rootJson.end()) {
            ConfigureException configureException(nodeName + " setting not found !");
            throw configureException;
        }
        json node = rootJson[nodeName];
        if (!node.is_object()) {
            ConfigureException configureException(nodeName + " is not object !");
            throw configureException;
        }
        return node;
    }

    std::wstring getString(const json& rootJson, const std::string& rootName, const std::string& name)
    {
        if (rootJson.find(name) == rootJson.end()) {
            ConfigureException configureException(rootName + "." + name + " setting not found !");
            throw configureException;
        }
        json stringData = rootJson[name];
        if (!stringData.is_string()) {
            ConfigureException configureException(rootName + "." + name + " is not string !");
            throw configureException;
        }

        const std::string str = stringData.get<std::string>();
        return Convert::utf8ToWchar(str);
    }

    size_t getNumberSizet(const json& rootJson, const std::string& rootName, const std::string& name)
    {
        if (rootJson.find(name) == rootJson.end()) {
            ConfigureException configureException(rootName + "." + name + " setting not found !");
            throw configureException;
        }
        json nemberData = rootJson[name];
        if (!nemberData.is_number()) {
            ConfigureException configureException(rootName + "." + name + " is not number !");
            throw configureException;
        }

        return nemberData.get<size_t>();
    }

    bool getBooleam(const json& rootJson, const std::string& rootName, const std::string& name)
    {
        if (rootJson.find(name) == rootJson.end()) {
            ConfigureException configureException(rootName + "." + name + " setting not found !");
            throw configureException;
        }
        json booleamData = rootJson[name];
        if (!booleamData.is_boolean()) {
            ConfigureException configureException(rootName + "." + name + " is not boolean !");
            throw configureException;
        }

        return booleamData.get<bool>();
    }

    std::vector<std::wstring> getVectorString(const json& rootJson, const std::string& rootName, const std::string& name)
    {
        if (rootJson.find(name) == rootJson.end()) {
            ConfigureException configureException(rootName + "." + name + " setting not found !");
            throw configureException;
        }
        json stringVector = rootJson[name];
        if (!stringVector.is_array()) {
            ConfigureException configureException(rootName + "." + name + " is not array !");
            throw configureException;
        }
        
        if (!stringVector.empty() && !stringVector.front().is_string()) {
            ConfigureException configureException(rootName + "." + name + " array is not string type !");
            throw configureException;
        }

        std::vector<std::wstring> result;
        for (const auto& v : stringVector) {
            result.push_back(Convert::utf8ToWchar(v.get<std::string>()));
        }

        return result;
    }


    void parseUrl(const json& rootJson, Setting::Url& url)
    {
        json urlNode = ::findNode(rootJson, "url");
        url.url = ::getString(urlNode, "url", "path");
        url.pageNum = ::getNumberSizet(urlNode, "url", "pageNum");

        return;
    }

    void parseFilterSwitch(const json& rootJson, Setting::FilterSwitch& filterSwitch) 
    {
        json filterSwitchNode = ::findNode(rootJson, "filterSwitch");
        filterSwitch.toolEnable = ::getBooleam(filterSwitchNode, "filterSwitch", "toolEnable");
        filterSwitch.jobTitleEnable = ::getBooleam(filterSwitchNode, "filterSwitch", "jobTitleEnable");
        filterSwitch.jobContentEnable = ::getBooleam(filterSwitchNode, "filterSwitch", "jobContentEnable");

        return;
    }

    void parseDataSetting(const json& rootJson, Setting::DataSetting& dataSetting)
    {
        json dataSettingNode = ::findNode(rootJson, "dataSetting");
        dataSetting.useFile = ::getBooleam(dataSettingNode, "dataSetting", "useFile");
        dataSetting.saveData = ::getBooleam(dataSettingNode, "dataSetting", "saveData");
        dataSetting.fileName = ::getString(dataSettingNode, "url", "fileName");

        return;
    }

    void parseTool(const json& rootJson, Setting::Tool& tool)
    {
        json toolNode = ::findNode(rootJson, "tool");
        tool.include = ::getVectorString(toolNode, "tool", "include");
        tool.exclude = ::getVectorString(toolNode, "tool", "exclude");

        return;
    }

    void parseTitle(const json& rootJson, Setting::Title& title)
    {
        json toolNode = ::findNode(rootJson, "title");
        title.include = ::getVectorString(toolNode, "title", "include");
        title.exclude = ::getVectorString(toolNode, "title", "exclude");

        return;
    }

    void parseJobContent(const json& rootJson, Setting::JobContent& jobContent)
    {
        json toolNode = ::findNode(rootJson, "jobContent");
        jobContent.include = ::getVectorString(toolNode, "jobContent", "include");
        jobContent.exclude = ::getVectorString(toolNode, "jobContent", "exclude");

        return;
    }

    void parseJobLinkHTML(const json& rootJson, HTMLTagContent& jobLinkHTML) {
        json htmlTagNode = ::findNode(rootJson, "htmlTag");
        json jobLinkNode = ::findNode(htmlTagNode, "jobLink");
        jobLinkHTML.tagType = ::getString(jobLinkNode, "htmlTag.jobLink", "tagType");
        jobLinkHTML.attributeName = ::getString(jobLinkNode, "htmlTag.jobLink", "attributeName");
        jobLinkHTML.attributeValue = ::getString(jobLinkNode, "htmlTag.jobLink", "attributeValue");
        jobLinkHTML.contentAttribute = ::getString(jobLinkNode, "htmlTag.jobLink", "contentAttribute");

        return;
    }

    void parseToolHTML(const json& rootJson, HTMLTag& toolHTML) {
        json htmlTagNode = ::findNode(rootJson, "htmlTag");
        json toolNode = ::findNode(htmlTagNode, "tool");
        toolHTML.tagType = ::getString(toolNode, "htmlTag.tool", "tagType");
        toolHTML.attributeName = ::getString(toolNode, "htmlTag.tool", "attributeName");
        toolHTML.attributeValue = ::getString(toolNode, "htmlTag.tool", "attributeValue");

        return;
    }

    void parseJobTitleHTML(const json& rootJson, HTMLTagContent& jobTitle) {
        json htmlTagNode = ::findNode(rootJson, "htmlTag");
        json jobTitleNode = ::findNode(htmlTagNode, "jobTitle");
        jobTitle.tagType = ::getString(jobTitleNode, "htmlTag.jobTitle", "tagType");
        jobTitle.attributeName = ::getString(jobTitleNode, "htmlTag.jobTitle", "attributeName");
        jobTitle.attributeValue = ::getString(jobTitleNode, "htmlTag.jobTitle", "attributeValue");
        jobTitle.contentAttribute = ::getString(jobTitleNode, "htmlTag.jobTitle", "contentAttribute");

        return;
    }

    void parseSalaryHTML(const json& rootJson, HTMLTag& salaryHTML) {
        json htmlTagNode = ::findNode(rootJson, "htmlTag");
        json salaryNode = ::findNode(htmlTagNode, "salary");
        salaryHTML.tagType = ::getString(salaryNode, "htmlTag.salary", "tagType");
        salaryHTML.attributeName = ::getString(salaryNode, "htmlTag.salary", "attributeName");
        salaryHTML.attributeValue = ::getString(salaryNode, "htmlTag.salary", "attributeValue");

        return;
    }

    void parseJobContentHTML(const json& rootJson, HTMLTag& jobContentHTML) {
        json htmlTagNode = ::findNode(rootJson, "htmlTag");
        json jobContentNode = ::findNode(htmlTagNode, "jobContent");
        jobContentHTML.tagType = ::getString(jobContentNode, "htmlTag.jobContent", "tagType");
        jobContentHTML.attributeName = ::getString(jobContentNode, "htmlTag.jobContent", "attributeName");
        jobContentHTML.attributeValue = ::getString(jobContentNode, "htmlTag.jobContent", "attributeValue");

        return;
    }
}

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
    ::parseUrl(rootJson, url);
    ::parseFilterSwitch(rootJson, filterSwitch);
    ::parseDataSetting(rootJson, dataSetting);
    ::parseTool(rootJson, tool);
    ::parseTitle(rootJson, title);
    ::parseJobContent(rootJson, jobContent);
    ::parseJobLinkHTML(rootJson, jobLinkHTML);
    ::parseToolHTML(rootJson, toolHTML);
    ::parseJobTitleHTML(rootJson, jobTitleHTML);
    ::parseSalaryHTML(rootJson, salaryHTML);
    ::parseJobContentHTML(rootJson, jobContentHTML);
    return;
}