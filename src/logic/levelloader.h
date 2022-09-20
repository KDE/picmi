/*
    SPDX-FileCopyrightText: 2015 Jakob Gruber <jakob.gruber@gmail.com>
    SPDX-FileCopyrightText: 2011 Julian Helfferich <julian.helfferich@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <QImage>
#include <QList>
#include <QPixmap>
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
    QPixmap preview() const { return m_preview; }

    QString visibleName() const;
    bool solved() const { return m_solved; }
    int solvedTime() const { return m_solved_time; }
    void setSolved(int seconds);

    bool operator==(const Level &that) const;

private:
    void finalize(); /* needs to be called by loader when done constructing */
    void constructPreview();
    void readSettings();
    void writeSettings(int seconds);
    QString key() const;

    QString m_name, m_author, m_levelset;
    int m_difficulty;
    int m_width, m_height;
    QList<Board::State> m_map;
    bool m_solved;
    int m_solved_time;
    QPixmap m_preview;
};

class LevelLoader
{
public:
    explicit LevelLoader(const QString &filename);

    QList<QSharedPointer<Level> > loadLevels();
    static QList<QSharedPointer<Level> > load();

private:
    void setLevelset(const QString& levelname);
    QSharedPointer<Level> loadLevel(const QDomElement &node) const;
    QList<Board::State> loadRow(const QDomElement &node) const;
    QImage openXPM(const QDomElement &node) const;
    QList<Board::State> loadXPM(const QImage &xpm) const;

    QSharedPointer<QDomDocument> m_levelset;
    QString m_levelsetname;

    const QString m_filename;

    bool m_valid;
};

#endif // LEVELLOADER_H
