#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cp2112.h"
#include "smbus.h"
#include <QTableWidget>
#include <QTimer>
#include <QProgressBar>
#include <QStandardItemModel>
#include "A2168_interface.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setTableStatusbar(QTableWidget *table);

    void setTableStandardSBS(QTableView *tablePlace, QStandardItemModel *&tableName, BYTE start, BYTE end);
    void setDataTableStandardSBS(QStandardItemModel *&tableName, quint8 start, quint8 end, CP2112 *dev, SMBus *batt);

    void setTableExtendedUnseal(QTableView *tablePlace, QStandardItemModel *&tableName, BYTE start, BYTE end);
    void setDataTableExtendedUnseal(QStandardItemModel *&tableName, quint8 start, quint8 end, CP2112 *dev, SMBus *batt);

    void setTableFlashMemory(QTableView *tablePlace, QStandardItemModel *&tableName);
    void setDataTableFlashMemory(void);
    void onTableFlashMemoryCell();//const QModelIndex &current,const QModelIndex &previous);

private slots:
    void slotTimerStatusBarAlarm();

    void on_button_StandartSBS_ReadOnce_clicked();
    void on_button_StandartSBS_ReadAllTheTime_clicked();

    void on_button_SendPassKey_clicked();
    void on_button_ReadWord_clicked();
    void on_button_WriteWordHEX_clicked();
    void on_button_WriteWordDec_clicked();
    void writeWord_HEX_DEC(bool hex);
    void on_button_ReadBlock_clicked();
    void on_button_writeBlockDataHex_clicked();
    void on_button_writeBlockDataText_clicked();
    void writeBlock_HEX_TXT(bool hex);


    void on_button_ExtendedUnseal_ReadOnce_clicked();

    void on_button_ReadFlash_clicked();
    void on_button_WriteFlash_clicked();
    void onTableFlashMemoryCellClicked(const QModelIndex &current,const QModelIndex &previous);
    void onTableFlashMemoryCellChanded(const QModelIndex &current,const QModelIndex &previous);
    void flashMemorySaveToCell();
    void on_button_SaveFlashDataToFile_clicked();
    bool saveToFile(QString *pathToFile, QByteArray *dataToSave);
    bool readFromFile(QString *pathToFile, QByteArray *dataFromFile);



    void on_button_LoadFlashDataFromFile_clicked();

    void on_button_Search_clicked();

    void on_lineEdit_Search_editingFinished();

private:
    Ui::MainWindow *ui;
    CP2112  device;
    SMBus   battery;

    QByteArray *flashData;
    bool flashArrayIsFull;

    QTimer *timerStatusBar;
    QTimer *timerStandartSBSReading;
    QTableWidget *statusbarTable;
    QProgressBar *batteryLevel;

    QStandardItemModel *tableStandartSBS_Left;
    QStandardItemModel *tableStandartSBS_Right;

    QStandardItemModel *tableExtendedUnseal_Left;
    QStandardItemModel *tableExtendedUnseal_Right;

    QStandardItemModel *tableFlashMemory;

};
#endif // MAINWINDOW_H