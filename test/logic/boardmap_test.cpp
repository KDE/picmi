/*
    SPDX-FileCopyrightText: 2026 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>

#include "boardmap.h"

class BoardMapTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void explicitMap();
    void emptyExplicitMap();
    void randomMapBoxCount();
    void randomMapDistribution();
};

void BoardMapTest::explicitMap()
{
    QList<Board::State> data;
    data << Board::Box << Board::Nothing << Board::Box
         << Board::Nothing << Board::Box << Board::Nothing;

    BoardMap m(3, 2, data);

    QCOMPARE(m.boxCount(), 3);
    QCOMPARE(m.get(0, 0), Board::Box);
    QCOMPARE(m.get(1, 0), Board::Nothing);
    QCOMPARE(m.get(2, 0), Board::Box);
    QCOMPARE(m.get(0, 1), Board::Nothing);
    QCOMPARE(m.get(1, 1), Board::Box);
    QCOMPARE(m.get(2, 1), Board::Nothing);
}

void BoardMapTest::emptyExplicitMap()
{
    QList<Board::State> data(9, Board::Nothing);
    BoardMap m(3, 3, data);

    QCOMPARE(m.boxCount(), 0);
}

void BoardMapTest::randomMapBoxCount()
{
    /* The boxCount() reports the *target* count (width * height * ratio).
     * Verify it lines up with the constructor argument. */
    const int w = 10, h = 10;
    const double ratio = 0.5;
    BoardMap m(w, h, ratio);

    QCOMPARE(m.boxCount(), int(w * h * ratio));
}

void BoardMapTest::randomMapDistribution()
{
    /* Smoke test: actual box count in the grid matches the reported target. */
    const int w = 15, h = 15;
    const double ratio = 0.4;
    BoardMap m(w, h, ratio);

    int actual = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (m.get(x, y) == Board::Box) {
                actual++;
            }
        }
    }
    QCOMPARE(actual, m.boxCount());
}

QTEST_GUILESS_MAIN(BoardMapTest)
#include "boardmap_test.moc"
