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

        struct JobTitle
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

        void save();

        Setting::Url getSettingUrl() const;
        void setSettingUrl(const Setting::Url& url);
        Setting::FilterSwitch getSettingFilterSwitch() const;
        void setSettingFilterSwitch(const Setting::FilterSwitch& filterSwitch);
        Setting::DataSetting getSettingDataSetting() const;
        void setSettingDataSetting(const Setting::DataSetting& dataSetting);
        Setting::Tool getSettingTool() const;
        void setSettingTool(const Setting::Tool& tool);
        Setting::JobTitle getSettingJobTitle() const;
        void setSettingJobTitle(const Setting::JobTitle& JobTitle);
        Setting::JobContent getSettingJobContent() const;
        void setSettingJobContent(const Setting::JobContent& jobContent);

        HTMLTagContent getJobLinkHTML() const;
        void setJobLinkHTML(const HTMLTagContent& jobLinkHTML);
        HTMLTag getToolHTML() const;
        void setToolHTML(const HTMLTag& toolHTML);
        HTMLTagContent getJobTitleHTML() const;
        void setJobTitleHTML(const HTMLTagContent& jobTitleHTML);
        HTMLTag getSalaryHTML() const;
        void setSalaryHTML(const HTMLTag& salaryHTML);
        HTMLTag getJobContentHTML() const;
        void setJobContentHTML(const HTMLTag& jobContentHTML);

    private:
        std::wstring configureFileName;

        Setting::Url url;
        Setting::FilterSwitch filterSwitch;
        Setting::DataSetting dataSetting;
        Setting::Tool tool;
        Setting::JobTitle jobTitle;
        Setting::JobContent jobContent;

        HTMLTagContent jobLinkHTML;
        HTMLTag toolHTML;
        HTMLTagContent jobTitleHTML;
        HTMLTag salaryHTML;
        HTMLTag jobContentHTML;
    };

}



