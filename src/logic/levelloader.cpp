/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>
    SPDX-FileCopyrightText: 2011 Julian Helfferich <julian.helfferich@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <config.h>
#include "levelloader.h"

#include "picmi_debug.h"
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

#include <stdexcept>

#include "settings.h"

static void appendUniqueLevels(QList<QSharedPointer<Level> > &dst,
                               const QList<QSharedPointer<Level> > &src)
{
    for (const QSharedPointer<Level> &level : src) {
        bool seen = false;
        for (const QSharedPointer<Level> &existing : dst) {
            if (*existing == *level) {
                seen = true;
                break;
            }
        }
        if (!seen) {
            dst.append(level);
        }
    }
}

Level::Level() : m_solved(false), m_solved_time(0) { }

QString Level::key() const {
    return QStringLiteral("preset_scores/%1_%2").arg(m_levelset, m_name);
}

void Level::writeSettings(int seconds) {
    QSharedPointer<QSettings> settings = Settings::instance()->qSettings();
    QString k = key();

    settings->setValue(k, seconds);
    settings->sync();
}

void Level::finalize() {
    readSettings();
}

void Level::readSettings() {
    QSharedPointer<QSettings> settings = Settings::instance()->qSettings();
    QString k = key();

    if (settings->contains(k)) {
        m_solved = true;
        m_solved_time = settings->value(k).toInt();
    }
}

void Level::setSolved(int seconds) {
    if (m_solved_time > 0 && seconds >= m_solved_time) {
        return;
    }
    m_solved = true;
    m_solved_time = seconds;
    writeSettings(seconds);
}

bool Level::operator==(const Level &that) const {
    return (that.m_name == m_name && that.m_author == m_author);
}

QList<QSharedPointer<Level> > LevelLoader::load(XpmReader reader) {
    const QString prefix = QStringLiteral("levels/");
    QList<QString> paths;
    paths << QString(prefix)
          << QStringLiteral(FILEPATH) + QStringLiteral("/") + prefix
          << QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                    prefix,
                                    QStandardPaths::LocateOption::LocateDirectory);

    QList<QSharedPointer<Level> > list;

    for (int i = 0; i < paths.size(); i++) {
        QDir dir(paths[i]);
        if (!dir.exists()) {
            continue;
        }

        QStringList files = dir.entryList(QStringList(QStringLiteral("*.xml")));

        for (int j = 0; j < files.size(); j++) {
            LevelLoader loader(dir.absoluteFilePath(files[j]), reader);
            appendUniqueLevels(list, loader.loadLevels());
        }
    }

    return list;
}

LevelLoader::LevelLoader(const QString &filename, XpmReader reader) :
    m_filename(filename), m_xpm_reader(std::move(reader)), m_valid(true)
{
    setLevelset(filename);
}

void LevelLoader::setLevelset(const QString& filename)
{
    m_levelset = QSharedPointer<QDomDocument>(new QDomDocument());

    QFile file(filename);
    if (!file.open( QIODevice::ReadOnly)) {
        throw std::runtime_error(QStringLiteral("Can't open file %1").arg(filename).toStdString());
    }

    const QDomDocument::ParseResult parseResult = m_levelset->setContent(&file);
    file.close();
    if (!parseResult) {
        qCDebug(PICMIC_LOG) << QStringLiteral("Can't read levelset from %1 \nError: %2 in Line %3, Column %4")
                              .arg(filename, parseResult.errorMessage).arg(parseResult.errorLine).arg(parseResult.errorColumn);
        m_valid = false;
    }
}

QList<QSharedPointer<Level> > LevelLoader::loadLevels() {
    QList<QSharedPointer<Level> > l;

    if (!m_valid) {
        return l;
    }

    QDomElement levels = m_levelset->documentElement();
    if (!levels.hasAttribute(QStringLiteral("name"))) {
        qCDebug(PICMIC_LOG) << "Loading level failed: no levelset name specified";
        return l;
    }
    m_levelsetname = levels.attribute(QStringLiteral("name"));

    QDomNodeList childNodes = levels.childNodes();
    for (int i = 0; i < childNodes.size(); i++) {
        try {
            l.append(loadLevel(childNodes.at(i).toElement()));
        } catch (const std::runtime_error &e) {
            qCDebug(PICMIC_LOG) << "Loading level failed: " << e.what();
        }
    }
    return l;
}

QSharedPointer<Level> LevelLoader::loadLevel(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != QLatin1String("board")) {
        throw std::runtime_error("Unexpected level node");
    }

    if (!node.hasAttribute(QStringLiteral("name")) || !node.hasAttribute(QStringLiteral("author"))
            || !node.hasAttribute(QStringLiteral("difficulty"))) {
        throw std::runtime_error("Level node missing attribute.");
    }

    QSharedPointer<Level> p(new Level);
    p->m_name = node.attribute(QStringLiteral("name"));
    p->m_author = node.attribute(QStringLiteral("author"));
    p->m_levelset = m_levelsetname;
    p->m_difficulty = node.attribute(QStringLiteral("difficulty")).toInt();

    QDomNodeList childNodes = node.childNodes();

    if (childNodes.isEmpty()) {
        throw std::runtime_error("Empty level definition.");
    }

    const QString tag_name = childNodes.at(0).toElement().tagName();
    if (tag_name == QLatin1String("row")) {
        int i;
        QList<Board::State> l;
        for (i = 0; i < childNodes.size(); i++) {
            l = loadRow(childNodes.at(i).toElement());
            p->m_map.append(l);
        }
        p->m_width = l.size();
        p->m_height = i;
    } else if (tag_name == QLatin1String("xpm")) {
        XpmGrid grid = loadXpm(childNodes.at(0).toElement());
        p->m_map = std::move(grid.map);
        p->m_width = grid.width;
        p->m_height = grid.height;
    } else {
        throw std::runtime_error("Unexpected level definition.");
    }

    if (p->m_map.size() != p->height() * p->width()) {
        throw std::runtime_error("Invalid board size");
    }

    p->finalize();

    return p;
}

static Board::State charToState(const QChar &c) {
    switch (c.toLatin1()) {
    case '-': return Board::Nothing;
    case '1': return Board::Box;
    default: throw std::runtime_error("Invalid char in level definition");
    }
}

XpmGrid LevelLoader::loadXpm(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != QLatin1String("xpm")) {
        throw std::runtime_error("Unexpected xpm node");
    }
    if (!m_xpm_reader) {
        throw std::runtime_error("Level references XPM but no reader was provided");
    }

    QFileInfo file(m_filename);
    const QString filepath = file.absolutePath() + QLatin1Char('/') + node.text();
    return m_xpm_reader(filepath);
}

QList<Board::State> LevelLoader::loadRow(const QDomElement &node) const {
    if (node.isNull() || node.tagName() != QLatin1String("row")) {
        throw std::runtime_error("Unexpected row node");
    }

    const QString text = node.text();
    QList<Board::State> list;
    for (const QChar &c : text) {
        Board::State s = charToState(c);
        list.append(s);
    }

    return list;
}
