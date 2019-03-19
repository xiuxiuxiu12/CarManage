#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include "resetpwd.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void ReadInit(QString key,QString &value);
    void loadcfg();
    bool connectDB();
    void initSaleCar();
    void fatoryChange(const QString str);
    void brandChange(const QString str);

    void initNewCar();

    void numChange(int i);

    void initResetPwd();

    void initCount();

    void changshangChange(const QString str);

    void pinpaiChange(const QString str);


private slots:
    void on_ok_clicked();

    void on_cancel_clicked();

    void on_commit_clicked();

    void on_revert_clicked();

    void slotpwd(QString o_pwd,QString n_pwd);

private:
    Ui::MainWindow *ui;
    QSqlDatabase m_db;
    QString username;
    QString passwd;
    QSqlTableModel *model;
    reSetPwd *rpwd;
};

#endif // MAINWINDOW_H
