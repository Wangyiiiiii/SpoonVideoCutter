#include "utils.h"

QString duration2str(int64_t duration)
{
    int hour = 0;
    int min = 0;
    int sec = 0;
    int msec = 0;

    duration = duration / AV_TIME_BASE * 1000; //Convert duration to msec
    hour = duration / (60 * 60 * 1000);
    min = (duration - hour * 60 * 60 * 1000) / (60 * 1000);
    sec = (duration - hour * 60 * 60 * 1000 - min * 60 * 1000) / (1000);
    msec = duration - hour * 60 * 60 * 1000 - min * 60 * 1000 - sec * 1000;

    QString rVal = "";
    rVal += QString::number(hour) + ":";
    rVal += QString("%1").arg(min, 2, 10, QLatin1Char('0')) + ":";
    rVal += QString("%1").arg(sec, 2, 10, QLatin1Char('0')) + ".";
    rVal += QString("%1").arg(QString::number(msec).left(3), -3, QLatin1Char('0'));

    return rVal;
}

int64_t str2duration(QString timeStr)
{
    int64_t rVal = 0;
    QRegularExpression reTimeStr(R"(\d{1,}:[0-5][0-9]:[0-5][0-9].\d{3})");
    QRegularExpressionMatch match = reTimeStr.match(timeStr);

    if(!match.hasMatch())
    {
        rVal = -1;
        return rVal;
    }
    QStringList timePartList = timeStr.split(QChar(':'));
    rVal += timePartList.at(0).toInt() * 60 * 60 * 1000;
    rVal += timePartList.at(1).toInt() * 60 * 1000;
    rVal += timePartList.at(2).toDouble() * 1000;

    return rVal / 1000 * AV_TIME_BASE;
}

bool hasOverlap(QPair<int64_t, int64_t> segment1, QPair<int64_t, int64_t> segment2)
{
    bool bRet = false;
    QList<int> tempList;
    tempList << segment1.first << segment1.second << segment2.first << segment2.second;
    int64_t min = 0;
    int64_t max = 0;
    min = *(std::min_element(tempList.begin(), tempList.end()));
    max = *(std::max_element(tempList.begin(), tempList.end()));
    if((max - min) < (segment1.second-segment1.first)+(segment2.second-segment2.first))
    {
        bRet = true;
    }
    else
    {
        bRet = false;
    }
    return bRet;
}

bool compareSlice(QPair<int64_t, int64_t> slice1, QPair<int64_t, int64_t> slice2)
{
    return (slice1.first < slice2.first);
}

void processErrorMsg(int ret)
{
    if (0 > ret)
    {
        char errMsg[1024];
        av_strerror(ret, errMsg, 1024);
        QMessageBox* errMsgBox = new QMessageBox();
        errMsgBox->setText(errMsg);
        errMsgBox->exec();
        delete errMsgBox;
        exit(0);
    }
}
