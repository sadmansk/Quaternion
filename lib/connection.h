/******************************************************************************
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef QMATRIXCLIENT_CONNECTION_H
#define QMATRIXCLIENT_CONNECTION_H

#include <QtCore/QUrl>

class QNetworkAccessManager;

namespace QMatrixClient
{
    class Connection
    {
        public:
            Connection(QUrl baseUrl);
            virtual ~Connection();
            
            bool isConnected() const;
            QString token() const;
            QUrl baseUrl() const;
            
            QNetworkAccessManager* nam() const;
            void setToken( QString token );
            
        private:
            class Private;
            Private* d;
    };
}           

#endif // QMATRIXCLIENT_CONNECTION_H