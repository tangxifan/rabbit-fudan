/*******************************************************************

Part of the RABBIT project

RABBIT is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RABBIT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rabbit.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************

$Revision: 1 $:
$Author: running_teeth@sina.com $:
$Date: 2011-04-03 21:17:33 +0800 (Sun, 3 Apr 2011) $

********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include "unititem.h"
#include "unitview.h"

class QAction;
class QGraphicsScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

private slots:
    void open();
    void zoomIn();
    void zoomOut();
    void updateActions();

private:


    void createActions();
    void createMenus();
    void createToolBars();

    bool loadFile(const QString &fileName);
    bool readFile(const QString &fileName);
    void processLine(QString &line);

    void loadUnitLib();

    void generateUnitItem();
    void addUnitItem(UnitItem *unit);

    QPointF bbCToPos(QString bbCoord);

    QMenu *fileMenu;
    QMenu *viewMenu;
    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QAction *openAction;
    QAction *exitAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;

    QGraphicsScene *scene;
    UnitView *view;
    QMap<QString, QVector<int> > unitTypePin;
    QVector<UnitItem *> units;
};

#endif // MAINWINDOW_H
