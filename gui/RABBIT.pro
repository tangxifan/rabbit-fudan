#
#
#
#

QT += core \
    gui \
    svg \
    xml \

RESOURCES += RABBITresources.qrc
    include(pri/maingui.pri)
    include(pri/autoroute.pri)

TARGET = RABBIT
TEMPLATE = app
