#
# move_tetra.pro
# tmake project file
#
TEMPLATE	= app
CONFIG		= opengl debug
DEFINES		= ALTERN DAMPED CONSTVOL # SURFACE VOLINFO BENCHMARK MEASURE
INCLUDEPATH	= .
LIBS		+= -lglut
#
SOURCES		= intersect_triangle.c move_tetra.C
TARGET		= move_tetra
