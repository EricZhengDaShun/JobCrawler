#include "JobCrawlerGui.hpp"

#include <exception>
#include <string>
#include <vector>

#include <qstring.h>
#include <qtranslator.h>

#include "ConfigureLoader.hpp"

using namespace JobCrawler;

JobCrawlerGui::JobCrawlerGui(QWidget *parent)
    : QMainWindow(parent)
    , configureLoader(std::make_shared<ConfigureLoader>())
{
    ui.setupUi(this);
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
    ui.filterSwitchToolEnableCheckBox->setCheckState(toolEnable ? Qt::Checked : Qt::Unchecked);

    const bool useFile = configureLoader->getSettingDataSetting().useFile;
    const bool saveFile = configureLoader->getSettingDataSetting().saveData;
    const std::wstring dataFileName = configureLoader->getSettingDataSetting().fileName;
    ui.dataSettingUseFileCheckBox->setCheckState(useFile ? Qt::Checked : Qt::Unchecked);
    ui.dataSettingSaveFileCheckBox->setCheckState(saveFile ? Qt::Checked : Qt::Unchecked);
    ui.dataSettingFileNameLineEdit->setText(QString::fromStdWString(dataFileName));

    const std::vector<std::wstring> toolIncludes = configureLoader->getSettingTool().include;
    const std::vector<std::wstring> toolExcludes = configureLoader->getSettingTool().exclude;
    for (const auto& v : toolIncludes) {
        ui.toolFilterIncludeListWidget->addItem(QString::fromStdWString(v));
    }
    for (const auto& v : toolExcludes) {
        ui.toolFilterExcludeListWidget->addItem(QString::fromStdWString(v));
    }

    ui.statusBar->showMessage(tr("Reload configure file done !"));
    return;
}
