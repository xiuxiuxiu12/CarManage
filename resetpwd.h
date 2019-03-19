#ifndef RESETPWD_H
#define RESETPWD_H

#include <QDialog>

namespace Ui {
class reSetPwd;
}

class reSetPwd : public QDialog
{
    Q_OBJECT

public:
    explicit reSetPwd(QWidget *parent = 0);
    ~reSetPwd();

private:
    Ui::reSetPwd *ui;
    QString o_pwd;
    QString n_pwd;
signals:
    void sigpwd(QString o_pwd,QString n_pwd);
private slots:
    void on_rpwd_ok_clicked();
    void on_rpwd_cancel_clicked();
};

#endif // RESETPWD_H
