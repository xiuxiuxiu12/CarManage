#include "resetpwd.h"
#include "ui_resetpwd.h"
#include <QMessageBox>

reSetPwd::reSetPwd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reSetPwd)
{
    ui->setupUi(this);

}

reSetPwd::~reSetPwd()
{
    delete ui;
}

void reSetPwd::on_rpwd_ok_clicked()
{
    o_pwd = ui->o_pwd->text();
    n_pwd = ui->n_pwd->text();
    QString rn_pwd = ui->rn_pwd->text();
    if(o_pwd.isEmpty() || n_pwd.isEmpty() || rn_pwd.isEmpty()){
        QMessageBox::critical(this,"错误","密码不能为空");
        return;
    }
    if(n_pwd != rn_pwd){
        QMessageBox::critical(this,"错误","两次输入的新密码不一致");
        return;
    }
    emit sigpwd(o_pwd,n_pwd);
}

void reSetPwd::on_rpwd_cancel_clicked()
{

    this->close();
}
