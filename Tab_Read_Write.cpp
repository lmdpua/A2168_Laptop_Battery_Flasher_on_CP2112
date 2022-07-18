#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

void MainWindow::on_button_SendPassKey_clicked()
{
    bool        ok;             //Флаг успешного преобразования из текста в число
    QString     str;            //Объявляем строку для приема текста
    quint64     pass;           //Переменная для приема пароля

    while(device.isInTransferMode()){} //Ждем пока закончится передача
    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        return;
    }

    str = ui->lineEdit_Passkey->text();     //Получаем строку из поля для ввода
    pass = str.toUInt(&ok, 16); //Преобразуем строку в число

    if (!ok || pass>0xffffffff){       //Если преобразование не успешное или число больше чем 0xff
        QMessageBox::critical(this,"Error","Incorrect data entered\nIt must be an integer between\n0x0000 0000 and 0xFFFF FFFF");
        device.close();
        device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
        return;
    }

    quint16 pass1 = pass >> 16;
    quint16 pass2 = pass & 0xFFFF;

    if(battery.writeWord(&device,0x00,pass1)){  //Если первое слово записалось
        if(!battery.writeWord(&device,0x00,pass2)){ //Если второе слово НЕ записалось
            QMessageBox::critical(this,"Error","Write Pass error");
        }
    }else{
        QMessageBox::critical(this,"Error","Write Pass error");
    }

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
}

void MainWindow::on_button_ReadWord_clicked()
{
    bool        ok;                 //Флаг успешного преобразования из текста в число
    quint64     addr = -1;          //Переменная для полученного числа
    QString     str;                //Объявляем строку для приема текста
    quint16     data;

    while(device.isInTransferMode()){} //Ждем пока закончится передача
    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        return;
    }

    str = ui->lineEdit_readWordAddress->text();     //Получаем строку из поля для ввода
    addr = str.toUInt(&ok, 16); //Преобразуем строку в число

    if (!ok || addr>0xff){       //Если преобразование не успешное или число больше чем 0xff
        QMessageBox::critical(this,"Error","Incorrect data entered\nIt must be an integer between\n0x00 and 0xFF");
        device.close();
        device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
        return;
    }

    if(!battery.readWord(&device, addr, &data)){
        QMessageBox::critical(this,"Error","Read Word error");
        device.close();
        device.setTransfer(false);
    }
    str = QString("%1").arg(data, 4, 16, QLatin1Char('0')).toUpper();
    ui->lineEdit_resultReadWordHEX->setText(str);
    str = QString::number(data);
    ui->lineEdit_resultReadWordDEC->setText(str);

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
}

void MainWindow::on_button_WriteWordHEX_clicked()
{
    writeWord_HEX_DEC(true);
}

void MainWindow::on_button_WriteWordDec_clicked()
{
    writeWord_HEX_DEC(false);
}

void MainWindow::writeWord_HEX_DEC(bool hex)
{
    bool        ok;             //Флаг успешного преобразования из текста в число
    quint64     addr    = -1;   //Переменная для полученного числа
    QString     str;            //Объявляем строку для приема текста
    quint64     data;

    while(device.isInTransferMode()){} //Ждем пока закончится передача
    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        return;
    }

    str = ui->lineEdit_WriteWordAddress->text();    //Получаем строку из поля для ввода
    addr = str.toUInt(&ok, 16); //Преобразуем строку в число

    if (!ok || addr>0xff){      //Если преобразование не успешное или число больше чем 0xff
        QMessageBox::critical(this,"Error","Incorrect address entered\nIt must be an integer between\n0x00 and 0xFF");
        device.close();
        device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
        return;
    }

    if(hex){
        str = ui->lineEdit_WriteWordDataHEX->text();     //Получаем строку из поля для ввода
        data = str.toUInt(&ok, 16); //Преобразуем строку в HEX число
    }else{
        str = ui->lineEdit_WriteWordDataDEC->text();
        data = str.toUInt(&ok, 10); //Преобразуем строку в DEC число
    }

    if (!ok || data>0xffff){    //Если преобразование не успешное или число больше чем 0xffff
        QMessageBox::critical(this,"Error","Incorrect data entered\nIt must be an integer between\n0x0000 and 0xFFFF");
        device.close();
        device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
        return;
    }

    if(!battery.writeWord(&device,(quint8)addr,(quint16)data)){
        QMessageBox::critical(this,"Error","Write Word error");
    }

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
}

void MainWindow::on_button_ReadBlock_clicked()
{
    bool        ok;                 //Флаг успешного преобразования из текста в число
    quint64     addr = -1;          //Переменная для полученного числа
    QString     str;                //Объявляем строку для приема текста
    QByteArray  array;
    quint8      len;

    while(device.isInTransferMode()){} //Ждем пока закончится передача
    device.setTransfer(true);//Поставим флаг, что устройство в режиме передачи
    if(!device.init()){
        device.close();
        device.setTransfer(false);
        return;
    }

    str = ui->lineEdit_ReadBlockAddress->text();     //Получаем строку из поля для ввода
    addr = str.toUInt(&ok, 16); //Преобразуем строку в число

    if (!ok || addr>0xff){       //Если преобразование не успешное или число больше чем 0xff
        QMessageBox::critical(this,"Error","Incorrect data entered\nIt must be an integer between\n0x00 and 0xFF");
        device.close();
        device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
        return;
    }

    if(!battery.readBlock(&device, addr, &array, &len)){
        QMessageBox::critical(this,"Error","Block Read error");
        device.close();
        device.setTransfer(false);
        return;
    }

    str.clear();
    str.append(array);
    ui->lineEdit_ReadBlockResultText->setText(str);

    str.clear();
    str = array.toHex(' ').toUpper();
    ui->textEdit_ReadBlockResultHEX->setPlainText(str);

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
}

void MainWindow::on_button_writeBlockDataHex_clicked()
{
    writeBlock_HEX_TXT(true);
}

void MainWindow::on_button_writeBlockDataText_clicked()
{
    writeBlock_HEX_TXT(false);
}

void MainWindow::writeBlock_HEX_TXT(bool hex)
{
    bool        ok               = 0;           //Флаг успешного преобразования из текста в число
    quint64     addr             = -1;          //Переменная для полученного числа
    QString     str;                            //Объявляем строку для приема текста
    quint8      strSize          = 0;
    quint8      block_length     = 0;
    bool        PEC;
    QByteArray  array;

    str = ui->lineEdit_WriteBlockAddress->text();     //Получаем строку адреса из поля для ввода
    addr = str.toUInt(&ok, 16); //Преобразуем строку в число
    if (!ok || addr>0xff){       //Если преобразование не успешное или число больше чем 0xff
        QMessageBox::critical(this,"Error","Incorrect data entered\nIt must be an integer between\n0x00 and 0xFF");
        device.close();
        device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
        return;
    }

    str.clear(); //Очистим строку

    if(hex){
        str = ui->lineEdit_WriteBlockDataHEX->toPlainText();
        QRegularExpression re("[^01234567890abcdefABCDEF]");
        str.remove(re);//Удалим неправильные символы
        strSize = str.size();

        if (strSize % 2 == 1){//Если количество символов нечетное
            QMessageBox::critical(this,"Error","Please enter an even number of characters");
            device.close();
            device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
            return;
        }

        block_length = strSize/2;

        if (block_length>MAX_SMBUS_BLOCK_SIZE){ //Если длина блока получится больше 32 байт
            QMessageBox::critical(this,"Error","The maximum number of characters is 64");
            device.close();
            device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
            return;
        }

        array.resize(block_length);

        for (BYTE i=0; i<strSize; i+=2){//Преобразуем строку в массив
            QString byteStr = str.mid(i,2);
            array[i/2]=byteStr.toUInt(&ok, 16);
            if (!ok){       //Если преобразование не успешное или число больше чем 0xff
                QMessageBox::critical(this,"Error","Interpretation error");
                device.close();
                device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
                return;
            }
        }

        PEC = (ui->checkBox_PEC_HEX->isChecked()) ? true : false;
    }else{
        str = ui->lineEdit_WriteBlockDataText->text();
        block_length = str.size();

        if (block_length > MAX_SMBUS_BLOCK_SIZE){
            QMessageBox::critical(this,"Error", "The number of characters must not exceed 32");
            device.close();
            device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
            return;
        }

        array.resize(block_length);

        for (BYTE i=0; i<block_length; i++){
            array[i] = (str.data()+i)->unicode(); //str.data() - указатель на первый символ строки
        }

        PEC = (ui->checkBox_PEC_Text->isChecked()) ? true : false;
    }

    if (!battery.writeBlock(&device, addr, &array,PEC)){
        QMessageBox::critical(this,"Error","Block Write error");
    }

    device.close();
    device.setTransfer(false);//снимем флаг, что устройство в режиме передачи
}
