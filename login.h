#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();
    bool connectDB();
protected:
    void WriteInit(QString key,QString value);

private slots:
    void on_ulogin_clicked();

    void on_quit_clicked();

private:
    Ui::login *ui;
    QSqlDatabase m_db;
    QString name;
    QString pwd;
};

#endif // LOGIN_H
