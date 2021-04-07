#pragma once

#include <memory>

#include <QtWidgets/QMainWindow>
#include "ui_JobCrawlerGui.h"

namespace JobCrawler {
    class ConfigureLoader;
}

class JobCrawlerGui : public QMainWindow
{
    Q_OBJECT

public:
    JobCrawlerGui(QWidget *parent = Q_NULLPTR);

protected slots:
    void on_configureReloadPushButton_clicked();

private:
    Ui::JobCrawlerGuiClass ui;

private:
    std::shared_ptr<JobCrawler::ConfigureLoader> configureLoader;
};
