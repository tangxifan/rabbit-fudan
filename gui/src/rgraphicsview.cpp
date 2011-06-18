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

rgraphicsview.cpp
@Author: Running Teeth <running_teeth@sina.com>

********************************************************************/

#include <cmath>
#include <QtGui>
#include <QGraphicsSvgItem>
#include <QGraphicsRectItem>
#include <QSvgRenderer>

#include "rgraphicsview.h"
#include "unit.h"

RGraphicsView::RGraphicsView(QWidget *parent)
    :QGraphicsView(parent)
{
    setCacheMode(QGraphicsView::CacheBackground);

    setScene(new QGraphicsScene(this));
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
/*
    QPixmap tilePixmap(64, 64);
    tilePixmap.fill(Qt::white);
    QPainter tilePainter(&tilePixmap);
    QColor color(220, 220, 220);
    tilePainter.fillRect(0, 0, 32, 32, color);
    tilePainter.fillRect(32, 32, 32, 32, color);
    tilePainter.end();

    setBackgroundBrush(tilePixmap);
*/
    this->drawBreadboard();

    Show * show = new Show;
    m_show = show;
    m_show->m_hideSelected = false;
    m_show->m_showResistor = true;
    m_show->m_showCapacitor = true;
    m_show->m_showTransistor = true;
    m_show->m_showIC = true;
}

void RGraphicsView::drawBackground(QPainter *p, const QRectF &)
{
    p->save();
    p->resetTransform();
    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
    p->restore();
}

bool RGraphicsView::readFile(const QString &fileName)
{
    QFile file(fileName);
    QGraphicsScene *s = scene();
    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("RABBIT"),
                             tr("Cannot read file %1:\n%2")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    //clear screen and old data
    s->clear();
    drawBreadboard();
    m_unitList.clear();

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        Unit *unit = new Unit();
        QString temp, type;
        QString line = stream.readLine();
        QList<QString> data = line.split(" ");
        QRegExp rx("^\\d[a-jA-Jv-zV-Z]?\\d+$");

        temp = data.takeFirst();
        int i = rx.indexIn(temp);
        type.clear();
        while (i < 0) {
            type.append(temp).append(" ");
            temp = data.takeFirst();
            i = rx.indexIn(temp);
        }
        unit->setUnitType(type);

        unit->appendUnitPin(temp);
        while (data.count()) {
            unit->appendUnitPin(data.takeFirst());
        }

        //add electronic parts
        unit->unitAdd();

        m_unitList.append(unit);
        s->addItem(unit);
        unit->setZValue(unit->getZ());
    }
    update();
    return true;
}

void RGraphicsView::updateItemShow()
{
    Unit *unit = new Unit();
    foreach (unit, m_unitList) {
        unit->setTypeVisible(m_show);
        unit->setLabelVisible(m_show->m_showLabel);
        if (m_show->m_hideSelected) {
            if (unit->isSelected()) {
                unit->setWasSelected(true);
                unit->setSelectVisible(m_show);
            }
            else {
                unit->setWasSelected(false);
            }
        }
        else {
            if (unit->wasSelected()) {
                unit->setWasSelected(false);
                unit->setSelectVisible(m_show);
                unit->setSelected(true);
            }
        }
    }
}

void RGraphicsView::drawBreadboard()
{
    QSvgRenderer *renderer = new QSvgRenderer(QString("../parts/svg/breadboard/breadboard.svg"));

    bread1 = new QGraphicsSvgItem;
    bread2 = new QGraphicsSvgItem;
    bread3 = new QGraphicsSvgItem;

    bread1->setSharedRenderer(renderer);
    bread2->setSharedRenderer(renderer);
    bread3->setSharedRenderer(renderer);

    scene()->addItem(bread1);
    scene()->addItem(bread2);
    bread2->setY(bread1->boundingRect().height());
    scene()->addItem(bread3);
    bread3->setY(bread1->boundingRect().height() * 2);

    QPen outline(Qt::black, 2);
    outline.setCosmetic(true);
    QGraphicsRectItem *outline1 = new QGraphicsRectItem(bread1->boundingRect());
    QGraphicsRectItem *outline2 = new QGraphicsRectItem(bread1->boundingRect());
    QGraphicsRectItem *outline3 = new QGraphicsRectItem(bread1->boundingRect());
    outline1->setPen(outline);
    outline2->setPen(outline);
    outline2->setPos(bread2->pos());
    outline3->setPen(outline);
    outline3->setPos(bread3->pos());
    scene()->addItem(outline1);
    scene()->addItem(outline2);
    scene()->addItem(outline3);
}

void RGraphicsView::setShowResistor(bool showResistor)
{
    m_show->m_showResistor = showResistor;
    updateItemShow();
}

void RGraphicsView::setShowCapacitor(bool showCapacitor)
{
    m_show->m_showCapacitor = showCapacitor;
    updateItemShow();
}

void RGraphicsView::setShowTransistor(bool showTransistor)
{
    m_show->m_showTransistor = showTransistor;
    updateItemShow();
}

void RGraphicsView::setShowIC(bool showIC)
{
    m_show->m_showIC = showIC;
    updateItemShow();
}

void RGraphicsView::setShowLabel(bool showLabel)
{
    m_show->m_showLabel = showLabel;
    updateItemShow();
}

void RGraphicsView::setHideSelected(bool hideSelected)
{
    m_show->m_hideSelected = hideSelected;
    updateItemShow();
}

void RGraphicsView::wheelEvent(QWheelEvent *event)
{
    double numDegrees = -event->delta() / 8.0;
    double numSteps = numDegrees / 15.0;
    double factor = std::pow(1.125, numSteps);
    scale(factor, factor);
}
