#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include "A2168_interface.h"

void MainWindow::setTableBitFlags (QTableView *tablePlace, QStandardItemModel *&tableName, const struct BITS *bits)
{
    const   quint8      rows        = 2;
    const   quint8      columns     = 8;
    const   quint8      height      = 20;
    const   quint8      width       = 49;
    const   QSize       table_size  = QSize(width*columns+2, height*rows+2);
            QModelIndex index;

    tableName = new QStandardItemModel(rows,columns,this);    //Создадим модель таблицы
    tablePlace->setModel(tableName);                          //Установим модель
    tablePlace->verticalHeader()->setMinimumSectionSize(5);   //Минимальная высота строки, без этого не работает изменение
//    tablePlace->horizontalHeader()->setMinimumSectionSize(5); //Минимальная ширина строки, без этого не работает изменение
    tablePlace->verticalHeader()->setDefaultSectionSize(height);  //Высота строки по умолчанию
    tablePlace->horizontalHeader()->setDefaultSectionSize(width); //Ширина строки по умолчанию
    tablePlace->verticalHeader()->hide();
    tablePlace->horizontalHeader()->hide();
    tablePlace->setMaximumSize(table_size);

    for(quint8 row=0; row<rows; row++){
        for (quint8 column=0; column<columns; column++){
            index = tableName->index(1-row,7-column); //Найдем индекс нужной ячейки
            tableName->setData(index, bits[columns*row + column].name);//Установим название
            tableName->setData(index, bits[columns*row + column].tooltip, Qt::ToolTipRole);//Установим подсказку
//            QBrush br(Qt::red);
//            tableName->setData(index, br, Qt::BackgroundRole);//Установим подсказку
            tableName->item(1-row,7-column)->setTextAlignment(Qt::AlignCenter); //Выровняем по центру
        }
    }
}
