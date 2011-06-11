#include <cmath>
#include <QtGui>
#include <QGraphicsSvgItem>
#include <QGraphicsRectItem>

#include "rgraphicsview.h"
#include "unit.h"

RGraphicsView::RGraphicsView(QWidget *parent)
    :QGraphicsView(parent)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    QPixmap tilePixmap(64, 64);
    tilePixmap.fill(Qt::white);
    QPainter tilePainter(&tilePixmap);
    QColor color(220, 220, 220);
    tilePainter.fillRect(0, 0, 32, 32, color);
    tilePainter.fillRect(32, 32, 32, 32, color);
    tilePainter.end();

    setBackgroundBrush(tilePixmap);

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
//    if (!file.exists())
//        return false;

    QFile file(fileName);
    QGraphicsScene *s = scene();

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("RABBIT"),
                             tr("Cannot read file %1:\n%2")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    QGraphicsItem *bread1 = new QGraphicsSvgItem("./parts/breadboard.svg");
    QGraphicsItem *bread2 = new QGraphicsSvgItem("./parts/breadboard.svg");
    QGraphicsItem *bread3 = new QGraphicsSvgItem("./parts/breadboard.svg");
    bread1->setVisible(true);
    s->addItem(bread1);
    bread2->setY(bread2->boundingRect().height());
    s->addItem(bread2);
    bread3->setY(bread3->boundingRect().height() * 2);
    s->addItem(bread3);
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
    s->addItem(outline1);
    s->addItem(outline2);
    s->addItem(outline3);



    QTextStream stream(&file);
    while (!stream.atEnd()) {
        Unit *unit = new Unit();
        QString line = stream.readLine();

        QList<QString> data = line.split(" ");
        if (data.count() < 3) {
            QMessageBox::critical(this, tr("RABBIT"),
                                  tr("File Syntax Error"));
            return false;
        }

        else {
            unit->setUnitType(data.takeFirst());
            unit->setUnitName(data.takeFirst());
            unit->setUnitValue(data.takeFirst());
        }

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
