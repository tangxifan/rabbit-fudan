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

#ifndef UNITITEM_H
#define UNITITEM_H

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>

class UnitItem : public QGraphicsPixmapItem
{
    Q_DECLARE_TR_FUNCTIONS(UnitItem)

public:
    /* This is the construct function of the class
     * UnitItem which refer to an electric unit
     * displayed on the screen.
     */
    UnitItem(QWidget *parent = 0);

    void setUnitName(QString name);
    void setUnitType(QString type);
    /* This function is used to set the position of
     * the pins of the unit, which is given in the
     * routed netlist-like file.
     */
    void setUnitPinPos(QPointF pinPos);
    /* This function is used to set the breadboard
     * coordinate (eg: X12, C33) of the pins of the
     * unit, which is decided by the size of the unit
     * itself.
     */
    void setUnitPinCoor(QPointF pinCoor);
    void setUnitPinNumber(QMap<QString, QVector<int> > unitTypePin);
    /* The following two functions are used to set
     * the height and width offset of a unit, which
     * is used to make the image better fit the
     * breadboard background.
     */
    void setUnitHOffset(QMap<QString, QVector<int> > unitTypePin);
    void setUnitWOffset(QMap<QString, QVector<int> > unitTypePin);
    /* This function is used to set the size of a unit,
     * which is shown by the gap between pins.
     */
    void setUnitPinGap(QMap<QString, QVector<int> > unitTypePin);

    int getPinNumber();
    QString getUnitType();
    QPointF getUnitPinPos(int i);
    QPointF getUnitPinCoor(int i);
    int getUnitHOffset();
    int getUnitWOffset();
    int getUnitPinGap();

private:
    QString unitName;
    QString unitType;
    QVector<QPointF> unitPinPos;
    QVector<QPointF> unitPinCoor;
    int unitHOffset;
    int unitWOffset;
    int unitPinGap;

    int unitPinNumber;
};

#endif // UNITITEM_H
