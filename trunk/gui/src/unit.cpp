#include <QtGui>
#include <QObject>
#include <QString>
#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <QFile>
#include <cmath>

#include "unit.h"
#include "electronicpart.h"

Unit::Unit()
{
    setFlags(ItemIsSelectable);
}

ElectronicPart * Unit::unitAdd()
{
    m_unit = new ElectronicPart();
    m_unit->svgPart = new QGraphicsSvgItem();
    //m_unit->outlinePart = new QGraphicsRectItem();
    m_unit->wirePart = new QGraphicsLineItem();

    if (m_unitType == "resistor")
        addResistor();
    if (m_unitType.contains("capacitor", Qt::CaseInsensitive))
        addCapacitor();
    if (m_unitType.contains("transistor", Qt::CaseInsensitive))
        addTransistor();
    if (m_unitType.contains("IC", Qt::CaseSensitive))
        addIC();
    return m_unit;
}

void Unit::setUnitType(const QString &type)
{
    m_unitType = type;//need to judge whether the type exists.
}

void Unit::setUnitName(const QString &name)
{
    m_unitName = name;
}

void Unit::setUnitValue(const QString &value)
{
    m_unitValue = value;
}

void Unit::appendUnitPin(QString pin)
{
    m_unitPin.append(bbCoordConvert(pin));
}

QPointF Unit::bbCoordConvert(QString &bbCoord)
{
    int x, y;
    char row = bbCoord[0].toLower().toAscii();
    QString column;
    bool ok;

    if (!bbCoord[0].isLetter()) {
        //QMessageBox::warning(this, QString("RABBIT"), QString("Routed File Syntax Error."));
        return QPointF(0, 0);
    }

    if ((row == 'x') || (row == 'y')) {
        y = 8 + 9 * (row - 'x');
        column = bbCoord.remove(0, 1);
        int extra;
        if (column.toInt(&ok, 10) % 5)
            extra = (column.toInt(&ok, 10) / 5);
        else
            extra = (column.toInt(&ok, 10) / 5 - 1);
        if (column.toInt(&ok, 10) > 25)
            extra += 1;
        x = 23 + 9 * (column.toInt(&ok, 10) + extra);
        //x = 24 + 9 * (toY.toInt(&ok, 10));
    }
    else if (((row - 'a') >= 0) && ((row - 'e') <= 0)) {
        y = 44 + 9 * (row - 'a');
        column = bbCoord.remove(0, 1);
        x = 23 + 9 * (column.toInt(&ok, 10));
    }
    else if (((row - 'f') >= 0) && ((row - 'j') <= 0)) {
        y = 62 + 9 * (row - 'a');
        column = bbCoord.remove(0, 1);
        x = 23 + 9 * (column.toInt(&ok, 10));
    }
    else {
        //QMessageBox::warning(this, "Rabbit", "File Syntax Error.");
        return QPointF(0, 0);
    }

    if (ok == false) {
        //QMessageBox::warning(this, "Rabbit", "File Syntax Error.");
        return QPointF(0, 0);
    }

    return QPointF(x, y);
}

void Unit::drawPins()
{

}

void Unit::setTypeVisible(Show *show)
{
    if (m_unitType.contains("resistor", Qt::CaseInsensitive)) {
        m_unit->svgPart->setVisible(show->m_showResistor);
        return;
    }
    if (m_unitType.contains("capacitor", Qt::CaseInsensitive)) {
        m_unit->svgPart->setVisible(show->m_showCapacitor);
        return;
    }
    if (m_unitType.contains("transistor", Qt::CaseInsensitive)) {
        m_unit->svgPart->setVisible(show->m_showTransistor);
        return;
    }
    if (m_unitType.contains("IC", Qt::CaseInsensitive)) {
        m_unit->svgPart->setVisible(show->m_showIC);
        return;
    }
}

void Unit::setSelectVisible(Show *show)
{
    m_unit->svgPart->setVisible(!(show->m_hideSelected));
}

QRectF Unit::boundingRect() const
{
    QRectF rect = m_unit->svgPart->boundingRect();
    return rect;
}

void Unit::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen(Qt::green);
    int i = 0;
    int c = m_unitPin.count();
    pen.setWidth(5);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);
    while (i < c) {
        painter->drawPoint(m_unitPin.at(i));
        i++;
    }

    if (option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
        pen.setColor(Qt::black);

        painter->setPen(pen);
        QRectF rect = boundingRect();
        painter->translate(m_unit->svgPart->pos());
        painter->rotate(m_unit->angle);
        painter->drawRect(rect);
    }
}

void Unit::addResistor()
{
    QSvgRenderer *renderer = new QSvgRenderer(QString("./parts/svg/breadboard/%1_%2.svg")
                                              .arg(m_unitType).arg(m_unitValue));
    m_unit->svgPart->setSharedRenderer(renderer);

    QPointF point = QPointF((m_unitPin.at(1).x()+m_unitPin.at(0).x()) / 2 - m_unit->svgPart->boundingRect().width() / 2,
                            (m_unitPin.at(1).y()+m_unitPin.at(0).y()) / 2 - m_unit->svgPart->boundingRect().height() / 2);
    m_unit->angle = std::atan((m_unitPin.at(1).x()-m_unitPin.at(0).x()) /
                             (m_unitPin.at(1).y()-m_unitPin.at(0).y()));
    m_unit->angle = 90 - 180 / 3.14 * m_unit->angle;

    m_unit->svgPart->setPos(point);

    //rotate around its centre
    m_unit->svgPart->translate(m_unit->svgPart->boundingRect().width() / 2,
                       m_unit->svgPart->boundingRect().height() / 2);
    m_unit->svgPart->rotate(m_unit->angle);
    m_unit->svgPart->translate(-m_unit->svgPart->boundingRect().width() / 2,
                               -m_unit->svgPart->boundingRect().height() / 2);

    m_unit->svgPart->setZValue(10);

    m_unit->wirePart->setLine(m_unitPin.at(0).x(), m_unitPin.at(0).y(),
                                            m_unitPin.at(1).x(), m_unitPin.at(1).y());
    QPen wire(Qt::gray, 3, Qt::SolidLine, Qt::RoundCap);//color not sure.
    wire.setCosmetic(false);
    m_unit->wirePart->setPen(wire);
    m_unit->wirePart->setZValue(9999);

    addToGroup(m_unit->svgPart);
    addToGroup(m_unit->wirePart);
}

void Unit::addCapacitor()
{
    QSvgRenderer *renderer = new QSvgRenderer(QString("./parts/svg/breadboard/%1.svg")
                                              .arg(m_unitType));
    m_unit->svgPart->setSharedRenderer(renderer);

    QPointF point = QPointF((m_unitPin.at(1).x()+m_unitPin.at(0).x()) / 2 - m_unit->svgPart->boundingRect().width() / 2,
                            (m_unitPin.at(1).y()+m_unitPin.at(0).y()) / 2 - m_unit->svgPart->boundingRect().height());
    m_unit->svgPart->setPos(point);
    m_unit->svgPart->setZValue((int)m_unitPin.at(0).y());

    m_unit->angle = 0;

    addToGroup(m_unit->svgPart);
    addToGroup(m_unit->wirePart);
}

void Unit::addTransistor()
{
    QSvgRenderer *renderer = new QSvgRenderer(QString("./parts/svg/breadboard/basic_%1.svg")
                                              .arg(m_unitType));
    m_unit->svgPart->setSharedRenderer(renderer);

    QPointF point = QPointF(m_unitPin.at(1).x() - m_unit->svgPart->boundingRect().width() / 2
                            , m_unitPin.at(1).y() - m_unit->svgPart->boundingRect().height());
    m_unit->svgPart->setPos(point);
    m_unit->svgPart->setZValue((int)m_unitPin.at(0).y());

    m_unit->angle = 0;

    addToGroup(m_unit->svgPart);
    addToGroup(m_unit->wirePart);
}

void Unit::addIC()
{
    QSvgRenderer *render = new QSvgRenderer(QString("./parts/svg/breadboard/%1.svg")
                                            .arg(m_unitType));
    m_unit->svgPart->setSharedRenderer(render);

    QPointF point = QPointF(m_unitPin.at(0).x() - m_unit->svgPart->boundingRect().width() / (m_unitPin.count() + 1)
                            , m_unitPin.at(0).y());
    m_unit->svgPart->setPos(point);
    m_unit->svgPart->setZValue(1);

    m_unit->angle = 0;
    addToGroup(m_unit->svgPart);
    addToGroup(m_unit->wirePart);
}

void Unit::addWire()
{

}
