#include "slicedialog.h"
#include "ui_slicedialog.h"
#include "mainwindow.h"

SliceDialog::SliceDialog(QWidget *parent, int64_t duration) :
    QDialog(parent),
    ui(new Ui::SliceDialog)
{
    ui->setupUi(this);
    fileDuration = duration;
}

SliceDialog::~SliceDialog()
{
    delete ui;
}

void SliceDialog::on_pushButton_clicked()
{
    foreach (QLineEdit* curEdit, this->findChildren<QLineEdit*>())
    {
        bool ok = false;
        int number = 0;
        number = curEdit->text().toInt(&ok);
        if(!ok or number < 0)
        {
            QMessageBox::warning(this, "提示", "时间格式出错啦！");
            return;
        }
        if(curEdit->objectName().contains("Hour"))
        {
            ;//小时只要是个数字即可
        }
        else if(curEdit->objectName().contains("Minute"))
        {
            if(!ok or number >= 60)
            {
                QMessageBox::warning(this, "提示", "时间格式出错啦！");
                return;
            }
        }
        else if(curEdit->objectName().contains("Second"))
        {
            if(!ok or number >= 60)
            {
                QMessageBox::warning(this, "提示", "时间格式出错啦！");
                return;
            }
        }
        else if(curEdit->objectName().contains("Millisec"))
        {
            ;//毫秒只要是个数字即可
        }
    }
    QString startTimeStr = "";
    QString endTimeStr = "";
    startTimeStr += ui->startHourEdit->text() + ":";
    startTimeStr += QString("%1").arg(ui->startMinuteEdit->text(), 2, QLatin1Char('0')) + ":";
    startTimeStr += QString("%1").arg(ui->startSecondEdit->text(), 2, QLatin1Char('0')) + ".";
    startTimeStr += QString("%1").arg(ui->startMillisecEdit->text().left(3), -3, QLatin1Char('0'));
    endTimeStr   += ui->endHourEdit->text() + ":";
    endTimeStr   += QString("%1").arg(ui->endMinuteEdit->text(), 2, QLatin1Char('0')) + ":";
    endTimeStr   += QString("%1").arg(ui->endSecondEdit->text(), 2, QLatin1Char('0')) + ".";
    endTimeStr   += QString("%1").arg(ui->endMillisecEdit->text().left(3), -3, QLatin1Char('0'));

    if(str2duration(startTimeStr) == -1 or str2duration(endTimeStr) == -1)
    {
        QMessageBox::warning(this, "提示", "时间格式出错啦！");
        return;
    }
    if(str2duration(endTimeStr) > fileDuration)
    {
        QMessageBox::warning(this, "提示", "结束时间应该不能超过文件总时长哦！");
        return;
    }
    if(str2duration(startTimeStr) >= str2duration(endTimeStr))
    {
        QMessageBox::warning(this, "提示", "开始时间应该比结束时间更小哦！");
        return;
    }

    emit addSlice(startTimeStr, endTimeStr);
    this->close();
}

