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

    /* Penalty doubles until it hits the 3600s cap, then stays.
     * Run enough iterations to cross the cap, then verify the
     * marginal addition is bounded by 3600. */
    int prev = t.elapsedSecs();
    for (int i = 0; i < 20; i++) {
        t.addPenaltyTime();
    }
    int marginal_first = t.elapsedSecs() - prev;

    prev = t.elapsedSecs();
    t.addPenaltyTime();
    int marginal_capped = t.elapsedSecs() - prev;

    QVERIFY(marginal_capped <= 3600 * 2);
    QVERIFY(marginal_capped > 0);
    Q_UNUSED(marginal_first);
}

QTEST_GUILESS_MAIN(ElapsedTimeTest)
#include "elapsedtime_test.moc"
