#
# move_hexa_ms.pro
# tmake project file
#
TEMPLATE	= app
CONFIG		= opengl debug
DEFINES		= DAMPED # BENCHMARK
INCLUDEPATH	= .
LIBS		+= -lglut
#
SOURCES		= move_hexa_ms.C
TARGET		= move_hexa_ms
