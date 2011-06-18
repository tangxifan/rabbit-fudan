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

mainwindow.cpp
@Author: Running Teeth <running_teeth@sina.com>

********************************************************************/

#include <QtGui>

#include "mainwindow.h"
#include "rgraphicsscene.h"
#include "rgraphicsview.h"
#include "unit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_view = new RGraphicsView();
    m_view->setSceneRect(-6, -50, 620, 630);
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    m_view->setRenderHints(QPainter::Antialiasing
                         | QPainter::TextAntialiasing);
    m_view->setContextMenuPolicy(Qt::ActionsContextMenu);
    setCentralWidget(m_view);

    createActions();
    createMenus();
    createToolBars();
    //createDockWindows();

    setWindowIcon(QIcon(":/images/logo.png"));
    setWindowTitle(tr("RABBIT"));

    updateActions();
}

void MainWindow::openNetlist()
{
    QString fileName = QFileDialog::getOpenFileName(
                       this, tr("Open Netlist File"), ".",
                       tr("XML files (*.xml)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.exists()) {
            QMessageBox::critical(this, tr("Open Netlist File"),
                                  QString("Could not open file '%1'").arg(fileName));
            return;
        }

        QProcess *cmd = new QProcess();
        cmd->start("../bread/src/test_main " + fileName);

        m_view->readFile("../bread/src/output.rtd");
        return;
    }
}

void MainWindow::importRouted()
{
    QString routedFileName = QFileDialog::getOpenFileName(
                        this, tr("Import Routed RABBIT File"), ".",
                        tr("Routed RABBIT files(*.rtd)"));
    if (!routedFileName.isEmpty()) {
        QFile file(routedFileName);
        if (!file.exists()) {
            QMessageBox::critical(this, tr("Import Routed RABBIT File"),
                                  QString("Could not open file '%1'").arg(routedFileName));
            return;
        }

        m_view->readFile(routedFileName);
        return;
    }
}

void MainWindow::zoomIn()
{
    m_view->scale(1.25, 1.25);
}

void MainWindow::zoomOut()
{
    m_view->scale(0.75, 0.75);
}

void MainWindow::updateActions()
{

}

void MainWindow::createActions()
{
    openNetlistAction = new QAction(tr("&Open"), this);
    openNetlistAction->setIcon(QIcon(":/images/document-open.svg"));
    openNetlistAction->setShortcut(tr("Ctrl+O"));
    connect(openNetlistAction, SIGNAL(triggered()),
            this, SLOT(openNetlist()));

    importRoutedAction = new QAction(tr("I&mport"), this);
    importRoutedAction->setIcon(QIcon(":/images/document-import.svg"));
    importRoutedAction->setShortcut(tr("Ctrl+I"));
    connect(importRoutedAction, SIGNAL(triggered()),
            this, SLOT(importRouted()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()),
            this, SLOT(close()));

    zoomInAction = new QAction(tr("Zoom In"), this);
    zoomInAction->setIcon(QIcon(":/images/zoomin.png"));
    zoomInAction->setShortcut(tr("Ctrl+["));
    connect(zoomInAction, SIGNAL(triggered()),
            this, SLOT(zoomIn()));

    zoomOutAction = new QAction(tr("Zoom Out"), this);
    zoomOutAction->setIcon(QIcon(":/images/zoomout.png"));
    zoomOutAction->setShortcut(tr("Ctrl+]"));
    connect(zoomOutAction, SIGNAL(triggered()),
            this, SLOT(zoomOut()));

    showResistorAction = new QAction(tr("Show Resistors"), this);
    showResistorAction->setIcon(QIcon(":/images/show-resistor.svg"));
    showResistorAction->setCheckable(true);
    showResistorAction->setChecked(true);
    connect(showResistorAction, SIGNAL(toggled(bool)),
            m_view, SLOT(setShowResistor(bool)));

    showCapacitorAction = new QAction(tr("Show Capacitors"), this);
    showCapacitorAction->setIcon(QIcon(":/images/show-capacitor.svg"));
    showCapacitorAction->setCheckable(true);
    showCapacitorAction->setChecked(true);
    connect(showCapacitorAction, SIGNAL(toggled(bool)),
            m_view, SLOT(setShowCapacitor(bool)));

    showTransistorAction = new QAction(tr("Show Transistors"), this);
    showTransistorAction->setIcon(QIcon(":/images/show-transistor.svg"));
    showTransistorAction->setCheckable(true);
    showTransistorAction->setChecked(true);
    connect(showTransistorAction, SIGNAL(toggled(bool)),
            m_view, SLOT(setShowTransistor(bool)));

    showICAction = new QAction(tr("Show ICs"), this);
    showICAction->setIcon(QIcon(":/images/show-IC.svg"));
    showICAction->setCheckable(true);
    showICAction->setChecked(true);
    connect(showICAction, SIGNAL(toggled(bool)),
            m_view, SLOT(setShowIC(bool)));

    hideSelectedAction = new QAction(tr("Hide Selected Items"), this);
    hideSelectedAction->setIcon(QIcon(":/images/hide.svg"));
    hideSelectedAction->setCheckable(true);
    hideSelectedAction->setChecked(false);
    connect(hideSelectedAction, SIGNAL(toggled(bool)),
            m_view, SLOT(setHideSelected(bool)));
}

void MainWindow::createMenus()
{

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openNetlistAction);
    fileMenu->addAction(importRoutedAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);
    viewMenu->addSeparator();
    viewMenu->addAction(showResistorAction);
    viewMenu->addAction(showCapacitorAction);
    viewMenu->addAction(showTransistorAction);
    viewMenu->addAction(showICAction);
    viewMenu->addAction(hideSelectedAction);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openNetlistAction);
    fileToolBar->addAction(importRoutedAction);

    viewToolBar = addToolBar(tr("View"));
    viewToolBar->addAction(zoomInAction);
    viewToolBar->addAction(zoomOutAction);
    viewToolBar->addSeparator();
    viewToolBar->addAction(showResistorAction);
    viewToolBar->addAction(showCapacitorAction);
    viewToolBar->addAction(showTransistorAction);
    viewToolBar->addAction(showICAction);
    viewToolBar->addAction(hideSelectedAction);
}

void MainWindow::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("Unit Information"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, dock);
    viewMenu->addSeparator();
    viewMenu->addAction(dock->toggleViewAction());

}

bool MainWindow::autoRoute(const QString &fileName)
{
    return true;
}
