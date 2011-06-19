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

rgraphicsview.cpp
@Author: Running Teeth <running_teeth@sina.com>

********************************************************************/

#ifndef RGRAPHICSVIEW_H
#define RGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QObject>
#include <QList>
#include <QFile>

#include "unit.h"

class RGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    RGraphicsView(QWidget *parent = 0);

    void drawBackground(QPainter *painter, const QRectF &rect);

    bool readFile(const QString &fileName);
    void updateItemShow();
    void drawBreadboard();

public slots:
    void setShowResistor(bool showResistor);
    void setShowDiode(bool showDiode);
    void setShowCapacitor(bool showCapacitor);
    void setShowTransistor(bool showTransistor);
    void setShowIC(bool showIC);
    void setShowLabel(bool showLabel);
    void setHideSelected(bool hideSelected);

protected:
    void wheelEvent(QWheelEvent *event);

private:
    QList<Unit *> m_unitList;
    QGraphicsSvgItem *bread1;
    QGraphicsSvgItem *bread2;
    QGraphicsSvgItem *bread3;
    struct Show *m_show;
};

#endif // RGRAPHICSVIEW_H
