#include "JobCrawlerGui.hpp"

#include <exception>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cwctype>
#include <sstream> 

#include <qstring.h>
#include <qtranslator.h>
#include <qstringlist.h>
#include <QtConcurrent/qtconcurrentrun.h>

#include "nlohmann/json.hpp"

#include "HTMLParser.hpp"
#include "Convert.hpp"
#include "FileHelper.hpp"

namespace {
    QStringList makeBaseUrlPathToUrlPaths(const QString baseUrl, const int pageNum)
    {
        QStringList urls;

        const QString headerTail{"&page="};
        const int headerTailPos = baseUrl.indexOf(headerTail);
        if (headerTailPos == -1) return urls;
        const QString header = baseUrl.mid(0, headerTailPos + headerTail.length());
        
        const QString tailHeader{ "&mode" };
        const int tailHeaderPos = baseUrl.indexOf(tailHeader);
        if (tailHeaderPos == -1) return urls;
        const QString tail = baseUrl.mid(tailHeaderPos);

        for (int count = 1; count <= pageNum; ++count) {
            urls.push_back(header + QString::number(count) + tail);
        }

        return urls;
    }

    std::vector<std::string> vectorTransformWstrToStr(const std::vector<std::wstring>& source)
    {
        std::vector<std::string> direct;
        std::transform(source.begin(), source.end(),
            std::back_inserter(direct),
            [](const std::wstring& wstr) { return Convert::wcharToUtf8(wstr); });
        return direct;
    }

    std::wstring WstrToUpper(const std::wstring& wstr)
    {
        std::wstringstream wss;
        for (const auto& wchar : wstr) {
            wss << std::towupper(wchar);
        }
        return wss.str();
    }

    bool isFound(const std::vector<std::string>& datas, const std::wstring& key)
    {
        std::vector<std::wstring> direct;
        std::transform(datas.begin(), datas.end(),
            std::back_inserter(direct),
            [](const std::string& str) { return ::WstrToUpper(Convert::utf8ToWchar(str)); });

        const std::wstring upperKey = ::WstrToUpper(key);
        auto pos = std::find(direct.cbegin(), direct.cend(), upperKey);
        if (pos == direct.cend()) return false;
        return true;
    }

    bool filterData(const std::vector<std::wstring>& includes
        , const std::vector<std::wstring>& excludes
        , const std::vector<std::string>& data)
    {
        for (const auto& item : includes) {
            if (!isFound(data, item)) return true;
        }

        for (const auto& item : excludes) {
            if (isFound(data, item)) return true;
        }

        return false;
    }

    bool filterData(const std::vector<std::wstring>& includes
        , const std::vector<std::wstring>& excludes
        , const std::string& rawData)
    {
        const std::wstring data = ::WstrToUpper(Convert::utf8ToWchar(rawData));
        for (const auto& item : includes) {
            const std::wstring::size_type pos = data.find(::WstrToUpper(item));
            if (pos == std::wstring::npos) return true;
        }

        for (const auto& item : excludes) {
            const std::wstring::size_type pos = data.find(item);
            if (pos != std::wstring::npos) return true;
        }

        return false;
    }
}

using namespace JobCrawler;

JobCrawlerGui::JobCrawlerGui(std::vector<std::unique_ptr<WebDownloader>>& webDownloaders, QWidget *parent)
    : QMainWindow(parent)
    , configureLoader(std::make_shared<ConfigureLoader>())
    , webDownloaders(webDownloaders)
    , downloadStep(0)
    , isWebDownloadDone(false)
{
    ui.setupUi(this);
    QObject::connect(this, SIGNAL(downloadPageFinshed()),
        this, SLOT(enableWebDownloadTab()));

    for (auto& webDownloader : webDownloaders) {
        QObject::connect(webDownloader.get(), SIGNAL(downloadHTMLFinished(WebDownloader&, QUrl, QString)),
            this, SLOT(downloadHTMLDone(WebDownloader&, QUrl, QString)));
    }

    QObject::connect(this, SIGNAL(startDownloadPage()),
         this, SLOT(downloadPage()));

    QObject::connect(this, SIGNAL(clickWebDownloadStartPushButton()),
        this, SLOT(on_webDownloadStartPushButton_clicked()));
    
    QObject::connect(this, SIGNAL(filterDone()),
        this, SLOT(showJobResult()));
}

void JobCrawlerGui::on_configureReloadPushButton_clicked()
{
    const QString fileName = ui.configureFileNameLineEdit->text();
    configureLoader->setFileName(fileName.toStdWString());

    try {
        configureLoader->parse();
    } catch (std::exception& e) {
        ui.statusBar->showMessage(tr(e.what()));
        return;
    }
    
    const std::wstring url = configureLoader->getSettingUrl().url;
    const int pageNum = configureLoader->getSettingUrl().pageNum;
    ui.urlPathPlainTextEdit->setPlainText(QString::fromStdWString(url));
    ui.urlPageNumSpinBox->setValue(pageNum);

    const bool jobContentEnable = configureLoader->getSettingFilterSwitch().jobContentEnable;
    const bool jobTitleEnable = configureLoader->getSettingFilterSwitch().jobTitleEnable;
    const bool toolEnable = configureLoader->getSettingFilterSwitch().toolEnable;
    ui.filterSwitchJobContentCheckBox->setCheckState(jobContentEnable ? Qt::Checked : Qt::Unchecked);
    ui.filterSwitchJobTitleCheckBox->setCheckState(jobTitleEnable ? Qt::Checked : Qt::Unchecked);
    ui.filterSwitchToolCheckBox->setCheckState(toolEnable ? Qt::Checked : Qt::Unchecked);

    const bool useFile = configureLoader->getSettingDataSetting().useFile;
    const bool saveFile = configureLoader->getSettingDataSetting().saveData;
    const std::wstring dataFileName = configureLoader->getSettingDataSetting().fileName;
    ui.dataSettingUseFileCheckBox->setCheckState(useFile ? Qt::Checked : Qt::Unchecked);
    ui.dataSettingSaveFileCheckBox->setCheckState(saveFile ? Qt::Checked : Qt::Unchecked);
    ui.dataSettingFileNameLineEdit->setText(QString::fromStdWString(dataFileName));

    ui.toolFilterIncludeListWidget->clear();
    ui.toolFilterExcludeListWidget->clear();
    toolSetting.include = configureLoader->getSettingTool().include;
    toolSetting.exclude = configureLoader->getSettingTool().exclude;
    for (const auto& v : toolSetting.include) {
        ui.toolFilterIncludeListWidget->addItem(QString::fromStdWString(v));
    }
    for (const auto& v : toolSetting.exclude) {
        ui.toolFilterExcludeListWidget->addItem(QString::fromStdWString(v));
    }

    ui.jobTitleFilterIncludeListWidget->clear();
    ui.jobTitleFilterExcludeListWidget->clear();
    const std::vector<std::wstring> jobTitleIncludes = configureLoader->getSettingJobTitle().include;
    const std::vector<std::wstring> jobTitleExcludes = configureLoader->getSettingJobTitle().exclude;
    for (const auto& v : jobTitleIncludes) {
        ui.jobTitleFilterIncludeListWidget->addItem(QString::fromStdWString(v));
    }
    for (const auto& v : jobTitleExcludes) {
        ui.jobTitleFilterExcludeListWidget->addItem(QString::fromStdWString(v));
    }

    ui.jobContentFilterIncludeListWidget->clear();
    ui.jobContentFilterExcludeListWidget->clear();
    const std::vector<std::wstring> jobContentIncludes = configureLoader->getSettingJobContent().include;
    const std::vector<std::wstring> jobContentExcludes = configureLoader->getSettingJobContent().exclude;
    for (const auto& v : jobContentIncludes) {
        ui.jobContentFilterIncludeListWidget->addItem(QString::fromStdWString(v));
    }
    for (const auto& v : jobContentExcludes) {
        ui.jobContentFilterExcludeListWidget->addItem(QString::fromStdWString(v));
    }

    const std::wstring jobLinkTagType = configureLoader->getJobLinkHTML().tagType;
    const std::wstring jobLinkAttributeName = configureLoader->getJobLinkHTML().attributeName;
    const std::wstring jobLinkAttributeValue = configureLoader->getJobLinkHTML().attributeValue;
    const std::wstring jobLinkContentAttribute = configureLoader->getJobLinkHTML().contentAttribute;
    ui.jobLinkHTMLTagTypeLineEdit->setText(QString::fromStdWString(jobLinkTagType));
    ui.jobLinkHTMLAttributeNameLineEdit->setText(QString::fromStdWString(jobLinkAttributeName));
    ui.jobLinkHTMLAttributeValueLineEdit->setText(QString::fromStdWString(jobLinkAttributeValue));
    ui.jobLinkHTMLContentAttributeLineEdit->setText(QString::fromStdWString(jobLinkContentAttribute));

    const std::wstring jobTitleTagType = configureLoader->getJobTitleHTML().tagType;
    const std::wstring jobTitleAttributeName = configureLoader->getJobTitleHTML().attributeName;
    const std::wstring jobTitleAttributeValue = configureLoader->getJobTitleHTML().attributeValue;
    const std::wstring jobTitleContentAttribute = configureLoader->getJobTitleHTML().contentAttribute;
    ui.jobTitleHTMLTagTypeLineEdit->setText(QString::fromStdWString(jobTitleTagType));
    ui.jobTitleHTMLAttributeNameLineEdit->setText(QString::fromStdWString(jobTitleAttributeName));
    ui.jobTitleHTMLAttributeValueLineEdit->setText(QString::fromStdWString(jobTitleAttributeValue));
    ui.jobTitleHTMLContentAttributeLineEdit->setText(QString::fromStdWString(jobTitleContentAttribute));

    const std::wstring toolTagType = configureLoader->getToolHTML().tagType;
    const std::wstring toolAttributeName = configureLoader->getToolHTML().attributeName;
    const std::wstring toolAttributeValue = configureLoader->getToolHTML().attributeValue;
    ui.toolHTMLTagTypeLineEdit->setText(QString::fromStdWString(toolTagType));
    ui.toolHTMLAttributeNameLineEdit->setText(QString::fromStdWString(toolAttributeName));
    ui.toolHTMLAttributeValueLineEdit->setText(QString::fromStdWString(toolAttributeValue));

    const std::wstring jobContentTagType = configureLoader->getJobContentHTML().tagType;
    const std::wstring jobContentAttributeName = configureLoader->getJobContentHTML().attributeName;
    const std::wstring jobContentAttributeValue = configureLoader->getJobContentHTML().attributeValue;
    ui.jobContentHTMLTagTypeLineEdit->setText(QString::fromStdWString(jobContentTagType));
    ui.jobContentHTMLAttributeNameLineEdit->setText(QString::fromStdWString(jobContentAttributeName));
    ui.jobContentHTMLAttributeValueLineEdit->setText(QString::fromStdWString(jobContentAttributeValue));

    const std::wstring salaryTagType = configureLoader->getSalaryHTML().tagType;
    const std::wstring salaryAttributeName = configureLoader->getSalaryHTML().attributeName;
    const std::wstring salaryAttributeValue = configureLoader->getSalaryHTML().attributeValue;
    ui.salaryHTMLTagTypeLineEdit->setText(QString::fromStdWString(salaryTagType));
    ui.salaryHTMLAttributeNameLineEdit->setText(QString::fromStdWString(salaryAttributeName));
    ui.salaryHTMLAttributeValueLineEdit->setText(QString::fromStdWString(salaryAttributeValue));

    saveSetting();
    saveTagHTML();
    ui.statusBar->showMessage(tr("Reload configure file done !"));
    return;
}

void JobCrawlerGui::on_configureSavePushButton_clicked()
{
    const QString fileName = ui.configureFileNameLineEdit->text();
    configureLoader->setFileName(fileName.toStdWString());

    Setting::Url url;
    url.url = ui.urlPathPlainTextEdit->toPlainText().toStdWString();
    url.pageNum = static_cast<size_t>(ui.urlPageNumSpinBox->value());
    configureLoader->setSettingUrl(url);

    saveSetting();
    configureLoader->setSettingFilterSwitch(filterSwitchSetting);
    configureLoader->setSettingTool(toolSetting);
    configureLoader->setSettingJobContent(jobContentSetting);
    configureLoader->setSettingJobTitle(jobTitleSetting);

    saveTagHTML();
    configureLoader->setJobLinkHTML(jobLinkHTML);
    configureLoader->setJobTitleHTML(jobTitleHTML);
    configureLoader->setToolHTML(toolHTML);
    configureLoader->setJobContentHTML(jobContentHTML);
    configureLoader->setSalaryHTML(salaryHTML);

    configureLoader->save();
    ui.statusBar->showMessage(tr("Save configure file done !"));
    return;
}

void JobCrawlerGui::on_toolFilterIncludeAddPushButton_clicked()
{
    const QString toolItem = ui.toolFilterIncludeAddLineEdit->text();
    if (toolItem.isEmpty()) return;
    ui.toolFilterIncludeListWidget->addItem(toolItem);
    ui.toolFilterIncludeAddLineEdit->clear();

    return;
}

void JobCrawlerGui::on_toolFilterExcludeAddPushButton_clicked()
{
    const QString toolItem = ui.toolFilterExcludeAddLineEdit->text();
    if (toolItem.isEmpty()) return;
    ui.toolFilterExcludeListWidget->addItem(toolItem);
    ui.toolFilterExcludeAddLineEdit->clear();

    return;
}

void JobCrawlerGui::on_jobTitleFilterIncludeAddPushButton_clicked()
{
    const QString jobTitleItem = ui.jobTitleFilterIncludeAddLineEdit->text();
    if (jobTitleItem.isEmpty()) return;
    ui.jobTitleFilterIncludeListWidget->addItem(jobTitleItem);
    ui.jobTitleFilterIncludeAddLineEdit->clear();

    return;
}

void JobCrawlerGui::on_jobTitleFilterExcludeAddPushButton_clicked()
{
    const QString jobTitleItem = ui.jobTitleFilterExcludeAddLineEdit->text();
    if (jobTitleItem.isEmpty()) return;
    ui.jobTitleFilterExcludeListWidget->addItem(jobTitleItem);
    ui.jobTitleFilterExcludeAddLineEdit->clear();

    return;
}

void JobCrawlerGui::on_jobContentFilterIncludeAddPushButton_clicked()
{
    const QString jobContentItem = ui.jobContentFilterIncludeAddLineEdit->text();
    if (jobContentItem.isEmpty()) return;
    ui.jobContentFilterIncludeListWidget->addItem(jobContentItem);
    ui.jobContentFilterIncludeAddLineEdit->clear();

    return;
}

void JobCrawlerGui::on_jobContentFilterExcludeAddPushButton_clicked()
{
    const QString jobContentItem = ui.jobContentFilterExcludeAddLineEdit->text();
    if (jobContentItem.isEmpty()) return;
    ui.jobContentFilterExcludeListWidget->addItem(jobContentItem);
    ui.jobContentFilterExcludeAddLineEdit->clear();

    return;
}

void JobCrawlerGui::on_webDownloadStartPushButton_clicked()
{
    timeMeasurer.start();
    isWebDownloadDone.store(false);
    ui.webDownloadStartPushButton->setEnabled(false);

    ui.webDownloadJobLinkProgressBar->reset();
    ui.webDownloadJobDescriptionProgressBar->reset();

    const QString jobItemBaseUrl = ui.urlPathPlainTextEdit->toPlainText();
    const int pageNum = ui.urlPageNumSpinBox->value();
    const QStringList urls = ::makeBaseUrlPathToUrlPaths(jobItemBaseUrl, pageNum);
    downloadStep = 1;
    QtConcurrent::run(this, &JobCrawlerGui::downloadAllJobItemPage, urls);
}

void JobCrawlerGui::on_filterPushButton_clicked()
{
    ui.filterPushButton->setEnabled(false);
    ui.filterResultPlainTextEdit->clear();
    ui.filterResultPlainTextEdit->setEnabled(false);

    saveSetting();
    isWebDownloadDone.store(false);

    std::string jsonData;
    if (dataSetting.useFile) {
        jobDescriptionWebs.clear();

        try {
            jsonData = FileHelper::readAll(dataSetting.fileName);
            nlohmann::json json = nlohmann::json::parse(jsonData);
            if (json.find("data") == json.end()) {
                std::string msg = "HTML json file pase fail !";
                throw msg;
                return;
            }

            nlohmann::json data = json["data"];
            for (auto it = data.cbegin(); it != data.cend(); ++it) {
                nlohmann::json item = *it;
                if (item.find("url") == item.end() ||
                    item.find("html") == item.end()) {
                    std::string msg = "HTML json file pase fail !";
                    throw msg;
                    return;
                }

                WebData webdata(QUrl(QString::fromStdString(item["url"])), item["html"]);
                jobDescriptionWebs.push_back(webdata);
            }

            isWebDownloadDone.store(true);

        } catch (std::exception& e) {
            ui.statusBar->showMessage(tr(e.what()));
            return;
        
        } catch (std::string& str) {
            ui.statusBar->showMessage(tr(str.c_str()));
            return;
        }


    } else {
        ui.webDownloadStartPushButton->click();
    }

    QtConcurrent::run(this, &JobCrawlerGui::filterJob);
}

void JobCrawlerGui::enableWebDownloadTab()
{
    ui.webDownloadStartPushButton->setEnabled(true);
    timeMeasurer.stop();

    const QString msg = "Total time: " + QString::number(timeMeasurer.getDurationSec()) + " sec"
        + "  jobItemWebError: " + QString::number(jobItemWebError.size()) 
        + " , jobDescriptionError: " + QString::number(jobDescriptionError.size());
    ui.statusBar->showMessage(msg);
}

void JobCrawlerGui::downloadHTMLDone(WebDownloader& webDownloader, QUrl url, QString html)
{
    if(downloadStep == 1){
        std::lock_guard<std::mutex> lockGuard(jobItemUrlsMutex);
        if (!jobItemUrls.empty()) {
            webDownloader.load(jobItemUrls.back().toStdWString());
            jobItemUrls.pop_back();
        }
    } else if (downloadStep == 2) {
        std::lock_guard<std::mutex> lockGuard(jobDescriptionUrlsMutex);
        if (!jobDescriptionUrls.empty()) {
            webDownloader.load(jobDescriptionUrls.back().toStdWString());
            jobDescriptionUrls.pop_back();
        }
    }

    {
        std::lock_guard<std::mutex> lockGuard(webDataMutex);
        webDatas.emplace_back(url, html.toStdString());
    }

    if (downloadStep == 1) {
        ui.webDownloadJobLinkProgressBar->setValue(webDatas.size());
    } else if (downloadStep == 2) {
        ui.webDownloadJobDescriptionProgressBar->setValue(webDatas.size());
    }
    return;
}

void JobCrawlerGui::downloadPage()
{
    if (downloadStep == 1) {
        ui.webDownloadJobLinkProgressBar->setRange(0, jobItemUrls.size());
        std::lock_guard<std::mutex> jobItemLockGuard(jobItemUrlsMutex);
        for (int count = 0; count < webDownloaders.size(); ++count) {
            if (jobItemUrls.empty()) break;
            webDownloaders[count]->load(jobItemUrls.back().toStdWString());
            jobItemUrls.pop_back();
        }

    } else if (downloadStep == 2) {
        
        std::lock_guard<std::mutex> jobbDescriptionLockGuard(jobDescriptionUrlsMutex);
        for (int count = 0; count < webDownloaders.size(); ++count) {
            if (jobDescriptionUrls.empty()) break;
            webDownloaders[count]->load(jobDescriptionUrls.back().toStdWString());
            jobDescriptionUrls.pop_back();
        }

    }

    return;
}

void JobCrawlerGui::showJobResult()
{
    for (const auto& jobData : jobDatas) {
        ui.filterResultPlainTextEdit->appendPlainText(QString::fromStdString(jobData.jobTitle));
        
        for (const auto& tool : jobData.tools) {
            ui.filterResultPlainTextEdit->appendPlainText(QString::fromStdString(tool));
        }
        
        ui.filterResultPlainTextEdit->appendPlainText(QString::fromStdString(jobData.salary));
        const QString jobUrlLink = QString::fromStdString(jobData.url);
        ui.filterResultPlainTextEdit->appendHtml("<a href='" + jobUrlLink + "'>" + jobUrlLink + "</a>");
        ui.filterResultPlainTextEdit->appendPlainText("\n");
    }

    ui.filterPushButton->setEnabled(true);
    ui.filterResultPlainTextEdit->setEnabled(true);
}

void JobCrawlerGui::downloadAllJobItemPage(const QStringList urls)
{
    downloadStep = 1;
    {
        std::lock_guard<std::mutex> jobItemLockGuard(jobItemUrlsMutex);
        jobItemUrls.clear();
        for (const auto& url : urls) {
            jobItemUrls.push_back(url);
        }

        std::lock_guard<std::mutex> webDataLockGuard(webDataMutex);
        webDatas.clear();
    }
    emit startDownloadPage();
    while (true) {
        {
            std::lock_guard<std::mutex> lockGuard(webDataMutex);
            if (webDatas.size() == urls.size()) break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    {
        std::lock_guard<std::mutex> lockGuard(jobItemWebErrorMutex);
        jobItemWebError.clear();
        jobItemWebs.clear();
        for (const auto& webData : webDatas) {
            if (webData.html.empty()) {
                jobItemWebError.push_back(webData.url.toString());
            } else {
                jobItemWebs.push_back(webData);
            }
        }
    }
    webDatas.clear();
    
    downloadStep = 2;
    std::vector<QString> jdUrlsBuf;
    {
        HTMLParser htmlParser;
        for (const auto& jobItemWeb : jobItemWebs) {
            htmlParser.setHTML(jobItemWeb.html);
            std::vector<std::string> urls = htmlParser.getAttrValue(jobLinkHTML.tagType
                , jobLinkHTML.attributeName
                , jobLinkHTML.attributeValue
                , jobLinkHTML.contentAttribute);
            for (const auto& url : urls) {
                jdUrlsBuf.push_back(QString::fromStdString("https:" + url));
            }
        }
    }
    const size_t jobbDescriptionUrlsNum = jdUrlsBuf.size();

    {
        std::lock_guard<std::mutex> lockGuard(jobDescriptionUrlsMutex);
        jobDescriptionUrls.clear();
    }

    static const size_t sleepInterval = 250;
    ui.webDownloadJobDescriptionProgressBar->setRange(0, jobbDescriptionUrlsNum);
    while (!jdUrlsBuf.empty()) {
        for (size_t count = 0; count < sleepInterval; ++count) {
            if (jdUrlsBuf.empty()) break;
            {
                std::lock_guard<std::mutex> lockGuard(jobDescriptionUrlsMutex);
                jobDescriptionUrls.push_back(jdUrlsBuf.back());
            }
            jdUrlsBuf.pop_back();
        }
        if (jobDescriptionUrls.size() < sleepInterval) {
            emit startDownloadPage();
            break;
        } else {
            emit startDownloadPage();
            while (true) {
                bool downloadAllDone = true;
                {
                    std::lock_guard<std::mutex> lockGuard(jobDescriptionUrlsMutex);
                    downloadAllDone = jobDescriptionUrls.empty();
                }
                if (!downloadAllDone) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                    break;
                }
            }
        }
    }

    while (true) {
        {
            std::lock_guard<std::mutex> lockGuard(webDataMutex);
            if (webDatas.size() == jobbDescriptionUrlsNum) break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    {
        std::lock_guard<std::mutex> lockGuard(jobDescriptionErrorMutex);
        jobDescriptionError.clear();
        jobDescriptionWebs.clear();
        for (const auto& webData : webDatas) {
            if (webData.html.empty()) {
                jobDescriptionError.push_back(webData.url.toString());
            } else {
                jobDescriptionWebs.push_back(webData);
            }
        }
    }
    webDatas.clear();

    if (dataSetting.saveData) {
        nlohmann::json dataArray;
        for (const auto& webData : jobDescriptionWebs) {
            nlohmann::json item;
            item["url"] = webData.url.toString().toStdString();
            item["html"] = webData.html;
            dataArray.push_back(item);
        }
        nlohmann::json json;
        json["data"] = dataArray;
        const std::string jsonData = json.dump();
        FileHelper::overwrite(dataSetting.fileName, jsonData);
    }

    isWebDownloadDone.store(true);
    emit downloadPageFinshed();
    return;
}

void JobCrawlerGui::saveTagHTML()
{
    jobLinkHTML.tagType = ui.jobLinkHTMLTagTypeLineEdit->text().toStdWString();
    jobLinkHTML.attributeName = ui.jobLinkHTMLAttributeNameLineEdit->text().toStdWString();
    jobLinkHTML.attributeValue = ui.jobLinkHTMLAttributeValueLineEdit->text().toStdWString();
    jobLinkHTML.contentAttribute = ui.jobLinkHTMLContentAttributeLineEdit->text().toStdWString();

    jobTitleHTML.tagType = ui.jobTitleHTMLTagTypeLineEdit->text().toStdWString();
    jobTitleHTML.attributeName = ui.jobTitleHTMLAttributeNameLineEdit->text().toStdWString();
    jobTitleHTML.attributeValue = ui.jobTitleHTMLAttributeValueLineEdit->text().toStdWString();
    jobTitleHTML.contentAttribute = ui.jobTitleHTMLContentAttributeLineEdit->text().toStdWString();

    toolHTML.tagType = ui.toolHTMLTagTypeLineEdit->text().toStdWString();
    toolHTML.attributeName = ui.toolHTMLAttributeNameLineEdit->text().toStdWString();
    toolHTML.attributeValue = ui.toolHTMLAttributeValueLineEdit->text().toStdWString();

    jobContentHTML.tagType = ui.jobContentHTMLTagTypeLineEdit->text().toStdWString();
    jobContentHTML.attributeName = ui.jobContentHTMLAttributeNameLineEdit->text().toStdWString();
    jobContentHTML.attributeValue = ui.jobContentHTMLAttributeValueLineEdit->text().toStdWString();

    salaryHTML.tagType = ui.salaryHTMLTagTypeLineEdit->text().toStdWString();
    salaryHTML.attributeName = ui.salaryHTMLAttributeNameLineEdit->text().toStdWString();
    salaryHTML.attributeValue = ui.salaryHTMLAttributeValueLineEdit->text().toStdWString();

    return;
}

void JobCrawlerGui::saveSetting()
{
    filterSwitchSetting.jobContentEnable =
        ui.filterSwitchJobContentCheckBox->checkState() == Qt::Checked ? true : false;
    filterSwitchSetting.jobTitleEnable =
        ui.filterSwitchJobTitleCheckBox->checkState() == Qt::Checked ? true : false;
    filterSwitchSetting.toolEnable =
        ui.filterSwitchToolCheckBox->checkState() == Qt::Checked ? true : false;

    dataSetting.fileName = ui.dataSettingFileNameLineEdit->text().toStdWString();
    dataSetting.saveData =
        ui.dataSettingSaveFileCheckBox->checkState() == Qt::Checked ? true : false;
    dataSetting.useFile =
        ui.dataSettingUseFileCheckBox->checkState() == Qt::Checked ? true : false;
    configureLoader->setSettingDataSetting(dataSetting);

    toolSetting.include.clear();
    int toolItemNum = ui.toolFilterIncludeListWidget->count();
    for (int count = 0; count < toolItemNum; ++count) {
        toolSetting.include.push_back(ui.toolFilterIncludeListWidget->item(count)->text().toStdWString());
    }
    toolSetting.exclude.clear();
    toolItemNum = ui.toolFilterExcludeListWidget->count();
    for (int count = 0; count < toolItemNum; ++count) {
        toolSetting.exclude.push_back(ui.toolFilterExcludeListWidget->item(count)->text().toStdWString());
    }

    jobTitleSetting.include.clear();
    int jobTitleItemNum = ui.jobTitleFilterIncludeListWidget->count();
    for (int count = 0; count < jobTitleItemNum; ++count) {
        jobTitleSetting.include.push_back(ui.jobTitleFilterIncludeListWidget->item(count)->text().toStdWString());
    }
    jobTitleSetting.exclude.clear();
    jobTitleItemNum = ui.jobTitleFilterExcludeListWidget->count();
    for (int count = 0; count < jobTitleItemNum; ++count) {
        jobTitleSetting.exclude.push_back(ui.jobTitleFilterExcludeListWidget->item(count)->text().toStdWString());
    }

    jobContentSetting.include.clear();
    int jobContentItemNum = ui.jobContentFilterIncludeListWidget->count();
    for (int count = 0; count < jobContentItemNum; ++count) {
        jobContentSetting.include.push_back(ui.jobContentFilterIncludeListWidget->item(count)->text().toStdWString());
    }
    jobContentSetting.exclude.clear();
    jobContentItemNum = ui.jobContentFilterExcludeListWidget->count();
    for (int count = 0; count < jobContentItemNum; ++count) {
        jobContentSetting.exclude.push_back(ui.jobContentFilterExcludeListWidget->item(count)->text().toStdWString());
    }
}

void JobCrawlerGui::filterJob()
{
    while (!isWebDownloadDone.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    jobDatas.clear();

    HTMLParser htmlParser;
    for (const auto& jd : jobDescriptionWebs) {
        htmlParser.setHTML(jd.html);
        JobData jobData;
        jobData.tools =
            htmlParser.getInnerHTML(toolHTML.tagType
                , toolHTML.attributeName
                , toolHTML.attributeValue);
        
        jobData.jobTitle =
            htmlParser.getAttrValue(jobTitleHTML.tagType
                , jobTitleHTML.attributeName
                , jobTitleHTML.attributeValue
                , jobTitleHTML.contentAttribute)
            .front();

        const std::vector<std::string> jobContent 
            = htmlParser.getInnerHTML(jobContentHTML.tagType
                , jobContentHTML.attributeName
                , jobContentHTML.attributeValue);
        if (jobContent.empty()) {
            jobData.jobContent = "";
        } else {
            jobData.jobContent = jobContent.front();
        }

        const std::vector<std::string> salary
            = htmlParser.getInnerHTML(salaryHTML.tagType
                , salaryHTML.attributeName
                , salaryHTML.attributeValue);
        if (salary.empty()) {
            jobData.salary = "";
        } else {
            jobData.salary = salary.front();
        }

        jobData.url = jd.url.toString().toStdString();

        jobDatas.push_back(jobData);
    }

    for (auto it = jobDatas.begin(); it != jobDatas.end();) {
        auto jobData = *it;
        if (filterSwitchSetting.toolEnable) {
            if (::filterData(toolSetting.include, toolSetting.exclude,
                jobData.tools)) {
                it = jobDatas.erase(it);
                continue;
            }
        }

        if (filterSwitchSetting.jobTitleEnable) {
            if (::filterData(jobTitleSetting.include, jobTitleSetting.exclude,
                jobData.jobTitle)) {
                it = jobDatas.erase(it);
                continue;
            }
        }

        if (filterSwitchSetting.jobContentEnable) {
            if (::filterData(jobContentSetting.include, jobContentSetting.exclude,
                jobData.jobContent)) {
                it = jobDatas.erase(it);
                continue;
            }
        }

        ++it;
    }

    emit filterDone();
}