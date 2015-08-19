#
# move_hexa.pro
# tmake project file
#
TEMPLATE	= app
CONFIG		= opengl debug
DEFINES		= ALTERN DAMPED CONSTVOL # SURFACE BENCHMARK
INCLUDEPATH	= .
LIBS		+= -lglut
#
SOURCES		= intersect_triangle.c move_hexa.C
TARGET		= move_hexa
