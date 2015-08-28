#
# move_tetra.pro
# qmake project file
#
TEMPLATE	= app
CONFIG		= opengl warn_on debug
DEFINES		= ALTERN DAMPED CONSTVOL # SURFACE VOLINFO BENCHMARK MEASURE
INCLUDEPATH	= .
LIBS		+= -lglut -lGLU
SOURCES		= intersect_triangle.c move_tetra.C
TARGET		= move_tetra
