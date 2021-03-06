/**************************************************************************
 *                                                                        *
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>                        *
 *                                                                        *
 * This program is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU General Public License            *
 * as published by the Free Software Foundation; either version 3         *
 * of the License, or (at your option) any later version.                 *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                        *
 **************************************************************************/

#include "logindialog.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>

#include <QtCore/QUrl>

#include "quaternionconnection.h"
#include "settings.h"

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
{
    serverEdit = new QLineEdit("https://matrix.org");
    userEdit = new QLineEdit();
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode( QLineEdit::Password );
    saveTokenCheck = new QCheckBox(tr("Stay logged in"));
    statusLabel = new QLabel(tr("Welcome to Quaternion"));
    loginButton = new QPushButton(tr("Login"));

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(tr("Server"), serverEdit);
    formLayout->addRow(tr("User"), userEdit);
    formLayout->addRow(tr("Password"), passwordEdit);
    formLayout->addRow(saveTokenCheck);
    
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(statusLabel);
    
    setLayout(mainLayout);

    {
        // Fill defaults
        using namespace QMatrixClient;
        QStringList accounts { SettingsGroup("Accounts").childGroups() };
        if ( !accounts.empty() )
        {
            AccountSettings account { accounts.front() };
            QUrl homeserver = account.homeserver();
            QString username = account.userId();
            if (username.startsWith('@'))
            {
                QString serverpart = ":" + homeserver.host();
                if (homeserver.port() != -1)
                    serverpart += ":" + QString::number(homeserver.port());
                if (username.endsWith(serverpart))
                {
                    // Keep only the local part of the user id
                    username.remove(0, 1).chop(serverpart.size());
                }
            }
            serverEdit->setText(homeserver.toString());
            userEdit->setText(username);
            saveTokenCheck->setChecked(account.keepLoggedIn());
        }
        else
        {
            serverEdit->setText("https://matrix.org");
            saveTokenCheck->setChecked(false);
        }
    }
    if (userEdit->text().isEmpty())
        userEdit->setFocus();
    else
        passwordEdit->setFocus();

    connect( loginButton, &QPushButton::clicked, this, &LoginDialog::login );
}

void LoginDialog::setStatusMessage(const QString& msg)
{
    statusLabel->setText(msg);
}

QMatrixClient::Connection* LoginDialog::releaseConnection()
{
    return m_connection.take();
}

bool LoginDialog::keepLoggedIn() const
{
    return saveTokenCheck->isChecked();
}

void LoginDialog::login()
{
    setStatusMessage(tr("Connecting and logging in, please wait"));
    setDisabled(true);
    QUrl url = QUrl::fromUserInput(serverEdit->text());
    QString user = userEdit->text();
    QString password = passwordEdit->text();

    m_connection.reset(new Connection(url));

    connect( m_connection.data(), &Connection::connected,
             this, &QDialog::accept );
    connect( m_connection.data(), &Connection::loginError,
             this, &LoginDialog::error );
    m_connection->connectToServer(user, password);
}

void LoginDialog::error(QString error)
{
    setStatusMessage(error);
    m_connection.reset();
    setDisabled(false);
}

