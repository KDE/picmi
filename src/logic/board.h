/* *************************************************************************
 *  Copyright 2015 Jakob Gruber <jakob.gruber@gmail.com>                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */


#ifndef BOARD_H
#define BOARD_H

#include <QVector>

#include "src/outofboundsexception.h"

class Board
{
public:

    enum State {
        Nothing,
        Box,
        Cross
    };

    /* 0 < width, height */
    Board(int width, int height);

    virtual ~Board() { }

    /* 0 <= x < m_width; 0 <= y < m_height */
    enum State get(int x, int y) const;

    /* returns whether (x, y) is outside the playing area */
    bool outOfBounds(int x, int y) const;

    int width() const { return m_width; }
    int height() const { return m_height; }

protected:
    /* throws OutOfBoundsException if x,y are not located in bounds */
    void assertInbounds(int x, int y) const;

    /* convert between (x,y) coordinates and the flat list index */
    int xy_to_i(int x, int y) const;
    int i_to_x(int i) const;
    int i_to_y(int i) const;

    const int m_width, m_height, m_size;
    QVector<enum State> m_state;
};

#endif // BOARD_H
