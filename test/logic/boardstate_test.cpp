/*
    SPDX-FileCopyrightText: 2026 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QSignalSpy>
#include <QTest>

#include "boardmap.h"
#include "boardstate.h"

class BoardStateTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initialState();
    void setAndGet();
    void boxCountTracking();
    void setNoOpWhenUnchanged();
    void undoSingleStep();
    void undoEmptyStack();
    void undoSequence();
    void saveAndLoad();
    void saveIdempotent();
    void loadEmptyStack();
    void undoPastSavedStateDropsIt();
    void currentStateAge();
    void replace();
    void solve();
    void undoSignal();
    void saveSignal();
};

void BoardStateTest::initialState()
{
    BoardState s(4, 3);

    QCOMPARE(s.width(), 4);
    QCOMPARE(s.height(), 3);
    QCOMPARE(s.boxCount(), 0);
    QCOMPARE(s.currentStateAge(), 0);

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 4; x++) {
            QCOMPARE(s.get(x, y), Board::Nothing);
        }
    }
}

void BoardStateTest::setAndGet()
{
    BoardState s(3, 3);

    s.set(1, 1, Board::Box);
    QCOMPARE(s.get(1, 1), Board::Box);

    s.set(2, 0, Board::Cross);
    QCOMPARE(s.get(2, 0), Board::Cross);
    QCOMPARE(s.get(1, 1), Board::Box);
}

void BoardStateTest::boxCountTracking()
{
    BoardState s(3, 3);

    s.set(0, 0, Board::Box);
    QCOMPARE(s.boxCount(), 1);

    s.set(1, 0, Board::Box);
    QCOMPARE(s.boxCount(), 2);

    /* Replacing a Box with Cross decrements. */
    s.set(0, 0, Board::Cross);
    QCOMPARE(s.boxCount(), 1);

    /* Cross to Nothing leaves count unchanged. */
    s.set(0, 0, Board::Nothing);
    QCOMPARE(s.boxCount(), 1);

    /* Nothing back to Box increments. */
    s.set(0, 0, Board::Box);
    QCOMPARE(s.boxCount(), 2);
}

void BoardStateTest::setNoOpWhenUnchanged()
{
    BoardState s(3, 3);

    s.set(0, 0, Board::Box);
    QCOMPARE(s.currentStateAge(), 1);

    s.set(0, 0, Board::Box);
    QCOMPARE(s.currentStateAge(), 1); /* No undo entry created. */
}

void BoardStateTest::undoSingleStep()
{
    BoardState s(3, 3);

    s.set(1, 1, Board::Box);
    QPoint p = s.undo();

    QCOMPARE(p, QPoint(1, 1));
    QCOMPARE(s.get(1, 1), Board::Nothing);
    QCOMPARE(s.boxCount(), 0);
}

void BoardStateTest::undoEmptyStack()
{
    BoardState s(3, 3);

    QPoint p = s.undo();
    QCOMPARE(p, QPoint());
}

void BoardStateTest::undoSequence()
{
    BoardState s(3, 3);

    s.set(0, 0, Board::Box);
    s.set(1, 0, Board::Cross);
    s.set(2, 0, Board::Box);

    QCOMPARE(s.boxCount(), 2);

    s.undo();
    QCOMPARE(s.get(2, 0), Board::Nothing);
    QCOMPARE(s.boxCount(), 1);

    s.undo();
    QCOMPARE(s.get(1, 0), Board::Nothing);

    s.undo();
    QCOMPARE(s.get(0, 0), Board::Nothing);
    QCOMPARE(s.boxCount(), 0);
}

void BoardStateTest::saveAndLoad()
{
    BoardState s(3, 3);

    s.set(0, 0, Board::Box);
    s.set(1, 0, Board::Cross);
    s.saveState();

    s.set(2, 0, Board::Box);
    QCOMPARE(s.boxCount(), 2);
    QCOMPARE(s.currentStateAge(), 1); /* one action since save */

    s.loadState();
    QCOMPARE(s.get(2, 0), Board::Nothing);
    QCOMPARE(s.boxCount(), 1);
    QCOMPARE(s.currentStateAge(), 2); /* save consumed; age now full undo stack */
}

void BoardStateTest::saveIdempotent()
{
    BoardState s(3, 3);

    s.set(0, 0, Board::Box);

    QSignalSpy spy(&s, &BoardState::saveStackSizeChanged);
    s.saveState();
    s.saveState();
    QCOMPARE(spy.count(), 1);
}

void BoardStateTest::loadEmptyStack()
{
    BoardState s(3, 3);

    s.set(0, 0, Board::Box);
    s.loadState(); /* no-op, no saved state */

    QCOMPARE(s.get(0, 0), Board::Box);
}

void BoardStateTest::undoPastSavedStateDropsIt()
{
    BoardState s(3, 3);

    s.set(0, 0, Board::Box);
    s.saveState();

    QSignalSpy spy(&s, &BoardState::saveStackSizeChanged);
    s.undo(); /* undo size = 0, falls below saved size of 1 -> save dropped */

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toInt(), 0);
}

void BoardStateTest::currentStateAge()
{
    BoardState s(3, 3);

    QCOMPARE(s.currentStateAge(), 0);

    s.set(0, 0, Board::Box);
    QCOMPARE(s.currentStateAge(), 1);

    s.set(1, 0, Board::Box);
    QCOMPARE(s.currentStateAge(), 2);

    s.saveState();
    QCOMPARE(s.currentStateAge(), 0);

    s.set(2, 0, Board::Box);
    QCOMPARE(s.currentStateAge(), 1);
}

void BoardStateTest::replace()
{
    BoardState s(3, 3);

    s.set(0, 0, Board::Box);
    s.set(1, 0, Board::Cross);
    s.set(2, 0, Board::Box);

    s.replace(Board::Nothing, Board::Cross);

    QCOMPARE(s.get(0, 0), Board::Box);
    QCOMPARE(s.get(1, 0), Board::Cross);
    QCOMPARE(s.get(2, 0), Board::Box);
    /* Previously-empty cells are now Cross. */
    QCOMPARE(s.get(0, 1), Board::Cross);
    QCOMPARE(s.get(2, 2), Board::Cross);
}

void BoardStateTest::solve()
{
    QList<Board::State> mapData;
    mapData << Board::Box << Board::Nothing << Board::Box
            << Board::Nothing << Board::Box << Board::Nothing;
    BoardMap m(3, 2, mapData);

    BoardState s(3, 2);
    s.solve(&m);

    QCOMPARE(s.get(0, 0), Board::Box);
    QCOMPARE(s.get(1, 0), Board::Cross); /* Nothing replaced by Cross. */
    QCOMPARE(s.get(2, 0), Board::Box);
    QCOMPARE(s.get(0, 1), Board::Cross);
    QCOMPARE(s.get(1, 1), Board::Box);
    QCOMPARE(s.get(2, 1), Board::Cross);
}

void BoardStateTest::undoSignal()
{
    BoardState s(3, 3);

    QSignalSpy spy(&s, &BoardState::undoStackSizeChanged);

    s.set(0, 0, Board::Box);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 1);

    s.set(1, 0, Board::Box);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 2);

    s.undo();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 1);
}

void BoardStateTest::saveSignal()
{
    BoardState s(3, 3);

    QSignalSpy spy(&s, &BoardState::saveStackSizeChanged);

    s.set(0, 0, Board::Box);
    s.saveState();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 1);
}

QTEST_GUILESS_MAIN(BoardStateTest)
#include "boardstate_test.moc"
