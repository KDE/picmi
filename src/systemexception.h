/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SYSTEMEXCEPTION_H
#define SYSTEMEXCEPTION_H

#include <QString>
#include <exception>

class SystemException : public std::exception
{
public:
    SystemException() { m_msg = QStringLiteral("system error"); }
    SystemException(const QString &msg) { m_msg = msg; }

    ~SystemException() throw() override { }

    const char *what() const throw() override {
        return m_msg.toLatin1().constData();
    }

private:
    QString m_msg;
};

#endif // SYSTEMEXCEPTION_H
