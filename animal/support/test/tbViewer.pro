#
# tbViewer.pro
# tmake project file
#
TEMPLATE	= app
CONFIG		= opengl warn_on debug
INCLUDEPATH	= ../../..
LIBS		+= -lglui -lglut
#
SOURCES		= tbViewer_test.C
TARGET		= tbViewer_test
