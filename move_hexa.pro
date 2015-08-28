#
# move_hexa.pro
# qmake project file
#
TEMPLATE	= app
CONFIG		= opengl warn_on debug
DEFINES		= ALTERN DAMPED CONSTVOL # SURFACE BENCHMARK
INCLUDEPATH	= .
LIBS		+= -lglut -lGLU
SOURCES		= intersect_triangle.c move_hexa.C
TARGET		= move_hexa
