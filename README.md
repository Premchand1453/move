Source code from the paper
--------------------------
Controlling Anisotropy in Mass-Spring Systems.
David Bourguignon and Marie-Paule Cani.
In: Proceedings of the Eleventh Eurographics Workshop on Computer Animation and Simulation, pp. 113-123, Springer-Verlag, Berlin, 2000.

License
-------
Copyright (c) 2000 iMAGIS-GRAVIR / IMAG-INRIA

Requirements
------------
GLUI v2.36 (http://glui.sourceforge.net/)
GLUT v3.7 (http://freeglut.sourceforge.net/)
OpenGL v1.2 (http://www.opengl.org/)
qmake v5.5 (http://www.qt.io/)
gnuplot v4.6 (http://www.gnuplot.info/)

Input
-----
MESH format specifying volume mesh geometry (3D points and tetrahedra or hexahedra).
FACES format specifying surface mesh geometry (3D points and triangles or quadrangles). This format is used for rendering purposes.

Compiling
---------
With g++ v4.9.2, setup the makefile using qmake v5.5 and type 'make'. There are test programs in subdirectories, for example animal/geometry/test.

Visualizing results
-----------------
Type gnuplot -persist [scriptname]

Online help
-----------
There is an online help: you can reach it by pressing the "h" key within the animation window. It describes the mouse and keyboard commands. Examples meshes are available in examples/hexa and examples/tetra.

Enjoy! And please send me your complaints/comments/congrats using the contact address on http://www.davidbourguignon.net/

Thanks!
