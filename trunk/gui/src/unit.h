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

unit.h
@Author: Running Teeth <running_teeth@sina.com>

********************************************************************/

#ifndef UNIT_H
#define UNIT_H

#include <QString>
#include <QVector>
#include <QPointF>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSvgItem>

#define VOFFSET 10      //
#define HOLEGAP 9       //
#define SLOTGAP 18      //
#define TOLEFTEDGE 23   //
#define POWERTOEDGE 8   //
#define GNDTOEDGE 10    //
#define TOTOPEDGE 44    //
#define BOARDWIDTH 190  //

struct Show {
    bool m_showResistor;
    bool m_showCapacitor;
    bool m_showTransistor;
    bool m_showIC;
    bool m_showLabel;
    bool m_hideSelected;
};

class Unit : public QGraphicsItemGroup
{
public:
    Unit();

    void unitAdd();
    void setUnitType(const QString &type);
    void setUnitName(const QString &name);
    void setUnitValue(const QString &value);
    void appendUnitPin(QString pin);
    void drawPins();
    void setUnitWireVisible(bool visible);
    void setLabelVisible(bool visible);
    void setTypeVisible(struct Show *show);
    void setSelectVisible(struct Show *show);
    void setWasSelected(bool wasSelected);
    int getZ();
    bool wasSelected();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPointF bbCoordConvert(QString &bbCoord);
    void addResistor();
    void addCapacitor();
    void addTransistor();
    void addIC();
    void addWire();
    void addLabel();

    QGraphicsSvgItem *m_svg;
    QList<QGraphicsLineItem *> m_wire;
    QGraphicsTextItem *m_label;
    QString m_unitType;
    QString m_unitName;
    QString m_unitValue;
    QString m_unitLabel;
    QVector<QPointF> m_unitPin;
    qreal m_minPinY;
    double m_angle;
    int m_z;
    bool m_wasSelected;
};

#endif // UNIT_H
