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

********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QAction>

class RGraphicsView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

private slots:
    void openNetlist();
    void importRouted();
    void zoomIn();
    void zoomOut();
    void updateActions();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createDockWindows();

    bool autoRoute(const QString &fileName);

    QMenu *fileMenu;
    QMenu *viewMenu;
    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QAction *openNetlistAction;
    QAction *importRoutedAction;
    QAction *exitAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *showResistorAction;
    QAction *showCapacitorAction;
    QAction *showTransistorAction;
    QAction *showICAction;
    QAction *hideSelectedAction;

    RGraphicsView *m_view;
};

#endif // MAINWINDOW_H
