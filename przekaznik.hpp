#include <iostream>
#include <bitset>
#include <cstring>
#include <fstream>


{
    QString plik1 = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 "c://",
                                                 tr("Fasta (*.fasta)"));
    ui -> comboBox -> clear();
    QFileInfo f1(plik1);
    ui -> label -> setText(f1.fileName());
    QFile file(plik1);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){ // metoda open w otwiera plik w trybie tylko do odczytu z zamianą znaków \r\n ma \n
        //int i = 0;
        QTextStream in(&file);
        //QString line = in.readLine();
        //QString text = in.readAll();
        while(!in.atEnd()){
            QString line = in.readLine();
            if (line.front() == ">"){
                ui->comboBox->addItem(line);
                if(line.front() != ">"){
                    ui -> textEdit -> setText(line);
                }
            }

}

        //ui->comboBox->addItem(line);
        //ui->textEdit->setText(text);
        //if (text.startsWith(">")){
        //    i++;
       // }
       //QString s = QString::number(i);
        //ui -> textEdit_2 -> setText(s);
        //file.close();

    }
}
