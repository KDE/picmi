/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ELAPSEDTIME_H
#define ELAPSEDTIME_H

#include <QDateTime>

class Time
{
public:
    Time(int seconds) : m_seconds(seconds) { }
    QString toString(const QString &format = QStringLiteral("%1:%2:%3")) const;

private:
    const int m_seconds;

    static const int m_secs_per_minute = 60;
    static const int m_secs_per_hour = 60 * 60;
};

class ElapsedTime
{
public:
    ElapsedTime();

    /* start, stop, or (un)pause the timer */
    void start();
    void pause(bool paused);
    void stop();

    /* adds penalty time and increases the next penalty amount */
    void addPenaltyTime();

    /* return elapsed seconds since the datetime when start() was called */
    int elapsedSecs() const;
    QDateTime startDate() const;

private:
    int currentTimesliceSecs() const;

    QDateTime m_start_date;
    int m_elapsed;
    qint64 m_start;
    bool m_paused, m_stopped;

    int m_next_penalty;
    const int m_penalty_multiplier;
};

#endif // ELAPSEDTIME_H
