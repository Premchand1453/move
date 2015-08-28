#
# move_tetra_ms.pro
# qmake project file
#
TEMPLATE	= app
CONFIG		= opengl warn_on debug
DEFINES		= DAMPED # VOLINFO BENCHMARK MEASURE
INCLUDEPATH	= .
LIBS		+= -lglut -lGLU
SOURCES		= move_tetra_ms.C
TARGET		= move_tetra_ms
