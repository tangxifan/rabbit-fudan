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

rgraphicsscene.h
@Author: Running Teeth <running_teeth@sina.com>

********************************************************************/

#ifndef RGRAPHICSSCENE_H
#define RGRAPHICSSCENE_H

#include <QGraphicsScene>

class RGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    RGraphicsScene(QWidget *parent = 0);
};

#endif // RGRAPHICSSCENE_H
