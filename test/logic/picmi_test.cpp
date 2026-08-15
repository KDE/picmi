/*
    SPDX-FileCopyrightText: 2026 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QSignalSpy>
#include <QTest>

#include "boardmap.h"
#include "picmi.h"

class PicmiTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initialDimensions();
    void remainingBoxCount();
    void stateAtAfterSet();
    void undoStackSizeSignal();
    void completedByCorrectSolution();
    void solveEmitsCompletedNotWon();
    void hintReducesIncorrectCells();
    void hintOnSolvedBoardIsNoop();
};

static QSharedPointer<BoardMap> makeMap(int w, int h, const QString &pattern)
{
    QList<Board::State> data;
    for (QChar c : pattern) {
        if (c == QLatin1Char('1')) {
            data.append(Board::Box);
        } else {
            data.append(Board::Nothing);
        }
    }
    return QSharedPointer<BoardMap>(new BoardMap(w, h, data));
}

void PicmiTest::initialDimensions()
{
    Picmi game(makeMap(4, 3, QStringLiteral("100110010001")));
    QCOMPARE(game.width(), 4);
    QCOMPARE(game.height(), 3);
}

void PicmiTest::remainingBoxCount()
{
    Picmi game(makeMap(3, 1, QStringLiteral("101")));
    QCOMPARE(game.remainingBoxCount(), 2);

    game.setState(0, 0, Board::Box);
    QCOMPARE(game.remainingBoxCount(), 1);

    game.setState(2, 0, Board::Box);
    /* Setting both correct boxes wins the game; setState then
     * triggers replace(Nothing, Cross). remainingBoxCount() must
     * still report 0. */
    QCOMPARE(game.remainingBoxCount(), 0);
}

void PicmiTest::stateAtAfterSet()
{
    Picmi game(makeMap(3, 1, QStringLiteral("010")));

    QCOMPARE(game.stateAt(0, 0), Board::Nothing);
    game.setState(0, 0, Board::Cross);
    QCOMPARE(game.stateAt(0, 0), Board::Cross);
}

void PicmiTest::undoStackSizeSignal()
{
    Picmi game(makeMap(3, 1, QStringLiteral("000")));

    QSignalSpy spy(&game, &Picmi::undoStackSizeChanged);

    game.setState(0, 0, Board::Cross);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 1);

    game.undo();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 0);
}

void PicmiTest::completedByCorrectSolution()
{
    Picmi game(makeMap(3, 1, QStringLiteral("101")));

    QSignalSpy completedSpy(&game, &Picmi::gameCompleted);
    QSignalSpy wonSpy(&game, &Picmi::gameWon);

    game.setState(0, 0, Board::Box);
    QCOMPARE(completedSpy.count(), 0);

    game.setState(2, 0, Board::Box);
    QCOMPARE(completedSpy.count(), 1);
    QCOMPARE(wonSpy.count(), 1);
}

void PicmiTest::solveEmitsCompletedNotWon()
{
    Picmi game(makeMap(3, 1, QStringLiteral("101")));

    QSignalSpy completedSpy(&game, &Picmi::gameCompleted);
    QSignalSpy wonSpy(&game, &Picmi::gameWon);

    game.solve();
    QCOMPARE(completedSpy.count(), 1);
    QCOMPARE(wonSpy.count(), 0);
}

void PicmiTest::hintReducesIncorrectCells()
{
    Picmi game(makeMap(3, 1, QStringLiteral("101")));

    /* Set wrong state on (0,0): map has Box, state is Cross. */
    game.setState(0, 0, Board::Cross);

    QPoint p = game.hint();
    /* Hint picks one of the incorrect cells and sets it correctly. */
    QCOMPARE(game.stateAt(p.x(), p.y()), game.getBoardMap()->get(p.x(), p.y()) == Board::Box
                                          ? Board::Box : Board::Cross);
}

void PicmiTest::hintOnSolvedBoardIsNoop()
{
    Picmi game(makeMap(3, 1, QStringLiteral("101")));
    game.solve();

    QPoint p = game.hint();
    QCOMPARE(p, QPoint(0, 0));
}

QTEST_GUILESS_MAIN(PicmiTest)
#include "picmi_test.moc"
