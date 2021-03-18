#pragma once

#include <string>
#include <vector>

namespace JobCrawler {

    struct UrlSetting
    {
        std::wstring url;
        size_t page;
    };

    struct ToolSetting
    {
        std::vector<std::wstring> include;
        std::vector<std::wstring> exclude;
    };

    struct JobLinkTag
    {
        std::wstring tagType;
        std::wstring attributeName;
        std::wstring attributeValue;
        std::wstring contentAttribute;
    };

    struct ToolTag
    {
        std::wstring tagType;
        std::wstring attributeName;
        std::wstring attributeValue;

    };

    struct JobTitle
    {
        std::wstring tagType;
        std::wstring attributeName;
        std::wstring attributeValue;
    };

    struct Salary
    {
        std::wstring tagType;
        std::wstring attributeName;
        std::wstring attributeValue;
    };

    struct HtmlTag
    {
        JobLinkTag jobLinkTag;
        ToolTag toolTag;
        JobTitle jobTitle;
        Salary salary;
    };


    class ConfigureLoader final
    {
    public:
        ConfigureLoader();

        void setFileName(const std::wstring& fileName);
        std::wstring getFileName() const;

        void parse();

    private:
        std::wstring configureFileName;
    };

}



