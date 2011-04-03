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

#include "unititem.h"

UnitItem::UnitItem(QWidget *parent)
{

}

void UnitItem::setUnitName(QString name)
{
    unitName = name;
}

void UnitItem::setUnitType(QString type)
{
    unitType = type;
}

void UnitItem::setUnitPinPos(QPointF pinPos)
{
    unitPinPos.append(pinPos);
}

void UnitItem::setUnitPinCoor(QPointF pinCoor)
{
    unitPinCoor.append(pinCoor);
}

void UnitItem::setUnitPinNumber(QMap<QString, QVector<int> > unitTypePin)
{
    unitPinNumber = unitTypePin.value(unitType)[0];
}

void UnitItem::setUnitHOffset(QMap<QString, QVector<int> > unitTypePin)
{
    unitHOffset = unitTypePin.value(unitType)[1];
}

void UnitItem::setUnitWOffset(QMap<QString, QVector<int> > unitTypePin)
{
    unitWOffset = unitTypePin.value(unitType)[2];
}

void UnitItem::setUnitPinGap(QMap<QString, QVector<int> > unitTypePin)
{
    unitPinGap = unitTypePin.value(unitType)[3];
}

int UnitItem::getPinNumber()
{
    return unitPinNumber;
}

QString UnitItem::getUnitType()
{
    return unitType;
}

QPointF UnitItem::getUnitPinPos(int i)
{
    return unitPinPos[i];
}

QPointF UnitItem::getUnitPinCoor(int i)
{
    return unitPinCoor[i];
}

int UnitItem::getUnitHOffset()
{
    return unitHOffset;
}

int UnitItem::getUnitWOffset()
{
    return unitWOffset;
}

int UnitItem::getUnitPinGap()
{
    return unitPinGap;
}
