/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "board.h"

#include <QtGlobal>

Board::Board(int width, int height)
    : m_width(width), m_height(height), m_size(width * height),
      m_state(width * height, Nothing)
{
}

bool Board::outOfBounds(int x, int y) const {
    return (x < 0 || x >= m_width || y < 0 || y >= m_height);
}

Board::State Board::get(int x, int y) const {
    Q_ASSERT(!outOfBounds(x, y));
    return m_state[xy_to_i(x, y)];
}

int Board::xy_to_i(int x, int y) const {
    return y * m_width + x;
}

int Board::i_to_x(int i) const {
    return i % m_width;
}

int Board::i_to_y(int i) const {
    return i / m_width;
}
