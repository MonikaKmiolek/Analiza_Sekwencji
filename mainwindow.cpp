#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "NeedlemanWunsch.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <iostream>
#include <fstream>
#include <string>





MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMinimumSize(QSize(670,610));
    this->setMaximumSize(QSize(1555,610));
    ui->plot->hide();
    ui->pushButton_6 ->hide();
    ui->pushButton_7 ->hide();
    ui->pushButton_8 ->hide();
    ui->pushButton_9 ->hide();
    ui->pushButton_13->hide();
    ui->plot->xAxis->setVisible(false);
    ui->plot->xAxis2->setVisible(true);
    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plot->setInteraction(QCP::iRangeDrag,true);
    ui->plot->setInteraction(QCP::iRangeZoom,true);
    ui->plot->axisRect()->setRangeDragAxes(ui->plot->xAxis2,ui->plot->yAxis);
    ui->plot->axisRect()->setRangeZoomAxes(ui->plot->xAxis2,ui->plot->yAxis);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addPoint()
{

    QCPGraph *points = new QCPGraph(ui->plot->xAxis2,ui->plot->yAxis);
    QString x,y;
    QString x1 = ui -> textEdit_3 -> toPlainText();
    QString y1 = ui -> textEdit_4 -> toPlainText();
    for(i = 0; i<=x1.size();i++){
        for(j =0;j<=y1.size();j++)
        {
            if (x1[i] == y1[j]){
              points->addData(i,j);
              points->setLineStyle(QCPGraph::lsNone);
              points->setScatterStyle(QCPScatterStyle::ssPlus);
              ui->plot->yAxis->setRangeReversed(true);
              ui->plot->xAxis2->setLabel(ui->comboBox->currentText());
              ui->plot->yAxis->setLabel(ui->comboBox_2->currentText());

            }
        }
    }
}

void MainWindow::plot()
{


    ui->plot->xAxis2->setRange(0,i);
    ui->plot->yAxis->setRange(0,j);

    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::on_pushButton_clicked()
{
    QString plik1 = QFileDialog::getOpenFileName(this, tr("Otwórz plik FASTA"),
                                                 "c://",
                                                 tr("Fasta (*.fasta)"));
    ui -> comboBox -> clear();
    QFileInfo f1(plik1);
    ui -> label -> setText(f1.filePath());
    QFile file(plik1);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        while(!in.atEnd()){

            QString line = in.readLine();
            if (line.front() == ">"){
                ui->comboBox->addItem(line);

                }
            }

        }
}

void MainWindow::on_pushButton_2_clicked()
{
    QString plik2 = QFileDialog::getOpenFileName(this, tr("Otwórz plik FASTA"),
                                                  "c://",
                                                  tr("Fasta (*.fasta)"));
    QFileInfo f2(plik2);
    ui -> label_2 -> setText(f2.filePath());
    ui -> comboBox_2 -> clear();
    QFile file(plik2);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        while(!in.atEnd()){
            QString line = in.readLine();
            if (line.front() == ">"){
                ui->comboBox_2->addItem(line);
                }
            }
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    QTextCursor cursor;
    QString text;
    cursor = ui->textEdit->textCursor();
    text = cursor.selectedText();
    ui -> textEdit_3 -> setPlainText(text);

    QFile file("seq1.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << text;
    file.close();


}

void MainWindow::on_pushButton_4_clicked()
{
    QTextCursor cursor;
    QString text;
    cursor = ui->textEdit_2->textCursor();
    text = cursor.selectedText();
    ui -> textEdit_4 -> setText(text);
    QFile file("seq2.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << text;
    file.close();

}

void MainWindow::on_pushButton_5_clicked()
{
    QString q1;
    QString q2;
    if (!(ui->textEdit_3->document()->isEmpty() || ui->textEdit_4->document()->isEmpty())){
        QFile S1("seq1.txt");
        if(S1.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream in(&S1);
                q1 = in.readAll();
                S1.close();
        }
        QFile S2("seq2.txt");
        if(S2.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream in(&S2);
                q2 = in.readAll();
                S1.close();

        }
        std::string Q1 = q1.toLocal8Bit().constData();
        std::string Q2 = q2.toLocal8Bit().constData();
        if(Q1.length() > Q2.length()){
        NeedlemanWunsch *nw = new NeedlemanWunsch(Q2,Q1);
        nw->populate_subs_matrix("BLOSUM62.txt");
        nw->calculate_similarity();
        nw->dna_align();
        nw->print_results();
        }
        else{
            NeedlemanWunsch *nw = new NeedlemanWunsch(Q1,Q2);
            nw->populate_subs_matrix("BLOSUM62.txt");
            nw->calculate_similarity();
            nw->dna_align();
            nw->print_results();
        }
        ui->pushButton_9->show();
    }
    else{
        QMessageBox::information(this, "Błąd","Brak danych do porównania");
    }
}
void MainWindow::on_comboBox_highlighted(const QString &arg1)
{
    QString plik1 = ui->label->text();
    QFile file(plik1);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        QString line = in.readLine();
        QString x = arg1;
        while (line != x ){
            line = in.readLine();
        }
        line = in.readLine();
        ui -> textEdit ->clear();
        do{

            ui ->textEdit ->append(line);
            line = in.readLine();
        }while((!line.isNull()) && (line.front() != ">"));
    }
}

void MainWindow::on_comboBox_2_highlighted(const QString &arg1)
{
    QString plik1 = ui->label_2->text();
    QFile file(plik1);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        QString line = in.readLine();
        QString x = arg1;
        while (line != x ){
            line = in.readLine();
        }
        line = in.readLine();
        ui -> textEdit_2 ->clear();
        do{
            ui ->textEdit_2 ->append(line);
            line = in.readLine();
        }while((!line.isNull()) && (line.front() != ">"));
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    addPoint();
    plot();
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->plot->xAxis2->setRange(*std::min_element(k.begin(),k.end()),*std::max_element(k.begin(),k.end()));
    ui->plot->xAxis2->setRange(*std::min_element(l.begin(),l.end()),*std::max_element(l.begin(),l.end()));
    plot();
}

void MainWindow::on_pushButton_8_clicked()
{
    ui->plot->clearGraphs();
    plot();
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1==0){
        this->setFixedSize(QSize(670, 610));
        ui->plot->hide();
        ui->pushButton_6 ->hide();
        ui->pushButton_7 ->hide();
        ui->pushButton_8 ->hide();
        ui->pushButton_13->hide();
    }
    else{
        this->setFixedSize(QSize(1555, 610));
        ui->plot->show();
        ui->pushButton_6 ->show();
        ui->pushButton_7 ->show();
        ui->pushButton_8 ->show();
        if (ui->pushButton_9->isVisible()){
            ui->pushButton_13->show();
        }

    }
}

void MainWindow::on_pushButton_9_clicked()
{
        QPrinter printer(QPrinter::HighResolution);
          printer.setOrientation(QPrinter::Portrait);
          printer.setOutputFormat(QPrinter::PdfFormat);
          printer.setPaperSize(QPrinter::A4);
          QString fileName = QFileDialog::getSaveFileName(nullptr,"Zapisz raport", "c:\\", "pdf (*.pdf)");
          printer.setOutputFileName(fileName);

          QPainter painter;
          painter.begin(&printer);
          QFile S1("pdf.txt");
          int i =200;
          if(S1.open(QIODevice::ReadOnly | QIODevice::Text)){
                  QTextStream in(&S1);
                  while(!in.atEnd()){
                  painter.drawText( 200,i , in.readLine(70) );
                  i+=200;
                  }

                  S1.close();
          painter.end();
          }
          QDesktopServices::openUrl(QUrl(fileName));
   S1.remove();
   QFile x1("seq1.txt");
   QFile x2("seq2.txt");
   QFile x3("seq3.txt");
   QFile x4("seq4.txt");
   x1.remove();
   x2.remove();
   x3.remove();
   x4.remove();
}


void MainWindow::on_pushButton_10_clicked()
{
    addPoint();
    ui->plot->clearGraphs();
    QCPGraph *points = new QCPGraph(ui->plot->xAxis2,ui->plot->yAxis);
    QString x,y;
    QString x1 = ui -> textEdit_3 -> toPlainText();
    QString y1 = ui -> textEdit_4 -> toPlainText();
    for(i = 0; i<=x1.size();i++){
        for(j =0;j<=y1.size();j++)
        {
            if (x1[i] == y1[j] && x1[i+1] == y1[j+1] && x1[i+2] == y1[j+2] && x1[i+3] == y1[j+3]){
              points->addData(i,j);
              points->setLineStyle(QCPGraph::lsNone);
              points->setScatterStyle(QCPScatterStyle::ssPlus);
              ui->plot->xAxis2->setLabel(ui->comboBox->currentText());
              ui->plot->yAxis->setLabel(ui->comboBox_2->currentText());
              i+=3;
              j+=3;
            }
        }
    }
    plot();
}


void MainWindow::on_pushButton_11_clicked()

    {
        addPoint();
        ui->plot->clearGraphs();
        QCPGraph *points = new QCPGraph(ui->plot->xAxis2,ui->plot->yAxis);
        QString x,y;
        QString x1 = ui -> textEdit_3 -> toPlainText();
        QString y1 = ui -> textEdit_4 -> toPlainText();
        for(i = 0; i<=x1.size();i++){
            for(j =0;j<=y1.size();j++)
            {
                if (x1[i] == y1[j] && x1[i+1] == y1[j+1] && x1[i+2] == y1[j+2] && x1[i+3] == y1[j+3] && x1[i+4] == y1[j+4] && x1[i+5] == y1[j+5]){
                  points->addData(i,j);
                  points->setLineStyle(QCPGraph::lsNone);
                  points->setScatterStyle(QCPScatterStyle::ssPlus);
                  ui->plot->xAxis2->setLabel(ui->comboBox->currentText());
                  ui->plot->yAxis->setLabel(ui->comboBox_2->currentText());
                  i+=5;
                  j+=5;
                }
            }
        }
        plot();
    }


void MainWindow::on_pushButton_12_clicked()
{
    addPoint();
    ui->plot->clearGraphs();
    QCPGraph *points = new QCPGraph(ui->plot->xAxis2,ui->plot->yAxis);
    QString x,y;
    QString x1 = ui -> textEdit_3 -> toPlainText();
    QString y1 = ui -> textEdit_4 -> toPlainText();
    for(i = 0; i<=x1.size();i++){
        for(j =0;j<=y1.size();j++)
        {
            if (x1[i] == y1[j] && x1[i+1] == y1[j+1] && x1[i+2] == y1[j+2] && x1[i+3] == y1[j+3] && x1[i+4] == y1[j+4] && x1[i+5] == y1[j+5] && x1[i+6] == y1[j+6] && x1[i+7] == y1[j+7]){
              points->addData(i,j);
              points->setLineStyle(QCPGraph::lsNone);
              points->setScatterStyle(QCPScatterStyle::ssPlus);
              ui->plot->xAxis2->setLabel(ui->comboBox->currentText());
              ui->plot->yAxis->setLabel(ui->comboBox_2->currentText());
              i+=7;
              j+=7;
            }
        }
    }
    plot();
}

void MainWindow::on_pushButton_13_clicked()
{
    ui->plot->savePng("graph.png");
    QImage image ("graph.png");


        QPrinter printer(QPrinter::HighResolution);
          printer.setOrientation(QPrinter::Portrait);
          printer.setOutputFormat(QPrinter::PdfFormat);
          printer.setPaperSize(QPrinter::A4);
          QString fileName = QFileDialog::getSaveFileName(nullptr,"Zapisz raport", "c:\\", "pdf (*.pdf)");
          printer.setOutputFileName(fileName);

          QPainter painter;
          painter.begin(&printer);
          QFile S1("pdf.txt");
          int i =200;
          if(S1.open(QIODevice::ReadOnly | QIODevice::Text)){
                  QTextStream in(&S1);
                  while(!in.atEnd()){
                  painter.drawText( 200,i , in.readLine(70) );
                  i+=200;
                  }

                  S1.close();
          QPoint pointer(200,i+600);
          painter.drawImage(pointer,image.scaled(8500,5000));
          painter.end();
          }
          QDesktopServices::openUrl(QUrl(fileName));
   S1.remove();
   QFile x("graph.png");
   QFile x1("seq1.txt");
   QFile x2("seq2.txt");
   QFile x3("seq3.txt");
   QFile x4("seq4.txt");
   x.remove();
   x1.remove();
   x2.remove();
   x3.remove();
   x4.remove();
}

void MainWindow::on_actionhelp_2_triggered()
{
     QProcess::execute("assistant -collectionFile C:\\Users\\Trev\\Desktop\\Pracownia\\pracownia\\help.qhc");

}
