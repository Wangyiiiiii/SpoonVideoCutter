#ifndef SLICEDIALOG_H
#define SLICEDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QString>
#include "utils.h"

namespace Ui {
class SliceDialog;
}

class SliceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SliceDialog(QWidget *parent = nullptr, int64_t duration = 0);
    ~SliceDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SliceDialog *ui;
    int fileDuration;
signals:
    void addSlice(QString startTime, QString endTime);
};

#endif // SLICEDIALOG_H
