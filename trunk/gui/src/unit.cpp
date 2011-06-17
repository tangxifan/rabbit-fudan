#include <QtGui>
#include <QObject>
#include <QString>
#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <QFile>
#include <cmath>

#include "unit.h"

Unit::Unit()
{
    setFlags(ItemIsSelectable);
    m_wasSelected = false;
    m_minPinY = 9999;
}

void Unit::unitAdd()
{
    m_svg = new QGraphicsSvgItem();

    if (m_unitType.contains("resistor", Qt::CaseInsensitive))
        addResistor();
    if (m_unitType.contains("capacitor", Qt::CaseInsensitive))
        addCapacitor();
    if (m_unitType.contains("transistor", Qt::CaseInsensitive))
        addTransistor();
    if (m_unitType.contains("IC", Qt::CaseSensitive))
        addIC();
    if (m_unitType.contains("wire", Qt::CaseInsensitive))
        addWire();
    m_svg->setVisible(true);
    setUnitWireVisible(true);
}

void Unit::unitTypeParser()
{
    QList<QString> data = m_unitType.split(" ");
    m_unitName.clear();

    if (m_unitType.contains("resistor", Qt::CaseInsensitive)) {
        m_unitName.append("resistor");
    }
    else if (m_unitType.contains("wire", Qt::CaseInsensitive)) {
        m_unitName.append("wire");
    }
    else if (m_unitType.contains("capacitor", Qt::CaseInsensitive)) {
        if (m_unitType.contains("ceramic", Qt::CaseInsensitive)) {
            m_unitName.append("ceramic_capacitor");
        }
        else if (m_unitType.contains("tantalum", Qt::CaseInsensitive)) {
            m_unitName.append("capacitor_tantalum");
        }
        else if (m_unitType.contains("electrolytic", Qt::CaseInsensitive)) {
            m_unitName.append("electrolytic_capacitor");
        }
        else {
            m_unitName.append("ceramic_capacitor");
        }
    }
    else if (m_unitType.contains("transistor", Qt::CaseInsensitive)) {
        if (m_unitType.contains("pnp", Qt::CaseInsensitive)) {
            m_unitName.append("basic_transistor_pnp");
        }
        else {
            m_unitName.append("basic_transistor_npn");
        }
    }
    else {
        m_unitName = "IC";
    }

    QRegExp rx("^\\d+(k|M|T|m|u|p|n|f)?");
    for (; data.count(); data.removeFirst()) {
        if (rx.indexIn(data.first()) >= 0) {
            m_unitValue = "_" + data.first().left(rx.matchedLength());
        }
    }

    m_unitName.append(m_unitValue);
}

void Unit::setUnitType(const QString &type)
{
    if (m_unitType.isEmpty())
        m_unitType = type;
    else
        m_unitType.append(" " + type);
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
    QPointF pinPos = bbCoordConvert(pin);
    if (pinPos.y() < m_minPinY)
        m_minPinY = pinPos.y();
    m_unitPin.append(pinPos);
}

QPointF Unit::bbCoordConvert(QString &bbCoord)
{
    int x, y = 0;
    char row;
    QString column;
    bool ok;

    //Decide which board
    if (!bbCoord[0].isNumber()) {
        return QPointF(0, 0);
    }
    y += (bbCoord[0].toAscii() - '0') * BOARDWIDTH;
    bbCoord.remove(0, 1);

    //Decide the position
    row = bbCoord[0].toLower().toAscii();
    if (!bbCoord[0].isLetter()) {
        //QMessageBox::warning(this, QString("RABBIT"), QString("Routed File Syntax Error."));
        return QPointF(0, 0);
    }

    if ((row == 'x') || (row == 'y')) {
        y += POWERTOEDGE + HOLEGAP * (row - 'x');
        column = bbCoord.remove(0, 1);
        int extra;
        if (column.toInt(&ok, 10) % 5)
            extra = (column.toInt(&ok, 10) / 5);
        else
            extra = (column.toInt(&ok, 10) / 5 - 1);
        if (column.toInt(&ok, 10) > 25)
            extra += 1;
        x = TOLEFTEDGE + HOLEGAP * (column.toInt(&ok, 10) + extra);
        //x = 24 + 9 * (toY.toInt(&ok, 10));
    }
    else if (((row - 'a') >= 0) && ((row - 'e') <= 0)) {
        y += TOTOPEDGE + HOLEGAP * (row - 'a');
        column = bbCoord.remove(0, 1);
        x = TOLEFTEDGE + HOLEGAP * (column.toInt(&ok, 10));
    }
    else if (((row - 'f') >= 0) && ((row - 'j') <= 0)) {
        y += TOTOPEDGE + SLOTGAP + HOLEGAP * (row - 'a');
        column = bbCoord.remove(0, 1);
        x = TOLEFTEDGE + HOLEGAP * (column.toInt(&ok, 10));
    }
    else if ((row == 'v') || (row == 'w')) {
        y += BOARDWIDTH - GNDTOEDGE + HOLEGAP * (row - 'w');

        column = bbCoord.remove(0, 1);
        int extra;
        if (column.toInt(&ok, 10) % 5)
            extra = (column.toInt(&ok, 10) / 5);
        else
            extra = (column.toInt(&ok, 10) / 5 - 1);
        if (column.toInt(&ok, 10) > 25)
            extra += 1;
        x = TOLEFTEDGE + HOLEGAP * (column.toInt(&ok, 10) + extra);
        //x = 24 + 9 * (toY.toInt(&ok, 10));
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

void Unit::setUnitWireVisible(bool visible)
{
    QGraphicsLineItem *wires = new QGraphicsLineItem();
    foreach(wires, m_wire)
        wires->setVisible(visible);
}

void Unit::setTypeVisible(Show *show)
{
    if (m_unitType.contains("resistor", Qt::CaseInsensitive)) {
        m_svg->setVisible(show->m_showResistor);
        return;
    }
    if (m_unitType.contains("capacitor", Qt::CaseInsensitive)) {
        m_svg->setVisible(show->m_showCapacitor);
        return;
    }
    if (m_unitType.contains("transistor", Qt::CaseInsensitive)) {
        m_svg->setVisible(show->m_showTransistor);
        return;
    }
    if (m_unitType.contains("IC", Qt::CaseInsensitive)) {
        m_svg->setVisible(show->m_showIC);
        return;
    }
}

void Unit::setSelectVisible(Show *show)
{
    setUnitWireVisible(!(show->m_hideSelected));
    m_svg->setVisible(!(show->m_hideSelected));

}

void Unit::setWasSelected(bool wasSelected)
{
    m_wasSelected = wasSelected;
}

int Unit::getZ()
{
    return m_z;
}

bool Unit::wasSelected()
{
    return m_wasSelected;
}

QRectF Unit::boundingRect() const
{
    QRectF rect = m_svg->boundingRect();
    rect.moveTopLeft(m_svg->pos());
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
        rect.moveTo(0, 0);
        painter->translate(m_svg->pos());
        painter->rotate(m_angle);
        painter->drawRect(rect);
    }
}

void Unit::addResistor()
{
    QSvgRenderer *renderer = new QSvgRenderer(QString("../parts/svg/breadboard/")
                                              .append(m_unitName + ".svg"));
\
    m_svg->setSharedRenderer(renderer);

    QPointF point = QPointF((m_unitPin.at(1).x()+m_unitPin.at(0).x()) / 2 - m_svg->boundingRect().width() / 2,
                            (m_unitPin.at(1).y()+m_unitPin.at(0).y()) / 2 - m_svg->boundingRect().height() / 2);
    m_angle = std::atan((m_unitPin.at(1).x()-m_unitPin.at(0).x()) /
                             (m_unitPin.at(1).y()-m_unitPin.at(0).y()));
    m_angle = 90 - 180 / 3.14 * m_angle;

    m_svg->setPos(point);

    //rotate around its centre
    m_svg->translate(m_svg->boundingRect().width() / 2,
                       m_svg->boundingRect().height() / 2);
    m_svg->rotate(m_angle);
    m_svg->translate(-m_svg->boundingRect().width() / 2,
                               -m_svg->boundingRect().height() / 2);
    m_svg->setZValue(2);

    QGraphicsLineItem *wires =
            new QGraphicsLineItem(m_unitPin.at(0).x(), m_unitPin.at(0).y(),
                                  m_unitPin.at(1).x(), m_unitPin.at(1).y());
    QPen wire(Qt::gray, 3, Qt::SolidLine, Qt::RoundCap);//color not sure.
    wire.setCosmetic(false);
    wires->setPen(wire);
    wires->setZValue(1);
    addToGroup(wires);
    m_wire.append(wires);

    m_z = 2;

    addToGroup(m_svg);
}

void Unit::addCapacitor()
{
    QSvgRenderer *renderer = new QSvgRenderer(QString("../parts/svg/breadboard/")
                                              .append(m_unitName + ".svg"));
    m_svg->setSharedRenderer(renderer);

    QPointF point = QPointF((m_unitPin.at(1).x()+m_unitPin.at(0).x()) / 2 - m_svg->boundingRect().width() / 2,
                            m_minPinY - m_svg->boundingRect().height() - VOFFSET);
    m_svg->setPos(point);
    m_z = (int)m_unitPin.at(0).y();
    m_angle = 0;

    //add connectors to the unit(NOT WIRE)
    //left connector
    QGraphicsLineItem *wires =
            new QGraphicsLineItem(m_unitPin.at(0).x(), m_unitPin.at(0).y(),
                                  boundingRect().center().x() - HOLEGAP / 2, boundingRect().bottom());
    QPen wire(Qt::gray, 3, Qt::SolidLine, Qt::RoundCap);//color not sure.
    wire.setCosmetic(false);
    wires->setPen(wire);
    wires->setZValue(1);
    addToGroup(wires);
    m_wire.append(wires);

    wires = new QGraphicsLineItem(m_unitPin.at(1).x(), m_unitPin.at(1).y(),
                                   boundingRect().center().x() + HOLEGAP / 2, boundingRect().bottom());
    wire.setCosmetic(false);
    wires->setPen(wire);
    wires->setZValue(1);
    addToGroup(wires);
    m_wire.append(wires);

    addToGroup(m_svg);
}

void Unit::addTransistor()
{
    QSvgRenderer *renderer = new QSvgRenderer(QString("../parts/svg/breadboard/")
                                              .append(m_unitName + ".svg"));
    m_svg->setSharedRenderer(renderer);

    QPointF point = QPointF(m_unitPin.at(1).x() - m_svg->boundingRect().width() / 2
                            , m_minPinY - m_svg->boundingRect().height() - VOFFSET);
    m_svg->setPos(point);
    m_z = (int)m_unitPin.at(0).y();

    m_angle = 0;

    QGraphicsLineItem *wires =
            new QGraphicsLineItem(m_unitPin.at(0).x(), m_unitPin.at(0).y(),
                                  boundingRect().center().x() - HOLEGAP, boundingRect().bottom());
    QPen wire(Qt::gray, 3, Qt::SolidLine, Qt::RoundCap);//color not sure.
    wire.setCosmetic(false);
    wires->setPen(wire);
    wires->setZValue(1);
    addToGroup(wires);
    m_wire.append(wires);

    wires = new QGraphicsLineItem(m_unitPin.at(1).x(), m_unitPin.at(1).y(),
                                   boundingRect().center().x(), boundingRect().bottom());
    wire.setCosmetic(false);
    wires->setPen(wire);
    wires->setZValue(1);
    addToGroup(wires);
    m_wire.append(wires);

    wires = new QGraphicsLineItem(m_unitPin.at(2).x(), m_unitPin.at(2).y(),
                                   boundingRect().center().x() + HOLEGAP, boundingRect().bottom());
    wire.setCosmetic(false);
    wires->setPen(wire);
    wires->setZValue(1);
    addToGroup(wires);
    m_wire.append(wires);

    addToGroup(m_svg);
}

void Unit::addIC()
{
    QSvgRenderer *render = new QSvgRenderer(QString("../parts/svg/breadboard/")
                                            .append(m_unitName + ".svg"));
    m_svg->setSharedRenderer(render);

    QPointF point = QPointF(m_unitPin.at(0).x() - m_svg->boundingRect().width() / (m_unitPin.count() + 1)
                            , m_unitPin.at(0).y());
    m_svg->setPos(point);
    m_z = 0;

    m_angle = 0;
    addToGroup(m_svg);
}

void Unit::addWire()
{
    QGraphicsLineItem *wires =
            new QGraphicsLineItem(m_unitPin.at(0).x(), m_unitPin.at(0).y(),
                                  m_unitPin.at(1).x(), m_unitPin.at(1).y());
    QPen wire(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap);//color not sure.
    wire.setCosmetic(false);
    wires->setPen(wire);
    wires->setZValue(1);
    addToGroup(wires);
    m_wire.append(wires);

    m_z = 1;

    addToGroup(wires);
}
