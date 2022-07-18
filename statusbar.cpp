#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include <QTableWidget>
#include <QHeaderView>
#include <QProgressBar>


void MainWindow::setTableStatusbar(QTableWidget *table)
{
//    table = new QTableWidget(this); //Создадим пустой виджет таблицы
//    ui->statusbar->addPermanentWidget(table);//Установим его в статусбар
    ui->statusbar->addWidget(table);
    quint8 rows     = 2;
    quint8 columns  = 5;
    table->setRowCount(rows);       //число строк
    table->setColumnCount(columns); //число столбцов
    table->setColumnWidth(0,130);
    table->setColumnWidth(1,250);
    table->setColumnWidth(2,250);
    table->setColumnWidth(3,250);
    table->setColumnWidth(4,140);
    table->horizontalHeader()->setStretchLastSection(true); // Устанавливаем адаптивный размер последнего столбца
    table->verticalHeader()->setMinimumSectionSize(1); //Без этого невозможно уменьшить высоту ячейки меньше определенного
    table->setRowHeight(0,18); //Высота ячеек
    table->setRowHeight(1,18); //Высота ячеек
    table->setFixedSize(1024,38);
    table->horizontalHeader()->hide(); //скроем заголовки
    table->verticalHeader()->hide();   //скроем заголовки

    for(quint8 x=0; x<rows; x++)
    {
        for(quint8 y=0; y<columns; y++)
        {
            QTableWidgetItem *itm = new QTableWidgetItem();         //Создадим пустой предмет таблицы
            table->setItem(x,y,itm);                                //установим в каждую ячейку
            table->item(x,y)->setBackground(QColor(240,240,240));   //Задний фон
            table->item(x,y)->setFlags(Qt::ItemIsEnabled);//Включим только один флаг
        }
    }

    batteryLevel = new QProgressBar();
    table->setCellWidget(0,4,batteryLevel);
    batteryLevel->setRange(0,100);
    batteryLevel->setValue(0);
}
