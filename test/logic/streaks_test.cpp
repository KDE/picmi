#include "streaks_test.h"

#include <QTest>

#include "streaks.h"

QTEST_GUILESS_MAIN(StreaksTest)

#define STREAK_TEST(map, state, expected) do { \
    QSharedPointer<Streaks> s = generateStreaks(map, state); \
    if (!s) { \
        QFAIL("Streak generation failed."); \
    } \
    QVector<Streaks::Streak> row = s->getRowStreak(0); \
    QVERIFY(streakEquals(row, expected)); \
} while (false);

/**
 * Generates a Streaks instance from the given string representations
 * of map and state. A map string consists of '.' and 'b' chars, representing
 * respectively Nothing and Box cells. A state string consists of '.', 'b' and
 * 'x' chars (with 'x' representing cross cells). If the board has more than
 * one row, each line is terminated by '\n'.
 */
static QSharedPointer<Streaks>
generateStreaks(const QString &map, const QString &state)
{
    if (map.size() != state.size()) {
        return QSharedPointer<Streaks>();
    }

    /* Generate the map. */

    QList<Board::State> ss;
    for (int i = 0; i < map.size(); i++) {
        switch (map[i].toLatin1()) {
        case '.': /* Nothing. */
            ss.append(Board::Nothing);
            break;
        case 'b': /* Box. */
            ss.append(Board::Box);
            break;
        default:
            return QSharedPointer<Streaks>();
        }
    }

    QSharedPointer<BoardMap> mptr(new BoardMap(map.size(), 1, ss));

    /* Generate the state. */

    QSharedPointer<BoardState> sptr(new BoardState(state.size(), 1));

    for (int i = 0; i < state.size(); i++) {
        switch (state[i].toLatin1()) {
        case '.': /* Nothing. */
            break;
        case 'b': /* Box. */
            sptr->set(i, 0, Board::Box);
            break;
        case 'x': /* Cross. */
            sptr->set(i, 0, Board::Cross);
            break;
        default:
            return QSharedPointer<Streaks>();
        }
    }

    return QSharedPointer<Streaks>(new Streaks(mptr, sptr));
}

/**
 * Compares the actual streak result against a string representing the
 * expected result. The string consists of 'x' and '.' characters representing,
 * respectively, solved and unsolved streaks.
 */
static bool
streakEquals(const QVector<Streaks::Streak> &actual,
             const QString &expected)
{
    if (actual.size() != expected.size()) {
        qWarning() << "Actual size not equal to expected size.";
        return false;
    }

    QString actualString;

    for (int i = 0; i < expected.size(); i++) {
        if (actual[i].solved) {
            actualString.append(QLatin1Char('x'));
        } else {
            actualString.append(QLatin1Char('.'));
        }
    }

    if (actualString == expected) {
        return true;
    }

    qWarning() << QStringLiteral("Result mismatch. Actual: ") + actualString + QStringLiteral(", Expected: ") + expected;

    return false;
}

void StreaksTest::testSanity()
{
    QSharedPointer<Streaks> s = generateStreaks(QStringLiteral("....b"), QStringLiteral("xxxxb"));

    QCOMPARE(1, 1);
}

void StreaksTest::test00()
{
    /* Lifted from https://bugs.kde.org/show_bug.cgi?id=321842 */
    STREAK_TEST(QStringLiteral("bb.b.bbbb.bbb.b"),
                QStringLiteral("bbxbxbb.bxbbbxb"),
                QStringLiteral("xx.xx"));
}

void StreaksTest::test01()
{
    /* Lifted from https://bugs.kde.org/show_bug.cgi?id=321842 */
    STREAK_TEST(QStringLiteral("bb.b.bbbb.bbb.b"),
                QStringLiteral("bbxbxbb.bxb.bxb"),
                QStringLiteral("xx..x"));
}

void StreaksTest::test02()
{
    /* Lifted from https://bugs.kde.org/show_bug.cgi?id=321842 */
    STREAK_TEST(QStringLiteral("bb.b.bbbb.bbb.b"),
                QStringLiteral("bbxbxbbbbxbbbxb"),
                QStringLiteral("xxxxx"));
}

void StreaksTest::test03()
{
    STREAK_TEST(QStringLiteral("bb.b.bbbb.bbb.b"),
                QStringLiteral("bbxbxbbbbxbb..."),
                QStringLiteral("xxx.."));
}

void StreaksTest::test04()
{
    STREAK_TEST(QStringLiteral("bb.b.bbbb.bbb.b"),
                QStringLiteral("bbxbxbbbbxbbb.."),
                QStringLiteral("xxxx."));
}

void StreaksTest::test05()
{
    STREAK_TEST(QStringLiteral("b.b......."),
                QStringLiteral("bxb....bxb"),
                QStringLiteral(".."));
}

void StreaksTest::test06()
{
    STREAK_TEST(QStringLiteral("b.b......."),
                QStringLiteral("b........b"),
                QStringLiteral("xx"));
}

void StreaksTest::test07()
{
    STREAK_TEST(QStringLiteral("b.b......."),
                QStringLiteral("bxb....bxb"),
                QStringLiteral(".."));
}

void StreaksTest::test08()
{
    STREAK_TEST(QStringLiteral("b.b......."),
                QStringLiteral("bxb......."),
                QStringLiteral("xx"));
}

void StreaksTest::test09()
{
    STREAK_TEST(QStringLiteral("b.bb.bbb."),
                QStringLiteral("bxbb..bbb"),
                QStringLiteral("xxx"));
}

void StreaksTest::test10()
{
    STREAK_TEST(QStringLiteral("b.bb.bbb."),
                QStringLiteral("bxb...bbb"),
                QStringLiteral("x.x"));
}

void StreaksTest::test11()
{
    STREAK_TEST(QStringLiteral("b.bb.bbb."),
                QStringLiteral("bxbbxxbbb"),
                QStringLiteral("xxx"));
}

void StreaksTest::test12()
{
    STREAK_TEST(QStringLiteral("bb.b....."),
                QStringLiteral("bb...xbxb"),
                QStringLiteral(".x"));
}

void StreaksTest::test13()
{
    STREAK_TEST(QStringLiteral("b.b.b.b.bb"),
                QStringLiteral("bbbxbxb..."),
                QStringLiteral(".xx.."));
}

void StreaksTest::bench00()
{
    QSharedPointer<Streaks> s = generateStreaks(QStringLiteral("b.b.b.b.bbb.b.b.b.bb"),
                                                QStringLiteral("bbbxbxb...bbbxbxb..."));
    if (!s) {
        QFAIL("Streak generation failed.");
    }
    QBENCHMARK {
        s->update(0, 0);
        s->getRowStreak(0);
    }
}

#include "moc_streaks_test.cpp"
