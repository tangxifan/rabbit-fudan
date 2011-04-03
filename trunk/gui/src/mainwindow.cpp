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

#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, 594, 189);
    scene->addPixmap(QPixmap(":/res/element/breadboard.svg"));

    view = new UnitView();
    view->setSceneRect(0, 0, 594, 189);
    view->setScene(scene);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->setRenderHints(QPainter::Antialiasing
                         | QPainter::TextAntialiasing);
    view->setContextMenuPolicy(Qt::ActionsContextMenu);
    view->show();
    setCentralWidget(view);

    loadUnitLib();
    createActions();
    createMenus();
    createToolBars();

    connect(scene, SIGNAL(selectionChanged()),
            this, SLOT(updateActions()));

    setWindowIcon(QIcon(":/res/images/logo.png"));
    setWindowTitle(tr("RABBIT"));

    updateActions();
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(
                       this, tr("Open RABBIT file"), ".",
                       tr("RABBIT files (*.rtd)"));
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::zoomIn()
{
    view->scale(1.25, 1.25);
}

void MainWindow::zoomOut()
{
    view->scale(0.75, 0.75);
}

void MainWindow::updateActions()
{

}

void MainWindow::createActions()
{
    openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon(":/res/images/document-open.svg"));
    openAction->setShortcut(tr("Ctrl+O"));
    connect(openAction, SIGNAL(triggered()),
            this, SLOT(open()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()),
            this, SLOT(close()));

    zoomInAction = new QAction(tr("Zoom In"), this);
    zoomInAction->setIcon(QIcon(":/res/images/zoomin.png"));
    zoomInAction->setShortcut(tr("Ctrl+["));
    connect(zoomInAction, SIGNAL(triggered()),
            this, SLOT(zoomIn()));

    zoomOutAction = new QAction(tr("Zoom Out"), this);
    zoomOutAction->setIcon(QIcon(":/res/images/zoomout.png"));
    zoomOutAction->setShortcut(tr("Ctrl+]"));
    connect(zoomOutAction, SIGNAL(triggered()),
            this, SLOT(zoomOut()));
}

void MainWindow::createMenus()
{

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openAction);

    viewToolBar = addToolBar(tr("View"));
    viewToolBar->addAction(zoomInAction);
    viewToolBar->addAction(zoomOutAction);
}

void MainWindow::loadUnitLib()
{
    if (!unitTypePin.isEmpty())
        return;
    QFile file(":/lib/unitType");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("RABBIT"),
                             tr("Cannot find file %1:\n%2")
                             .arg(file.fileName())
                             .arg(file.errorString()));
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList type = line.split("\t");
        if (type.size() >= 4) {
            QString typeName = type.takeFirst();
            QVector<int> numbers(4);
            for (int i = 0; i < 4; i++)
                numbers[i] = type.takeFirst().toInt();

            unitTypePin[typeName] = numbers;
        }
    }
    file.close();
}

bool MainWindow::loadFile(const QString &fileName)
{
    if (!readFile(fileName)) {
        statusBar()->showMessage(tr("Loading canceled"), 2000);
        return false;
    }

    generateUnitItem();
    statusBar()->showMessage(tr("File loaded"), 2000);
    return true;
}

bool MainWindow::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("RABBIT"),
                             tr("Cannot read file %1:\n%2")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        processLine(line);
    }
    file.close();
    return true;
}

void MainWindow::processLine(QString &line)
{
    QList<QString> info = line.split("\t");
    UnitItem *item = new UnitItem;

    QString bbCoord;
    if (info.count() < 2) {
        return;
    }
    item->setUnitName(info.takeFirst());
    item->setUnitType(info.takeFirst());
    item->setUnitPinNumber(unitTypePin);
    item->setUnitHOffset(unitTypePin);
    item->setUnitWOffset(unitTypePin);
    item->setUnitPinGap(unitTypePin);

    if (info.size() < item->getPinNumber()) {
        QMessageBox::warning(this, tr("Rabbit"), tr("File Error."));
        return;
    }
    for (int i = 0; i < item->getPinNumber(); i++) {
        bbCoord = info.takeFirst();
        item->setUnitPinPos(bbCToPos(bbCoord));
    }

    item->setUnitPinCoor(item->getUnitPinPos(0));
    for (int i = 1; i < item->getPinNumber(); i++) {
        item->setUnitPinCoor(QPointF(item->getUnitPinPos(0).x() +
                                     9 * item->getUnitPinGap() * i,
                                     item->getUnitPinPos(0).y()));
    }

    units.append(item);
}

void MainWindow::generateUnitItem()
{
    QVectorIterator<UnitItem *> i(units);
    while (i.hasNext()) {
        addUnitItem(i.next());
    }
}

void MainWindow::addUnitItem(UnitItem *unit)
{
    unit->setPos(unit->getUnitPinPos(0).x() - unit->getUnitWOffset(),
                 unit->getUnitPinPos(0).y() - unit->getUnitHOffset());
    unit->setPixmap(QPixmap(":/res/element/" + unit->getUnitType() + ".svg"));
    scene->addItem(unit);

    for (int i = 0; i < unit->getPinNumber(); i++) {
        QGraphicsLineItem *line = new QGraphicsLineItem(
                unit->getUnitPinCoor(i).x() - 1, unit->getUnitPinCoor(i).y() + 1,
                unit->getUnitPinPos(i).x() - 1, unit->getUnitPinPos(i).y() + 1);
        QPen pen(Qt::darkGray);
        pen.setWidth(2);
        line->setPen(pen);
        line->setZValue(9999);
        scene->addItem(line);
    }

}

QPointF MainWindow::bbCToPos(QString bbCoord)
{
    int x, y;
    char xx;
    QChar *toX = bbCoord.data();
    QString toY;
    bool ok;

    if (toX->isLetter()) {
        *toX = toX->toLower();
    }
    xx = toX->toAscii();
    if ((xx == 'x') || (xx == 'y')) {
        y = 8 + 9 * (xx - 'x');
        toY = bbCoord.remove(0, 1);
        int extra;
        if (toY.toInt(&ok, 10) % 5)
            extra = (toY.toInt(&ok, 10) / 5);
        else
            extra = (toY.toInt(&ok, 10) / 5 - 1);
        if (toY.toInt(&ok, 10) > 25)
            extra += 1;
        x = 24 + 9 * (toY.toInt(&ok, 10) + extra);
        //x = 24 + 9 * (toY.toInt(&ok, 10));
    }
    else if (((xx - 'a') >= 0) && ((xx - 'e') <= 0)) {
        y = 44 + 9 * (xx - 'a');
        toY = bbCoord.remove(0, 1);
        x = 24 + 9 * (toY.toInt(&ok, 10));
    }
    else if (((xx - 'f') >= 0) && ((xx - 'j') <= 0)) {
        y = 62 + 9 * (xx - 'a');
        toY = bbCoord.remove(0, 1);
        x = 24 + 9 * (toY.toInt(&ok, 10));
    }
    else {
        QMessageBox::warning(this, tr("Rabbit"),
                             tr("File Syntax Error."));
        return QPointF(0, 0);
    }

    if (ok == false) {
        QMessageBox::warning(this, tr("Rabbit"),
                             tr("File Syntax Error."));
        return QPointF(0, 0);
    }

    return QPointF(x, y);
}
