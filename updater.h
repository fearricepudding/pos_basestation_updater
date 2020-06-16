#ifndef UPDATER_H
#define UPDATER_H

#include <QMainWindow>
#include <libssh/libsshpp.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class updater; }
QT_END_NAMESPACE

class updater : public QMainWindow
{
    Q_OBJECT

public:
    updater(QWidget *parent = nullptr);
    ~updater();
    int verify_knownhost(ssh_session);

private:
    Ui::updater *ui;


public slots:
    void FindFile();
    bool startUpdate();
};
#endif // UPDATER_H
