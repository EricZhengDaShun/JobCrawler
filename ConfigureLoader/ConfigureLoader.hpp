#pragma once

#include <string>
#include <vector>
#include <exception>

namespace JobCrawler {

    namespace Setting {
        struct Url
        {
            std::wstring url;
            size_t pageNum;

            Url()
                : pageNum(0) {

            }
        };

        struct FilterSwitch
        {
            bool toolEnable;
            bool jobTitleEnable;
            bool jobContentEnable;

            FilterSwitch()
                : toolEnable(false)
                , jobTitleEnable(false)
                , jobContentEnable(false) {

            }
        };

        struct DataSetting
        {
            bool useFile;
            bool saveData;
            std::wstring fileName;

            DataSetting()
                : useFile(false)
                , saveData(false) {

            }
        };

        struct Tool
        {
            std::vector<std::wstring> include;
            std::vector<std::wstring> exclude;
        };

        struct Title
        {
            std::vector<std::wstring> include;
            std::vector<std::wstring> exclude;
        };

        struct JobContent
        {
            std::vector<std::wstring> include;
            std::vector<std::wstring> exclude;
        };
    }

    struct HTMLTag
    {
        std::wstring tagType;
        std::wstring attributeName;
        std::wstring attributeValue;
    };

    struct HTMLTagContent : public HTMLTag
    {
        std::wstring contentAttribute;
    };

    class ConfigureException : public std::exception
    {
    public:
        explicit ConfigureException(const std::string& msg)
            : message(msg) {

        }

        const char* what() const throw()
        {
            return message.c_str();
        }

    private:
        std::string message;
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
        Setting::Url url;
        Setting::FilterSwitch filterSwitch;
        Setting::DataSetting dataSetting;
        Setting::Tool tool;
        Setting::Title title;
        Setting::JobContent jobContent;
        HTMLTagContent jobLinkHTML;
        HTMLTag toolHTML;
        HTMLTagContent jobTitleHTML;
        HTMLTag salaryHTML;
        HTMLTag jobContentHTML;
    };

}



