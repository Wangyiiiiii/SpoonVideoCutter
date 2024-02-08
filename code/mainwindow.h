#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QProcess>
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
}

#include <slicedialog.h>
#include "utils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void on_addSlice(QString, QString);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void delete_handler();

private:
    Ui::MainWindow *ui;
    AVFormatContext* currentFmtContext;
    int64_t numOfRows;
    QList<QPair<int64_t, int64_t>> timeSlices;
    QList<QPushButton*> deleteBtns;
    QString srcStr;
    QString dstStr;

    void setTimeSliceTable();
    void videoProcess();
    void resetState();
};
#endif // MAINWINDOW_H
