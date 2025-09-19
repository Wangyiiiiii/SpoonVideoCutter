#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDragEnterEvent>
#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QMimeData>
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
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

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
    void loadVideo(); //当用户完成以下动作之一后调用
                      //1.拖拽文件
                      //2.通过文件选择框选择文件
};
#endif // MAINWINDOW_H
