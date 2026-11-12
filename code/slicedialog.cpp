#include "slicedialog.h"
#include "ui_slicedialog.h"

SliceDialog::SliceDialog(QWidget *parent, int64_t duration) :
    QDialog(parent),
    ui(new Ui::SliceDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(420, 260);
    setStyleSheet(R"(
        QDialog {
            background: #111827;
        }
        QLabel {
            color: #e5e7eb;
            font-size: 14px;
        }
        QLabel#label {
            font-size: 18px;
            font-weight: 700;
        }
        QLineEdit {
            border: 1px solid #4b5563;
            background: #1f2937;
            padding: 6px 8px;
            color: #f3f4f6;
        }
        QLineEdit:focus {
            border: 1px solid #60a5fa;
        }
        QPushButton {
            background: #2563eb;
            border: 1px solid #2563eb;
            color: white;
            font-size: 14px;
            font-weight: 600;
            padding: 8px 18px;
        }
        QPushButton:hover {
            background: #1d4ed8;
            border-color: #1d4ed8;
        }
        QPushButton:pressed {
            background: #1e40af;
            border-color: #1e40af;
        }
    )");

    ui->pushButton->setStyleSheet("font-size: 12px; padding: 2px 8px; min-width: 0;");
    ui->pushButton->setFixedSize(84, 30);

    foreach (QLineEdit *lineEdit, this->findChildren<QLineEdit *>())
    {
        lineEdit->setAlignment(Qt::AlignCenter);
        lineEdit->setPlaceholderText("0");
    }

    fileDuration = duration;
}

SliceDialog::~SliceDialog()
{
    delete ui;
}

void SliceDialog::on_pushButton_clicked()
{
    foreach (QLineEdit *curEdit, this->findChildren<QLineEdit *>())
    {
        bool ok = false;
        int number = 0;
        number = curEdit->text().toInt(&ok);
        if (!ok or number < 0)
        {
            QMessageBox::warning(nullptr, "提示", "时间格式出错啦！");
            return;
        }
        if (curEdit->objectName().contains("Hour"))
        {
            ;//小时只要是个数字即可
        }
        else if (curEdit->objectName().contains("Minute"))
        {
            if (!ok or number >= 60)
            {
                QMessageBox::warning(nullptr, "提示", "时间格式出错啦！");
                return;
            }
        }
        else if (curEdit->objectName().contains("Second"))
        {
            if (!ok or number >= 60)
            {
                QMessageBox::warning(nullptr, "提示", "时间格式出错啦！");
                return;
            }
        }
        else if (curEdit->objectName().contains("Millisec"))
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
    endTimeStr += ui->endHourEdit->text() + ":";
    endTimeStr += QString("%1").arg(ui->endMinuteEdit->text(), 2, QLatin1Char('0')) + ":";
    endTimeStr += QString("%1").arg(ui->endSecondEdit->text(), 2, QLatin1Char('0')) + ".";
    endTimeStr += QString("%1").arg(ui->endMillisecEdit->text().left(3), -3, QLatin1Char('0'));

    if (str2duration(startTimeStr) == -1 or str2duration(endTimeStr) == -1)
    {
        QMessageBox::warning(nullptr, "提示", "时间格式出错啦！");
        return;
    }
    if (str2duration(endTimeStr) > fileDuration)
    {
        QMessageBox::warning(nullptr, "提示", "结束时间应该不能超过文件总时长哦！");
        return;
    }
    if (str2duration(startTimeStr) >= str2duration(endTimeStr))
    {
        QMessageBox::warning(nullptr, "提示", "开始时间应该比结束时间更小哦！");
        return;
    }

    emit addSlice(startTimeStr, endTimeStr);
    this->close();
}

