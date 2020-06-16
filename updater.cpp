#include "updater.h"
#include "./ui_updater.h"
#include <QtWidgets>
#include <libssh/libsshpp.hpp>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

QPushButton *findFileButton;
QPushButton *updateButton;
QLineEdit *filePath;
QLabel *statusUpdate;
QString package;


updater::updater(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::updater)
{
    ui->setupUi(this);

    findFileButton = ui->findFileButton;
    filePath = ui->filePath;
    updateButton = ui->startButton;
    statusUpdate = ui->statusUpdate;

    this->setFixedSize(650, 155);


    connect(findFileButton, SIGNAL (released()), this, SLOT (FindFile()));
    connect(updateButton, SIGNAL (released()), this, SLOT (startUpdate()));

}

void updater::FindFile(){
    findFileButton->setText("Open File");
    package = QFileDialog::getOpenFileName();
    filePath->setText(package);
}

updater::~updater()
{
    delete ui;
}


bool updater::startUpdate(){
    statusUpdate->setText("Starting update... do NOT turn off the BaseStation!");
    printf("Staring update...");


    ssh_session my_ssh_session;
    int rc;
    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
        statusUpdate->setText("Failed to start ssh session");

	const void * username = "pi";
	const char * password = "raspberry";
	const void * hostname = "192.168.1.89";

	ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, (const void *)username);
	ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, hostname);

    rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error connecting to host: %s\n",
                ssh_get_error(my_ssh_session));
        statusUpdate->setText(ssh_get_error(my_ssh_session));
        return false;
    }

    rc = ssh_userauth_password(my_ssh_session, NULL, password);
    if (rc == SSH_AUTH_ERROR)
    {
        fprintf(stderr, "Authentication failed: %s\n",
                ssh_get_error(my_ssh_session));
        statusUpdate->setText(ssh_get_error(my_ssh_session));
    }


    // SCP START
    statusUpdate->setText("Uploading package");

    ssh_scp scp;
    scp = ssh_scp_new(my_ssh_session, SSH_SCP_RECURSIVE, "./");
    if (scp == NULL)
    {
        fprintf(stderr, "Error allocating scp session: %s\n",
                ssh_get_error(my_ssh_session));
        statusUpdate->setText(ssh_get_error(my_ssh_session));
    }
    rc = ssh_scp_init(scp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing scp session: %s\n",
                ssh_get_error(my_ssh_session));
        ssh_scp_free(scp);
        statusUpdate->setText(ssh_get_error(my_ssh_session));
        return rc;
    }

    rc = ssh_scp_push_directory(scp, "BASESTATION_UPDATE", 0700);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't create remote directory: %s\n",
                ssh_get_error(my_ssh_session));
        return rc;
    }

    int size = 0;
    QFile myFile(package);
    if (myFile.open(QIODevice::ReadOnly)){
        size = myFile.size();  //when file does open.
    }

    rc = ssh_scp_push_file(scp, package.toStdString().c_str(), size, 0700);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't open remote file: %s\n",
                ssh_get_error(my_ssh_session));
        statusUpdate->setText(ssh_get_error(my_ssh_session));
        return rc;
    }

    rc = ssh_scp_write(scp, myFile.readAll() , size);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't write to remote file: %s\n",
                ssh_get_error(my_ssh_session));
        statusUpdate->setText(ssh_get_error(my_ssh_session));
        return rc;
    }

    myFile.close();
    ssh_scp_close(scp);
    ssh_scp_free(scp);


    ssh_channel channel;

    channel = ssh_channel_new(my_ssh_session);
    if (channel == NULL) return SSH_ERROR;

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        statusUpdate->setText(ssh_get_error(my_ssh_session));
        return rc;
    }

    statusUpdate->setText("Telling BaseStation to update");



    char buffer[1000];
    int nbytes;


    rc = ssh_channel_request_exec(channel, "unzip -o ~/BASESTATION_UPDATE/Till-master.zip -d ~/BASESTATION_UPDATE && sh ~/BASESTATION_UPDATE/Till-master/update.sh");
    if (rc != SSH_OK)
    {
        statusUpdate->setText(ssh_get_error(my_ssh_session));
        return rc;
    }

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0)
    {

        if (fwrite(buffer, 1, nbytes, stdout) != nbytes)
        {
            statusUpdate->setText(ssh_get_error(my_ssh_session));
            fprintf(stderr, "SSH ERROR: %s", ssh_get_error(my_ssh_session));
            return SSH_ERROR;
        }
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
        //fprintf(stdout, "%s", buffer);
    }


    // Build basestation
    fprintf(stdout, "Sending build payload");



    statusUpdate->setText("Done. Base station will now update and reboot");

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);




    // SCP END

    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    return true;

}
