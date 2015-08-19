#
# move_tetra_ms.pro
# tmake project file
#
TEMPLATE	= app
CONFIG		= opengl debug
DEFINES		= DAMPED # VOLINFO BENCHMARK MEASURE
INCLUDEPATH	= .
LIBS		+= -lglut
#
SOURCES		= move_tetra_ms.C
TARGET		= move_tetra_ms
