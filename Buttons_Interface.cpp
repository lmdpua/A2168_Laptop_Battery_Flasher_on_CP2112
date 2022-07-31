#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::hideAllTabs()
{
    quint8 tabsNum=ui->tabWidget->count(); //Сколько всего вкладок
    while(tabsNum)
    {
        ui->tabWidget->removeTab(0);//Удалим самую левую вкладку
        tabsNum--;
    }
    ui->button_InLayoutReadFlash->hide();
    ui->button_InLayoutWriteFlash->hide();
    ui->progressBarFlashDelailed->hide();
}

void MainWindow::on_button_SBS_clicked()
{
    hideAllTabs();
    ui->tabWidget->insertTab(0, ui->tab_Standart_SBS, "Standart SBS");
    ui->tabWidget->insertTab(1, ui->tab_ExtendedUnseal, "Extended SBS in Unseal Mode");
    ui->tabWidget->insertTab(2, ui->tab_BitFlags, "Bit Flags");
}

void MainWindow::on_button_FlashCommon_clicked()
{
    hideAllTabs();
    ui->tabWidget->insertTab(0, ui->tab_FlashMemory, "Flash Memory");
}

void MainWindow::on_button_FlashDetailed_clicked()
{
    hideAllTabs();
    ui->tabWidget->insertTab(0, ui->tab_1stLevelProtection, "1st Level Protection");
    ui->tabWidget->insertTab(1, ui->tab_2ndLevelProtection, "2nd Level Protection");
    ui->tabWidget->insertTab(2, ui->tab_Charge_Control,     "Charge Control");
    ui->tabWidget->insertTab(3, ui->tab_System_Setting,     "System Setting");
    ui->tabWidget->insertTab(4, ui->tab_SBS_Setting,        "SBS Setting");
    ui->tabWidget->insertTab(5, ui->tab_System_Data,        "System Data");
    ui->tabWidget->insertTab(6, ui->tab_Gas_Gauge,          "Gas Gauge");
    ui->tabWidget->insertTab(7, ui->tab_Calibration,        "Calibration");
    ui->tabWidget->insertTab(8, ui->tab_LED_Support,        "LED_Support");

    ui->button_InLayoutReadFlash->show();
    ui->button_InLayoutWriteFlash->show();
    ui->progressBarFlashDelailed->show();
}

void MainWindow::on_button_Pro_clicked()
{
    hideAllTabs();
    ui->tabWidget->insertTab(0, ui->tab_Read_Write, "Read/Write");
}
