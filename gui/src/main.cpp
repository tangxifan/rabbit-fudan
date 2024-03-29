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

main.cpp
@Author: Running Teeth <running_teeth@sina.com>
********************************************************************/

#include <QtGui/QApplication>
#include <QMessageBox>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    try {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        return a.exec();
    }
    catch (char *exception) {
            QMessageBox msgBox;
            msgBox.setText("Some errors occurred!");
            return msgBox.exec();
    }
}
