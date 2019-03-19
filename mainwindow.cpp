#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDateTime>
#include <resetpwd.h>
#include <QSqlRecord>
#include <QColor>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //读取用户登录时候输入的用户名和密码
    loadcfg();
    //链接数据库
    bool bl = connectDB();
    if(bl == false){
        QMessageBox::critical(this,"错误","链接数据库失败！");
        return;
    }

    //出售车辆
    initSaleCar();

    //设置当前默认页面为车辆管理的默认页面
    ui->stackedWidget->setCurrentIndex(0);
    //点击车辆管理菜单项的时候，跳转到车辆管理的页面
    connect(ui->carManager,&QAction::triggered,this,[=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    //点击销售统计的时候，页面跳转到销售统计
    connect(ui->sell_count,&QAction::triggered,this,[=](){
        ui->stackedWidget->setCurrentIndex(1);
    });

    //新车入库
    initNewCar();

    //修改密码
    initResetPwd();

    //点击退出的时候，销毁窗口
    connect(ui->quit,&QAction::triggered,this,[=](){
        this->close();
    });

    //统计界面的实现
    initCount();





}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::ReadInit(QString key, QString &value){
    value.clear();
    QString path = "login.ini";
    QSettings *config = new QSettings(path,QSettings::IniFormat);
    //读取配置信息
    QVariant variant = config->value(QString("config/")+key);
    value=variant.value<QString>();
    delete config;
}
//从配置文件中读取用户名和密码
void MainWindow::loadcfg(){
    ReadInit("username",username);
    ReadInit("password",passwd);
    qDebug() << username << passwd;

}

//链接数据库
bool MainWindow::connectDB(){
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

void MainWindow::initSaleCar(){
    //设置厂商
    ui->factory->addItem("请选择厂商");
    QSqlQuery query;
    QString sql = "select name from factory";
    query.exec(sql);
    while (query.next()) {
        ui->factory->addItem(query.value(0).toString());
    }
    fatoryChange("请选择厂商");

    //brand随着厂商的改变而改变
    connect(ui->factory,&QComboBox::currentTextChanged,this,&MainWindow::fatoryChange);
    //价格随着brand的改变而改变
    connect(ui->brand,&QComboBox::currentTextChanged,this,&MainWindow::brandChange);

    //数量的变化，价格会随之变化
    void (QSpinBox::*sigNumChanged)(int) = &QSpinBox::valueChanged;
    connect(ui->num,sigNumChanged,this,&MainWindow::numChange);



}

void MainWindow::fatoryChange(const QString str){
    ui->brand->clear();
    if(str == "请选择厂商"){
        ui->price->clear();
        ui->num->setValue(0);
        ui->last->setText("0");
        ui->sum_price->setEnabled(false);
        ui->ok->setEnabled(false);

    }else {
        //查询对应厂商的品牌
        QSqlQuery query;
        QString sql = QString("select name from brand where fname = '%1'").arg(str);
        query.exec(sql);
        while (query.next()) {
            ui->brand->addItem(query.value(0).toString());
        }
    }
}

void MainWindow::brandChange(const QString str){
    QSqlQuery query;
    QString sql = QString("select price,last from brand where name = '%1' and fname = '%2'").arg(str).arg(ui->factory->currentText());
    query.exec(sql);
    while (query.next()) {

        ui->price->setText(query.value(0).toString());
        ui->last->setText(query.value(1).toString());
        ui->num->setValue(0);
        ui->sum_price->setEnabled(true);
        ui->sum_price->setText("0");
    }

}

void MainWindow::numChange(int i){
    //总价格
    int price = ui->price->text().toInt();
    int s_price = price * i;
//    qDebug() << s_price;
    ui->sum_price->setText(QString("%1").arg(s_price));
    //设置确定按钮可以使用
    if(s_price > 0){
        ui->ok->setEnabled(true);
    }else {
        ui->ok->setEnabled(false);
    }
}

void MainWindow::on_ok_clicked()
{
    //点击确定的时候
    //更新数据库
    int sale_num = ui->num->value();
    if(sale_num > ui->last->text().toInt()){
        QMessageBox::warning(this,"警告","当前库存不足");
        return;
    }
    //卖完的剩余数量
    int last = ui->last->text().toInt() - sale_num;
    ui->last->setText(QString("%1").arg(last));
    QSqlQuery query;
    QString sql = QString("select sell from brand where name = '%1' and fname = '%2'").arg(ui->brand->currentText()).arg(ui->factory->currentText());
    query.exec(sql);
    while (query.next()) {
        int sell = query.value(0).toInt() + sale_num;
        sql = QString("update brand set sell = %1,last = %2 where name = '%3' and fname = '%4'").arg(sell).arg(last)
                .arg(ui->brand->currentText()).arg(ui->factory->currentText());
        bool bl = query.exec(sql);
        if(bl){
            QDateTime current_time = QDateTime::currentDateTime();
            QString strCurTime = current_time.toString("yyyy-MM-DD hh:mm:ss");
            QString str = QString("%1 卖出了%2--%3,单价：%4,数量:%5,总价：%6").arg(strCurTime).arg(ui->factory->currentText()).arg(ui->brand->currentText())
                    .arg(ui->price->text()).arg(ui->num->value()).arg(ui->sum_price->text());
            ui->listWidget->addItem(str);
            QSqlDatabase::database().commit();
        }else {
            QSqlDatabase::database().rollback();
        }
    }

}

void MainWindow::on_cancel_clicked()
{
    fatoryChange("请选择厂商");
}

void MainWindow::initNewCar(){
    //实例化model
    model = new QSqlTableModel(this);
    //将model设置到tableView视图中
    ui->tableView->setModel(model);
    //设置model的提交方式为手动提交
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    //在model中设置数据库表
    model->setTable("brand");
    model->select();

    //在model中添加新纪录
    QSqlRecord record = model->record();  //获取model空记录
    int rows = model->rowCount();   //获取model的行数
    model->insertRecord(rows,record);

   //点击提交按钮的时候，将在视图中修改的数据，手动提交到数据库中
    //点击revert按钮的时候，撤回在视图中修改的数据
}

void MainWindow::on_commit_clicked()
{

    model->submitAll();
}

void MainWindow::on_revert_clicked()
{
    model->revertAll();
    model->submitAll();

}

void MainWindow::initResetPwd(){
    connect(ui->reset_pwd,&QAction::triggered,this,[=](){
        rpwd = new reSetPwd(this);
        rpwd->show();
        connect(rpwd,&reSetPwd::sigpwd,this,&MainWindow::slotpwd);
    });
}

void MainWindow::slotpwd(QString o_pwd, QString n_pwd){
    if(o_pwd == passwd){
        QSqlQuery query;
        QString sql = QString("update user set pwd = '%2' where user = '%1'").arg(username).arg(n_pwd);
        query.exec(sql);
        QSqlDatabase::database().commit();
        passwd = n_pwd;
        qDebug() << passwd;
        delete rpwd;
    }else {
        QMessageBox::critical(this,"错误","您输入的原密码错误");
        return;

    }
}

void MainWindow::initCount(){
    //设置厂商
    ui->changshang->addItem("请选择厂商");
    QSqlQuery query;
    QString sql = "select name from factory";
    query.exec(sql);
    while (query.next()) {
        ui->changshang->addItem(query.value(0).toString());
    }
    //brand随着厂商的改变而改变
    connect(ui->changshang,&QComboBox::currentTextChanged,this,&MainWindow::changshangChange);
    connect(ui->pinpai,&QComboBox::currentTextChanged,this,&MainWindow::pinpaiChange);


}

void MainWindow::changshangChange(const QString str){

    //品牌随着厂商的改变发生相应的改变
    //厂商改变，柱形图发生相应的改变
    ui->wg_histogram->Clear();
    ui->pinpai->clear();
    if(str != "请选择厂商"){

        ui->wg_histogram->SetMaxValue(300);
        QSqlQuery query;
        QString sql = QString("select name,sell from brand where fname = '%1'").arg(str);
        query.exec(sql);
        while (query.next()) {
            QString h_name = query.value(0).toString();
            int h_sell = query.value(1).toInt();
            ui->pinpai->addItem(h_name);
            //初始化一种颜色
            QColor color(qrand()%255,qrand()%255,qrand()%255);
            ui->wg_histogram->AddItem(h_name,h_sell,color);
        }

    }
    ui->wg_histogram->update();


}

void MainWindow::pinpaiChange(const QString str){
    QSqlQuery query;
    QVector<SectorInfo> vc;
    QString sql = QString("select sell,last from brand where fname = '%1' and name = '%2'").arg(ui->changshang->currentText()).arg(str);
    query.exec(sql);
    while (query.next()) {
        SectorInfo info;
        float sell = query.value(0).toInt();
        float last = query.value(1).toInt();
        float sum = sell + last;
        info.description = "已出售";
        info.percent = sell/sum * 100;
        vc.push_back(info);
        info.description = "剩余";
        info.percent = last/sum * 100;
        vc.push_back(info);
    }
    ui->wg_pie->setData(vc);
}
