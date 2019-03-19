#include "login.h"
#include "ui_login.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSettings>

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    bool bl = connectDB();
    if(bl == false){
        return;
    }


}

login::~login()
{
    delete ui;
}

bool login::connectDB(){
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    //设置链接数据库的主机名，数据库名，用户名和密码
    m_db.setHostName("127.0.0.1");
    m_db.setDatabaseName("cardb");
    m_db.setUserName("djx");
    m_db.setPassword("111");
    if(m_db.open() == false){
        QMessageBox::critical(this,"ERROR","链接数据库失败");
        return false;
    }
    return true;
}
void login::on_ulogin_clicked()
{
    name = ui->name->text();
    pwd = ui->pwd->text();
    if(name.isEmpty()){
        QMessageBox::warning(this,"WARNING","用户名不能为空");
        return;
    }
    if(name.isEmpty()){
        QMessageBox::warning(this,"WARNING","密码不能为空");
        return;
    }
    bool flag = false;
    QSqlQuery query;
    QString sql = QString("select pwd from user where user = '%1'").arg(name);
    query.exec(sql);
    while(query.next()){
        if(pwd == query.value(0).toString()){
            flag = true;
            break;
        }
    }
    if(flag){
        WriteInit("username",name);
        WriteInit("password",pwd);
        m_db.close();
        QDialog::accept();
    }else {
        QMessageBox::critical(this,"ERROR","用户名或密码错误");
        return;
    }

}

void login::on_quit_clicked()
{
    QDialog::reject();
}

//将用户名和密码写入到配置文件中
void login::WriteInit(QString key, QString value){
    QString path="login.ini";
    QSettings *config = new QSettings(path,QSettings::IniFormat);
    QVariant variant;
    variant.setValue(value);
    //将信息写入到配置文件中
    config->beginGroup("config");
    config->setValue(key,variant);
    config->endGroup();
    delete config;
}
