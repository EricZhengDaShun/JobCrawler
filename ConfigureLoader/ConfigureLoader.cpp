#include "ConfigureLoader.hpp"
#include <algorithm>

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

    void parseJobTitle(const json& rootJson, Setting::JobTitle& jobTitle)
    {
        json toolNode = ::findNode(rootJson, "jobTitle");
        jobTitle.include = ::getVectorString(toolNode, "jobTitle", "include");
        jobTitle.exclude = ::getVectorString(toolNode, "jobTitle", "exclude");

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

    void parseJobTitleHTML(const json& rootJson, HTMLTagContent& jobTitleHTML) {
        json htmlTagNode = ::findNode(rootJson, "htmlTag");
        json jobTitleNode = ::findNode(htmlTagNode, "jobTitle");
        jobTitleHTML.tagType = ::getString(jobTitleNode, "htmlTag.jobTitle", "tagType");
        jobTitleHTML.attributeName = ::getString(jobTitleNode, "htmlTag.jobTitle", "attributeName");
        jobTitleHTML.attributeValue = ::getString(jobTitleNode, "htmlTag.jobTitle", "attributeValue");
        jobTitleHTML.contentAttribute = ::getString(jobTitleNode, "htmlTag.jobTitle", "contentAttribute");

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

    std::vector<std::string> vectorTransformWstrToStr(const std::vector<std::wstring>& source)
    {
        std::vector<std::string> direct;
        std::transform(source.begin(), source.end(),
            std::back_inserter(direct),
            [](const std::wstring& wstr) { return Convert::wcharToUtf8(wstr); });
        return direct;
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

    json rootJson = json::parse(configureRawData);
    ::parseUrl(rootJson, url);
    ::parseFilterSwitch(rootJson, filterSwitch);
    ::parseDataSetting(rootJson, dataSetting);
    ::parseTool(rootJson, tool);
    ::parseJobTitle(rootJson, jobTitle);
    ::parseJobContent(rootJson, jobContent);
    ::parseJobLinkHTML(rootJson, jobLinkHTML);
    ::parseToolHTML(rootJson, toolHTML);
    ::parseJobTitleHTML(rootJson, jobTitleHTML);
    ::parseSalaryHTML(rootJson, salaryHTML);
    ::parseJobContentHTML(rootJson, jobContentHTML);
    return;
}

void ConfigureLoader::save()
{
    json rootJson;
    rootJson["url"]["path"] = Convert::wcharToUtf8(url.url);
    rootJson["url"]["pageNum"] = url.pageNum;

    rootJson["filterSwitch"]["toolEnable"] = filterSwitch.toolEnable;
    rootJson["filterSwitch"]["jobTitleEnable"] = filterSwitch.jobTitleEnable;
    rootJson["filterSwitch"]["jobContentEnable"] = filterSwitch.jobContentEnable;

    rootJson["dataSetting"]["useFile"] = dataSetting.useFile;
    rootJson["dataSetting"]["saveData"] = dataSetting.saveData;
    rootJson["dataSetting"]["fileName"] = Convert::wcharToUtf8(dataSetting.fileName);

    rootJson["tool"]["include"] = ::vectorTransformWstrToStr(tool.include);
    rootJson["tool"]["exclude"] = ::vectorTransformWstrToStr(tool.exclude);

    rootJson["jobTitle"]["include"] = ::vectorTransformWstrToStr(jobTitle.include);
    rootJson["jobTitle"]["exclude"] = ::vectorTransformWstrToStr(jobTitle.exclude);

    rootJson["jobContent"]["include"] = ::vectorTransformWstrToStr(jobContent.include);
    rootJson["jobContent"]["exclude"] = ::vectorTransformWstrToStr(jobContent.exclude);

    rootJson["htmlTag"]["jobLink"]["tagType"] = Convert::wcharToUtf8(jobLinkHTML.tagType);
    rootJson["htmlTag"]["jobLink"]["attributeName"] = Convert::wcharToUtf8(jobLinkHTML.attributeName);
    rootJson["htmlTag"]["jobLink"]["attributeValue"] = Convert::wcharToUtf8(jobLinkHTML.attributeValue);
    rootJson["htmlTag"]["jobLink"]["contentAttribute"] = Convert::wcharToUtf8(jobLinkHTML.contentAttribute);

    rootJson["htmlTag"]["tool"]["tagType"] = Convert::wcharToUtf8(toolHTML.tagType);
    rootJson["htmlTag"]["tool"]["attributeName"] = Convert::wcharToUtf8(toolHTML.attributeName);
    rootJson["htmlTag"]["tool"]["attributeValue"] = Convert::wcharToUtf8(toolHTML.attributeValue);

    rootJson["htmlTag"]["jobTitle"]["tagType"] = Convert::wcharToUtf8(jobTitleHTML.tagType);
    rootJson["htmlTag"]["jobTitle"]["attributeName"] = Convert::wcharToUtf8(jobTitleHTML.attributeName);
    rootJson["htmlTag"]["jobTitle"]["attributeValue"] = Convert::wcharToUtf8(jobTitleHTML.attributeValue);
    rootJson["htmlTag"]["jobTitle"]["contentAttribute"] = Convert::wcharToUtf8(jobTitleHTML.contentAttribute);

    rootJson["htmlTag"]["salary"]["tagType"] = Convert::wcharToUtf8(salaryHTML.tagType);
    rootJson["htmlTag"]["salary"]["attributeName"] = Convert::wcharToUtf8(salaryHTML.attributeName);
    rootJson["htmlTag"]["salary"]["attributeValue"] = Convert::wcharToUtf8(salaryHTML.attributeValue);

    rootJson["htmlTag"]["jobContent"]["tagType"] = Convert::wcharToUtf8(jobContentHTML.tagType);
    rootJson["htmlTag"]["jobContent"]["attributeName"] = Convert::wcharToUtf8(jobContentHTML.attributeName);
    rootJson["htmlTag"]["jobContent"]["attributeValue"] = Convert::wcharToUtf8(jobContentHTML.attributeValue);

    const std::string jsonData = rootJson.dump();
    FileHelper::overwrite(configureFileName, jsonData);
    
    return;
}

Setting::Url ConfigureLoader::getSettingUrl() const
{
    return url;
}

void ConfigureLoader::setSettingUrl(const Setting::Url& url)
{
    this->url = url;
    return;
}

Setting::FilterSwitch ConfigureLoader::getSettingFilterSwitch() const
{
    return filterSwitch;
}

void ConfigureLoader::setSettingFilterSwitch(const Setting::FilterSwitch& filterSwitch)
{
    this->filterSwitch = filterSwitch;
    return;
}

Setting::DataSetting ConfigureLoader::getSettingDataSetting() const
{
    return dataSetting;
}

void ConfigureLoader::setSettingDataSetting(const Setting::DataSetting& dataSetting)
{
    this->dataSetting = dataSetting;
    return;
}

Setting::Tool ConfigureLoader::getSettingTool() const
{
    return tool;
}

void ConfigureLoader::setSettingTool(const Setting::Tool& tool)
{
    this->tool = tool;
    return;
}

Setting::JobTitle ConfigureLoader::getSettingJobTitle() const
{
    return jobTitle;
}

void ConfigureLoader::setSettingJobTitle(const Setting::JobTitle& jobTitle)
{
    this->jobTitle = jobTitle;
    return;
}

Setting::JobContent ConfigureLoader::getSettingJobContent() const
{
    return jobContent;
}

void ConfigureLoader::setSettingJobContent(const Setting::JobContent& jobContent)
{
    this->jobContent = jobContent;
    return;
}

HTMLTagContent ConfigureLoader::getJobLinkHTML() const
{
    return jobLinkHTML;
}

void ConfigureLoader::setJobLinkHTML(const HTMLTagContent& jobLinkHTML)
{
    this->jobLinkHTML = jobLinkHTML;
    return;
}

HTMLTag ConfigureLoader::getToolHTML() const
{
    return toolHTML;
}

void ConfigureLoader::setToolHTML(const HTMLTag& toolHTML)
{
    this->toolHTML = toolHTML;
    return;
}

HTMLTagContent ConfigureLoader::getJobTitleHTML() const
{
    return jobTitleHTML;
}

void ConfigureLoader::setJobTitleHTML(const HTMLTagContent& jobTitleHTML)
{
    this->jobTitleHTML = jobTitleHTML;
    return;
}

HTMLTag ConfigureLoader::getSalaryHTML() const
{
    return salaryHTML;
}

void ConfigureLoader::setSalaryHTML(const HTMLTag& salaryHTML)
{
    this->salaryHTML = salaryHTML;
    return;
}

HTMLTag ConfigureLoader::getJobContentHTML() const
{
    return jobContentHTML;
}

void ConfigureLoader::setJobContentHTML(const HTMLTag& jobContentHTML)
{
    this->jobContentHTML = jobContentHTML;
    return;
}

