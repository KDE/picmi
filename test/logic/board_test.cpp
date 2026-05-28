/*
    SPDX-FileCopyrightText: 2026 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>

#include "board.h"
#include "boardmap.h"

class BoardTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initialization();
    void boundsCheck();
    void coordinateRoundTrip();
};

void BoardTest::initialization()
{
    BoardMap m(7, 5, QList<Board::State>(35, Board::Nothing));

    QCOMPARE(m.width(), 7);
    QCOMPARE(m.height(), 5);

    for (int y = 0; y < m.height(); y++) {
        for (int x = 0; x < m.width(); x++) {
            QCOMPARE(m.get(x, y), Board::Nothing);
        }
    }
}

void BoardTest::boundsCheck()
{
    BoardMap m(3, 3, QList<Board::State>(9, Board::Nothing));

    QVERIFY(!m.outOfBounds(0, 0));
    QVERIFY(!m.outOfBounds(2, 2));
    QVERIFY(m.outOfBounds(-1, 0));
    QVERIFY(m.outOfBounds(0, -1));
    QVERIFY(m.outOfBounds(3, 0));
    QVERIFY(m.outOfBounds(0, 3));
}

void BoardTest::coordinateRoundTrip()
{
    /* Verifies xy_to_i row-major layout via the explicit-map ctor. */
    QList<Board::State> data(6, Board::Nothing);
    data[0] = Board::Box;   /* (0,0) */
    data[2] = Board::Box;   /* (2,0) */
    data[3] = Board::Box;   /* (0,1) */
    data[5] = Board::Box;   /* (2,1) */

    BoardMap m(3, 2, data);
    QCOMPARE(m.get(0, 0), Board::Box);
    QCOMPARE(m.get(1, 0), Board::Nothing);
    QCOMPARE(m.get(2, 0), Board::Box);
    QCOMPARE(m.get(0, 1), Board::Box);
    QCOMPARE(m.get(1, 1), Board::Nothing);
    QCOMPARE(m.get(2, 1), Board::Box);
}

QTEST_GUILESS_MAIN(BoardTest)
#include "board_test.moc"
