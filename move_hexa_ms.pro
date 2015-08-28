#
# move_hexa_ms.pro
# qmake project file
#
TEMPLATE	= app
CONFIG		= opengl warn_on debug
DEFINES		= DAMPED # BENCHMARK
INCLUDEPATH	= .
LIBS		+= -lglut -lGLU
SOURCES		= move_hexa_ms.C
TARGET		= move_hexa_ms
