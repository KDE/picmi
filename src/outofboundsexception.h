/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef OUTOFBOUNDSEXCEPTION_H
#define OUTOFBOUNDSEXCEPTION_H

#include <exception>

class OutOfBoundsException : public std::exception
{
public:
    const char *what() const throw() override {
        return "index out of bounds";
    }
};

#endif // OUTOFBOUNDSEXCEPTION_H
