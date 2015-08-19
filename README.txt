Controlling Anisotropy in Mass-Spring Systems.
David Bourguignon and Marie-Paule Cani.
In Proceedings of the Eleventh Eurographics Workshop on Computer Animation
and Simulation, pp. 113-123, Springer-Verlag, Berlin, 2000.
--------------------------------------------------------------------------

License
-------
Copyright (c) 2000 iMAGIS-GRAVIR / IMAG-INRIA

Requirements
------------
glut-3.7 (http://freeglut.sourceforge.net/)
OpenGL-1.2 (http://www.opengl.org/)
tmake-2.12 (http://tmake.sourceforge.net/), for generating makefiles

Input
-----
MESH format specifying volume mesh geometry (3D points and tetrahedra or
hexahedra).
FACES format specifying surface mesh geometry (3D points and triangles or
quadrangles). This format is used for rendering purposes.

Compiling
---------
With gcc-3.3.1 under linux-2.4, setup the makefile and type 'make'. There
are test programs in subdirectories, for example animal/geometry/test.

This program does not currently compile with msvc-13.10.3052 under mswinxp.
This is mostly due to the heavy use of arcane template features. However,
you can try compiling this program with mingw (http://mingw.sourceforge.net/),
an excellent mswin port of gcc that produces binaries in native mswin format.

Manual
------
There is a command-line help: you can reach it by pressing the "h" key. It
describes the mouse and keyboard commands.

Enjoy! And please send me your complaints/comments/congrats. Thanks!

http://www.davidbourguignon.net/
