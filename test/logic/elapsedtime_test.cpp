/*
    SPDX-FileCopyrightText: 2026 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>

#include "elapsedtime.h"

class ElapsedTimeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void timeToStringZero();
    void timeToStringMinutesAndSeconds();
    void timeToStringHoursMinutesSeconds();
    void timeToStringPadsZeros();
    void penaltyEscalation();
    void penaltyCap();
};

void ElapsedTimeTest::timeToStringZero()
{
    QCOMPARE(Time(0).toString(), QStringLiteral("00:00"));
}

void ElapsedTimeTest::timeToStringMinutesAndSeconds()
{
    QCOMPARE(Time(65).toString(), QStringLiteral("01:05"));
}

void ElapsedTimeTest::timeToStringHoursMinutesSeconds()
{
    QCOMPARE(Time(3 * 3600 + 25 * 60 + 7).toString(),
             QStringLiteral("3:25:07"));
}

void ElapsedTimeTest::timeToStringPadsZeros()
{
    QCOMPARE(Time(60).toString(), QStringLiteral("01:00"));
    QCOMPARE(Time(3600).toString(), QStringLiteral("1:00:00"));
    QCOMPARE(Time(59).toString(), QStringLiteral("00:59"));
}

void ElapsedTimeTest::penaltyEscalation()
{
    ElapsedTime t;
    t.start();

    /* First penalty is 10 seconds, doubles each time. */
    const int initial = t.elapsedSecs();
    t.addPenaltyTime();
    QCOMPARE(t.elapsedSecs() - initial, 10);

    t.addPenaltyTime();
    QCOMPARE(t.elapsedSecs() - initial, 10 + 20);

    t.addPenaltyTime();
    QCOMPARE(t.elapsedSecs() - initial, 10 + 20 + 40);
}

void ElapsedTimeTest::penaltyCap()
{
    ElapsedTime t;
    t.start();
    t.pause(true); /* freeze the running timeslice so deltas are exact */

    /* Penalties double starting at 10. Doubling stops once a penalty
     * reaches the 3600 cap, so the largest single penalty is the first
     * doubled value past the cap: 5120. */
    int prev = t.elapsedSecs();
    int expected = 10;
    for (int i = 0; i < 10; i++) {
        t.addPenaltyTime();
        QCOMPARE(t.elapsedSecs() - prev, expected);
        prev = t.elapsedSecs();
        if (expected < 3600) {
            expected *= 2;
        }
    }
    QCOMPARE(expected, 5120);

    /* Once capped, every further penalty stays at the maximum. */
    t.addPenaltyTime();
    QCOMPARE(t.elapsedSecs() - prev, 5120);
}

QTEST_GUILESS_MAIN(ElapsedTimeTest)
#include "elapsedtime_test.moc"
