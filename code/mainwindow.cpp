#include "mainwindow.h"
#include "ui_mainwindow.h"

const int VERYLONGTIME = 2147483647;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/img/Resources/spoon.png"));
    ui->stackedWidget->setCurrentIndex(0);
    ui->tableWidget->setColumnWidth(0, 650);
    ui->tableWidget->setColumnWidth(1, 50);
    currentFmtContext = NULL;
    numOfRows = 0;
}

MainWindow::~MainWindow()
{
    avformat_close_input(&currentFmtContext);
    avformat_free_context(currentFmtContext);
    ui->tableWidget->clear();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString filter = "视频文件(*.mp4;*.mkv);;全部文件(*.*)";
    srcStr = QFileDialog::getOpenFileName(this, "请选择视频文件", "", filter);

    if(srcStr == "")
    {
        return;
    }

    currentFmtContext = avformat_alloc_context();
    processErrorMsg(avformat_open_input(&currentFmtContext, srcStr.toUtf8().data(), NULL, NULL));
    processErrorMsg(avformat_find_stream_info(currentFmtContext, NULL));

    ui->label->setText(srcStr.right(srcStr.size() - srcStr.lastIndexOf('/') - 1));
    ui->label_2->setText("视频时长:" + duration2str(currentFmtContext->duration));
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_2_clicked()
{
    SliceDialog sliceDlg(this, currentFmtContext->duration);
    connect(&sliceDlg, &SliceDialog::addSlice, this, &MainWindow::on_addSlice);
    sliceDlg.exec();
}

void MainWindow::delete_handler()
{
    QPushButton* deleteBtn = (QPushButton*)sender();
    for(int i=deleteBtn->toolTip().toInt()+1; i<deleteBtns.size(); i++)
    {
        deleteBtns.at(i)->setToolTip(QString::number(deleteBtns.at(i)->toolTip().toInt()-1));
    }
    ui->tableWidget->removeCellWidget(deleteBtn->toolTip().toInt(), 1);
    ui->tableWidget->removeRow(deleteBtn->toolTip().toInt());
    deleteBtns.removeAt(deleteBtn->toolTip().toInt());
    numOfRows--;
    timeSlices.removeAt(deleteBtn->toolTip().toInt());
    delete deleteBtn;
}

void MainWindow::setTimeSliceTable()
{
    ui->tableWidget->clearContents();
    deleteBtns.clear();
    ui->tableWidget->setRowCount(50 + numOfRows);
    for(int i=0; i<timeSlices.count(); i++)
    {
        QString tempText = "";
        tempText = duration2str(timeSlices.at(i).first) + " - " + duration2str(timeSlices.at(i).second);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(tempText));
        QPushButton* deleteBtn = new QPushButton(this);
        deleteBtn->setText("删除");
        deleteBtn->setToolTip(QString::number(i));
        deleteBtn->setToolTipDuration(2000);
        deleteBtns << deleteBtn;
        ui->tableWidget->setCellWidget(i, 1, deleteBtn);
        connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::delete_handler);
    }
}

void MainWindow::videoProcess()
{
    QString srcSuffix = srcStr.last(srcStr.length() - srcStr.lastIndexOf("."));
    QString srcPath = srcStr.left(srcStr.lastIndexOf('/'));
    QString dstPath = dstStr.left(dstStr.lastIndexOf('/'));
    QProcess *pFFmpeg = new QProcess(this);
    QStringList cutCommandList, concatCommandList, segmentNames;
    QString ffmpeg_dir = QDir::currentPath() + "/ffmpeg.exe";
//    QString ffmpeg_dir = "ffmpeg";
    cutCommandList << ffmpeg_dir << "-ss" << "<startTime>"
                   << "-to" << "<endTime>" << "-accurate_seek"
                   << "-i" << srcStr << "-avoid_negative_ts" << "1"
                   << "-c" << "copy" << "<segment>" << "-y";
    concatCommandList << ffmpeg_dir << "-f" << "concat"
                      << "-safe" << "0" << "-i" << "<segList>"
                      << "-c" << "copy" << dstStr << "-y";
    ui->stackedWidget->setCurrentIndex(3);

    //-------------------------step 1:cut all the video segments-------------------------
    for(int i=0; i<timeSlices.length(); i++)
    {
        QString segmentName = "segment" + QString::number(i) + srcSuffix;
        segmentName = dstPath + '/' + segmentName;
        segmentNames.append(segmentName);
        cutCommandList[2] = duration2str(timeSlices.at(i).first);
        cutCommandList[4] = duration2str(timeSlices.at(i).second);
        cutCommandList[12] = segmentName;
        qDebug().noquote() << cutCommandList.join(' ');
        QStringList cmdList;
        cmdList << "/c" << cutCommandList.join(" ");
        pFFmpeg->start("cmd", cmdList);
        bool state = pFFmpeg->waitForStarted(VERYLONGTIME);
        if(!state)
        {
            QMessageBox::warning(this, "提示", "剪切片段失败啦");
            resetState();
            return;
        }
        state = pFFmpeg->waitForFinished(VERYLONGTIME);
        if(!state)
        {
            QMessageBox::warning(this, "提示", "剪切片段失败啦");
            resetState();
            return;
        }
    }

    //-------------------------step 2:put the segments together-------------------------
    QFile f(srcPath + '/' + "segList.txt");
    bool ok = f.open(QIODevice::ReadWrite);
    if(ok)
    {
        for(int i=0; i<segmentNames.length(); i++)
        {
            QString content;
            content = "file \'" + segmentNames.at(i) + "\'";
            f.write(content.toUtf8());
            f.write("\r");
        }
        f.close();
    }
    else
    {
        QMessageBox::warning(this, "提示", "合并视频片段错误：建立合并列表时出错");
        return;
    }
    QStringList cmdList;
    concatCommandList[6] = srcPath + '/' + "segList.txt";
    cmdList << "/c" << concatCommandList.join(" ");
    pFFmpeg->start("cmd", cmdList);
    bool state = pFFmpeg->waitForStarted(VERYLONGTIME);
    if(!state)
    {
        QMessageBox::warning(this, "提示", "合并片段失败啦");
        resetState();
        return;
    }
    state = pFFmpeg->waitForFinished(VERYLONGTIME);
    if(!state)
    {
        QMessageBox::warning(this, "提示", "合并片段失败啦");
        resetState();
        return;
    }

    //--------------------------step3:clean up--------------------------
    delete pFFmpeg;
    pFFmpeg = nullptr;
    QFile::remove(srcPath + '/' + "segList.txt");
    for(int i=0; i<segmentNames.size(); i++)
    {
        QFile::remove(segmentNames.at(i));
    }
    QMessageBox::information(this, "提示", "剪切完毕~");
    resetState();
    return;
}

void MainWindow::resetState()
{
    avformat_close_input(&currentFmtContext);
    avformat_free_context(currentFmtContext);
    currentFmtContext = NULL;
    numOfRows = 0;
    timeSlices.clear();
    srcStr = "";
    dstStr = "";
    foreach(QPushButton* _btn, deleteBtns)
    {
        delete _btn;
    }
    deleteBtns.clear();
    ui->tableWidget->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_addSlice(QString startTimeStr, QString endTimeStr)
{
    int64_t startTimeInt = str2duration(startTimeStr);
    int64_t endTimeInt   = str2duration(endTimeStr);
    if(startTimeInt == -1 or endTimeInt == -1)
    {
        QMessageBox::warning(this, "提示", "时间格式出错啦");
        return;
    }
    QPair<int64_t, int64_t> curSlice;
    curSlice.first = startTimeInt;
    curSlice.second = endTimeInt;
    for(int i=0; i<timeSlices.count(); i++)
    {
        if(hasOverlap(timeSlices.at(i), curSlice))
        {
            QMessageBox::warning(this, "提示", "当前添加的片段与已添加片段存在重复~");
            return;
        }
    }
    timeSlices.append(curSlice);
    numOfRows++;
    std::sort(timeSlices.begin(), timeSlices.end(), compareSlice);
    setTimeSliceTable();
    return;
}

void MainWindow::on_pushButton_3_clicked()
{
    if(0 == timeSlices.length())
    {
        QMessageBox::warning(this, "提示", "最少也要有一个视频片段哦~");
        return;
    }
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pushButton_4_clicked()
{
    QString srcSuffix = srcStr.last(srcStr.length() - srcStr.lastIndexOf("."));
    QString filter = "视频文件(*" + srcSuffix + ");";
    dstStr = QFileDialog::getSaveFileName(this, "选择输出目录", "", filter);
    if(dstStr == "")
    {
        QMessageBox::warning(this, "提示", "保存目录出错啦");
        return;
    }
    videoProcess();
}
