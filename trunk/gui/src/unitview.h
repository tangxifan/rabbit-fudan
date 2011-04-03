/*******************************************************************

Part of the RAABIT project

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

#ifndef UNITVIEW_H
#define UNITVIEW_H

#include <QGraphicsView>

class UnitView : public QGraphicsView
{
    Q_OBJECT

public:
    UnitView(QWidget *parent = 0);

protected:
    void wheelEvent(QWheelEvent *event);
};

#endif // UNITVIEW_H
