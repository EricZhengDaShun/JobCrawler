#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

#include <QtWidgets/QMainWindow>
#include "ui_JobCrawlerGui.h"
#include "WebDownloader.hpp"
#include "TimeMeasurer.hpp"
#include "ConfigureLoader.hpp"

class JobCrawlerGui : public QMainWindow
{
    Q_OBJECT

public:
    JobCrawlerGui(std::vector<std::unique_ptr<WebDownloader>>& webDownloaders, QWidget *parent = Q_NULLPTR);

signals:
    void downloadPageFinshed();
    void startDownloadPage();
    void filterDone();

protected slots:
    void on_configureReloadPushButton_clicked();
    void on_configureSavePushButton_clicked();

    void on_toolFilterIncludeAddPushButton_clicked();
    void on_toolFilterExcludeAddPushButton_clicked();

    void on_jobTitleFilterIncludeAddPushButton_clicked();
    void on_jobTitleFilterExcludeAddPushButton_clicked();

    void on_jobContentFilterIncludeAddPushButton_clicked();
    void on_jobContentFilterExcludeAddPushButton_clicked();

    void on_webDownloadStartPushButton_clicked();

    void on_filterPushButton_clicked();

    void enableWebDownloadTab();
    void downloadHTMLDone(WebDownloader& webDownloader, QUrl url, QString html);
    void downloadPage();
    void showJobResult();

private:
    Ui::JobCrawlerGuiClass ui;

private:
    void downloadAllJobItemPage(const QStringList urls);
    void saveTagHTML();
    void saveSetting();
    void filterJob();

private:
    struct WebData
    {
        QUrl url;
        std::string html;

        WebData(const QUrl& url, const std::string& html)
            : url(url)
            , html(html)
        {

        }
    };

    struct JobData
    {
        std::vector<std::string> tools;
        std::string jobTitle;
        std::string jobContent;
        std::string salary;
        std::string url;
        std::string companyName;
    };


private:
    std::shared_ptr<JobCrawler::ConfigureLoader> configureLoader;
    std::vector<std::unique_ptr<WebDownloader>>& webDownloaders;
    TimeMeasurer timeMeasurer;

    std::vector<QString> jobItemUrls;
    std::mutex jobItemUrlsMutex;
    std::vector<WebData> webDatas;
    std::mutex webDataMutex;

    size_t downloadStep;

    std::vector<QString> jobItemWebError;
    std::mutex jobItemWebErrorMutex;
    std::vector<WebData> jobItemWebs;

    std::vector<QString> jobDescriptionUrls;
    std::mutex jobDescriptionUrlsMutex;

    std::vector<QString> jobDescriptionError;
    std::mutex jobDescriptionErrorMutex;
    std::vector<WebData> jobDescriptionWebs;

    std::atomic<bool> isWebDownloadDone;

    JobCrawler::HTMLTagContent jobLinkHTML;
    JobCrawler::HTMLTagContent jobTitleHTML;
    JobCrawler::HTMLTag toolHTML;
    JobCrawler::HTMLTag jobContentHTML;
    JobCrawler::HTMLTag salaryHTML;
    JobCrawler::HTMLTag companyNameHTML;

    JobCrawler::Setting::DataSetting dataSetting;
    JobCrawler::Setting::Tool toolSetting;
    JobCrawler::Setting::JobTitle jobTitleSetting;
    JobCrawler::Setting::JobContent jobContentSetting;
    JobCrawler::Setting::FilterSwitch filterSwitchSetting;

    std::vector<JobData> jobDatas;
};
