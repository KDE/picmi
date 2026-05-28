/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>
    SPDX-FileCopyrightText: 2011 Julian Helfferich <julian.helfferich@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <functional>

#include <QList>
#include <QString>
#include <QSharedPointer>

#include "src/logic/board.h"

class QDomDocument;
class QDomElement;
class LevelLoader;

class Level
{
    friend class LevelLoader;
public:
    Level();

    QString name() const;
    QString author() const;
    QString levelset() const { return m_levelset; }
    int difficulty() const { return m_difficulty; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    QList<Board::State> map() const { return m_map; }

    QString visibleName() const;
    bool solved() const { return m_solved; }
    int solvedTime() const { return m_solved_time; }
    void setSolved(int seconds);

    bool operator==(const Level &that) const;

private:
    void finalize(); /* needs to be called by loader when done constructing */
    void readSettings();
    void writeSettings(int seconds);
    QString key() const;

    QString m_name, m_author, m_levelset;
    int m_difficulty;
    int m_width, m_height;
    QList<Board::State> m_map;
    bool m_solved;
    int m_solved_time;
};

/* Bitmap data resolved from a referenced image file. The logic layer
 * does not depend on QtGui; image decoding is provided by callers via
 * the XpmReader callback below. */
struct XpmGrid {
    QList<Board::State> map;
    int width;
    int height;
};

/* Reads an image file at the given absolute path and returns its grid.
 * Should throw std::runtime_error on any failure. */
using XpmReader = std::function<XpmGrid(const QString &filepath)>;

class LevelLoader
{
public:
    explicit LevelLoader(const QString &filename, XpmReader reader = {});

    QList<QSharedPointer<Level> > loadLevels();
    static QList<QSharedPointer<Level> > load(XpmReader reader = {});

private:
    void setLevelset(const QString& levelname);
    QSharedPointer<Level> loadLevel(const QDomElement &node) const;
    QList<Board::State> loadRow(const QDomElement &node) const;
    XpmGrid loadXpm(const QDomElement &node) const;

    QSharedPointer<QDomDocument> m_levelset;
    QString m_levelsetname;

    const QString m_filename;
    XpmReader m_xpm_reader;

    bool m_valid;
};

#endif // LEVELLOADER_H
