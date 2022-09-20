/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BOARDMAP_H
#define BOARDMAP_H

#include <QList>

#include "board.h"

class BoardMap : public Board
{
public:
    /* 0 < width, height; 0.0 < box_ratio < 1.0 */
    BoardMap(int width, int height, double box_ratio);

    BoardMap(int width, int height, const QList<Board::State> &map);

    /* returns the total box count */
    int boxCount() const { return m_box_count; }

private:
    void genRandom();

private:
    const int m_box_count;
};

#endif // BOARDMAP_H
