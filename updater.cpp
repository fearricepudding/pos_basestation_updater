#include "updater.h"
#include "./ui_updater.h"
#include <QtWidgets>
#include <libssh/libsshpp.hpp>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

QPushButton *findFileButton;
QPushButton *updateButton;
QLineEdit *filePath;
QLabel *statusUpdate;
QString package;
QLineEdit *hostnameField;
QLineEdit *usernameField;
QLineEdit *passwordField;

updater::updater(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::updater){
    ui->setupUi(this);
    findFileButton = ui->findFileButton;
    filePath = ui->filePath;
    updateButton = ui->startButton;
    statusUpdate = ui->statusUpdate;
	usernameField = ui->usernameEntry;
	passwordField = ui->passwordEntry;
	hostnameField = ui->hostnameEntry;
    this->setFixedSize(650, 316);
    connect(findFileButton, SIGNAL (released()), this, SLOT (FindFile()));
    connect(updateButton, SIGNAL (released()), this, SLOT (startUpdate()));
}

void updater::FindFile(){
    findFileButton->setText("Open File");
    package = QFileDialog::getOpenFileName();
    filePath->setText(package);
}

updater::~updater(){
    delete ui;
}

bool updater::startUpdate(){
	try{
			int rc;
			statusUpdate->setText("Starting update");
			updateButton->setEnabled(false);
			printf("Staring update...");
			ssh_session my_ssh_session;
			my_ssh_session = ssh_new();
			if (my_ssh_session == NULL){
				throw updaterException("Failed to start SSH session");
			}
			// Convert the QString to the appropriate types
			std::string Qusername = usernameField->text().toUtf8().constData();
			std::string Qpassword = passwordField->text().toUtf8().constData();
			std::string Qhostname = hostnameField->text().toUtf8().constData();
			const void * username = Qusername.c_str();
			const void * hostname = Qhostname.c_str();
			const char * password = Qpassword.c_str();
			// Set connection info
			ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, (const void *)username);
			ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, hostname);
			rc = ssh_connect(my_ssh_session);
			if (rc != SSH_OK){
				throw updaterException("Failed to connect to host");
			}
			// Set password auth
			rc = ssh_userauth_password(my_ssh_session, NULL, password);
			if (rc == SSH_AUTH_ERROR){
				throw updaterException("Authentification Failed");
			}
			// SCP START
			ssh_scp scp;
			scp = ssh_scp_new(my_ssh_session, SSH_SCP_RECURSIVE, "./");
			if (scp == NULL){
				throw updaterException("SCP allocation failed");
			}
			rc = ssh_scp_init(scp);
			if (rc != SSH_OK){
				ssh_scp_free(scp);
				throw updaterException("SCP init failed");
			}
			// Create the update directory
			rc = ssh_scp_push_directory(scp, "BASESTATION_UPDATE", 0700);
			if (rc != SSH_OK){
				throw updaterException("Cannot create remote directory");
			}
			// Get the size of the update for uploading
			int size = 0;
			QFile myFile(package);
			if (myFile.open(QIODevice::ReadOnly)){
				size = myFile.size();  //when file does open.
			}else{
				throw updaterException("Failed to get pak filesize");
			}
			// move the update package to /tmp and set correct name
			std::rename(package.toStdString().c_str(), "/tmp/basestation_update.pak");
			// Upload the file
			rc = ssh_scp_push_file(scp, "/tmp/basestation_update.pak", size, 0700);
			if (rc != SSH_OK){
				throw updaterException("Failed to open remote directory");
			}
			rc = ssh_scp_write(scp, myFile.readAll() , size);
			if (rc != SSH_OK){
				throw updaterException("Failed to write pak to basestation");
			}
			// Close the scp connnection
			myFile.close();
			ssh_scp_close(scp);
			ssh_scp_free(scp);
			ssh_channel channel;
			channel = ssh_channel_new(my_ssh_session);
			if (channel == NULL){
				throw updaterException("Failed to create ssh channel");
			}
			rc = ssh_channel_open_session(channel);
			if (rc != SSH_OK){
				ssh_channel_free(channel);
				throw updaterException("SSH session failed");
			}
			char buffer[1000];
			int nbytes;
			// Send update command
			rc = ssh_channel_request_exec(channel, "unzip -o ~/BASESTATION_UPDATE/basestation_update.pak -d ~/BASESTATION_UPDATE && sh ~/BASESTATION_UPDATE/Till-master/update.sh");
			if (rc != SSH_OK){
				throw updaterException("Update payload failed");
			}
			nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
			while (nbytes > 0){
				if (fwrite(buffer, 1, nbytes, stdout) != nbytes){
					throw updaterException("Payload no answer");
				}
				nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
			}
			statusUpdate->setText("Uploaded. BaseStation will now update");
			ssh_channel_send_eof(channel);
			ssh_channel_close(channel);
			ssh_channel_free(channel);
			ssh_disconnect(my_ssh_session);
			ssh_free(my_ssh_session);
			updateButton->setEnabled(true);
			return true;
	}catch(std::exception &e){
    	std::cout << "Error: " << e.what() << std::endl;
		statusUpdate->setText(e.what());
		updateButton->setEnabled(true);
		return false;
	}
}
