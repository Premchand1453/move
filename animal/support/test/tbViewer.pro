#
# tbViewer.pro
# qmake project file
#
TEMPLATE	= app
CONFIG		= opengl warn_on debug
INCLUDEPATH	= ../../..
LIBS		+= -lglui -lglut -lGLU
SOURCES		= tbViewer_test.C
TARGET		= tbViewer_test
