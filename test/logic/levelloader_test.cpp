/*
    SPDX-FileCopyrightText: 2026 Jakob Gruber <jakob.gruber@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTest>
#include <QTextStream>

#include "levelloader.h"

class LevelLoaderTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void loadInlineRows();
    void loadMultipleLevels();
    void loadXpmLevel();
    void xpmWithoutReaderFails();
    void invalidFileMissingAttributes();
    void invalidFileMalformed();
    void emptyLevelset();

private:
    QString writeFixture(QTemporaryDir &dir, const QString &name, const QString &content) const;
};

QString LevelLoaderTest::writeFixture(QTemporaryDir &dir, const QString &name, const QString &content) const
{
    const QString path = dir.filePath(name);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qFatal("Could not write fixture %s", qPrintable(path));
    }
    QTextStream ts(&f);
    ts << content;
    return path;
}

void LevelLoaderTest::loadInlineRows()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString xml = QStringLiteral(R"(<?xml version="1.0"?>
<picmi name="Test Set">
    <board name="Tiny" author="Tester" difficulty="2">
        <row>1-1</row>
        <row>-1-</row>
    </board>
</picmi>)");

    const QString path = writeFixture(dir, QStringLiteral("test.xml"), xml);

    LevelLoader loader(path);
    QList<QSharedPointer<Level> > levels = loader.loadLevels();

    QCOMPARE(levels.size(), 1);
    QSharedPointer<Level> lvl = levels[0];

    QCOMPARE(lvl->name(), QStringLiteral("Tiny"));
    QCOMPARE(lvl->author(), QStringLiteral("Tester"));
    QCOMPARE(lvl->levelset(), QStringLiteral("Test Set"));
    QCOMPARE(lvl->difficulty(), 2);
    QCOMPARE(lvl->width(), 3);
    QCOMPARE(lvl->height(), 2);

    QList<Board::State> m = lvl->map();
    QCOMPARE(m.size(), 6);
    QCOMPARE(m[0], Board::Box);
    QCOMPARE(m[1], Board::Nothing);
    QCOMPARE(m[2], Board::Box);
    QCOMPARE(m[3], Board::Nothing);
    QCOMPARE(m[4], Board::Box);
    QCOMPARE(m[5], Board::Nothing);
}

void LevelLoaderTest::loadMultipleLevels()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString xml = QStringLiteral(R"(<?xml version="1.0"?>
<picmi name="Multi">
    <board name="A" author="x" difficulty="1">
        <row>1</row>
    </board>
    <board name="B" author="y" difficulty="3">
        <row>-1</row>
        <row>1-</row>
    </board>
</picmi>)");

    const QString path = writeFixture(dir, QStringLiteral("multi.xml"), xml);
    LevelLoader loader(path);
    QList<QSharedPointer<Level> > levels = loader.loadLevels();

    QCOMPARE(levels.size(), 2);
    QCOMPARE(levels[0]->name(), QStringLiteral("A"));
    QCOMPARE(levels[1]->name(), QStringLiteral("B"));
    QCOMPARE(levels[0]->difficulty(), 1);
    QCOMPARE(levels[1]->difficulty(), 3);
    QCOMPARE(levels[1]->width(), 2);
    QCOMPARE(levels[1]->height(), 2);
}

static const QString XPM_LEVEL_XML = QStringLiteral(R"(<?xml version="1.0"?>
<picmi name="XPM Set">
    <board name="X" author="y" difficulty="1">
        <xpm>level.xpm</xpm>
    </board>
</picmi>)");

void LevelLoaderTest::loadXpmLevel()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = writeFixture(dir, QStringLiteral("xpm.xml"), XPM_LEVEL_XML);

    QString receivedPath;
    XpmReader reader = [&receivedPath](const QString &filepath) {
        receivedPath = filepath;
        XpmGrid grid;
        grid.width = 2;
        grid.height = 2;
        grid.map = QList<Board::State>{ Board::Box, Board::Nothing,
                                        Board::Nothing, Board::Box };
        return grid;
    };

    LevelLoader loader(path, reader);
    QList<QSharedPointer<Level> > levels = loader.loadLevels();

    QCOMPARE(levels.size(), 1);

    /* The reader is called with the path relative to the levelset. */
    QCOMPARE(receivedPath,
             QFileInfo(path).absolutePath() + QLatin1Char('/') + QStringLiteral("level.xpm"));

    QCOMPARE(levels[0]->width(), 2);
    QCOMPARE(levels[0]->height(), 2);
    QList<Board::State> m = levels[0]->map();
    QCOMPARE(m.size(), 4);
    QCOMPARE(m[0], Board::Box);
    QCOMPARE(m[1], Board::Nothing);
    QCOMPARE(m[2], Board::Nothing);
    QCOMPARE(m[3], Board::Box);
}

void LevelLoaderTest::xpmWithoutReaderFails()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = writeFixture(dir, QStringLiteral("noxpm.xml"), XPM_LEVEL_XML);

    LevelLoader loader(path); /* no XpmReader provided */
    QList<QSharedPointer<Level> > levels = loader.loadLevels();

    /* The failing board is skipped rather than aborting the levelset. */
    QCOMPARE(levels.size(), 0);
}

void LevelLoaderTest::invalidFileMissingAttributes()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString xml = QStringLiteral(R"(<?xml version="1.0"?>
<picmi name="Broken">
    <board author="x" difficulty="1">
        <row>1</row>
    </board>
</picmi>)");

    const QString path = writeFixture(dir, QStringLiteral("broken.xml"), xml);
    LevelLoader loader(path);

    /* Loader logs and skips invalid boards rather than aborting. */
    QList<QSharedPointer<Level> > levels = loader.loadLevels();
    QCOMPARE(levels.size(), 0);
}

void LevelLoaderTest::invalidFileMalformed()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString xml = QStringLiteral("<not valid xml");
    const QString path = writeFixture(dir, QStringLiteral("malformed.xml"), xml);

    LevelLoader loader(path);
    QList<QSharedPointer<Level> > levels = loader.loadLevels();
    QCOMPARE(levels.size(), 0);
}

void LevelLoaderTest::emptyLevelset()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString xml = QStringLiteral(R"(<?xml version="1.0"?>
<picmi name="Empty">
</picmi>)");

    const QString path = writeFixture(dir, QStringLiteral("empty.xml"), xml);
    LevelLoader loader(path);
    QList<QSharedPointer<Level> > levels = loader.loadLevels();
    QCOMPARE(levels.size(), 0);
}

QTEST_GUILESS_MAIN(LevelLoaderTest)
#include "levelloader_test.moc"
