===========================================================================
    CONSOLE APPLICATION : CG_Assignment1 Project Overview
===========================================================================

This file contains a summary of what you will find in each of the files 
that make up the CG_Assignment1 application.

CG_Assignment1.cpp
    This is the main application source file. The program initializes the
OpenGLWindow, and compiles/attachs the shaders. The shapes are created and
bound to the window, and the Camera is attached. The game loop is then
called, which polls for key events, renders the shapes and then draws them.
The animations are handled in the game loop, by changing the rotation
values for the required objects. The game loop ends with swapping the
buffers, and has a clean up after exiting the loop.

OpenGLWindow.cpp/h
    This is a static class that manages all of the OpenGL calls, shaders
and shapes. Static public variables are used to control the animations of
the application, and define important constraints for computation. The
callback functions for key, mouse and cursor are also managed in this class
and implement all of the key functions described below. This class compiles,
and attachs the shaders, adds and binds buffers, and renders and draws
shapes in the window. This class also manages the aspect ratio when resizing
the window.

Camera.cpp/h
    This class manages the Camera in the scene. Using its position, direction
and up vector, the proper rotations are made to modify the orientation of
the camera, and the world. This class is responsible for creating the View 
matrix used by the OpenGL calls.

Shape.h
    This struct is used to maintain the parameters of a shape. The vertices,
scale, rotation and translation values are all contained in one structure for
simple manipulation.

ShapeGenerator.cpp/h
    This static class generates shapes for the OpenGL program. The three
shapes in this program include Cube, Line and Teddy. The functions construct
the vertices, associate a color, assign a scale, rotation and translation,
and return the shape for the application to use.

objloader.cpp/h
    This file contains the loadOBJ function implemented in the COMP371 Lab
sessions. The code was modified slightly to function with my implementation
of vertices.

Vertex.h
    This struct is used to define the necessary values for each vertex and
its attributes. For this application, the position and color of the vertex
are all that are necessary.

vertex.shader
    This shader code is used from the COMP371 Lab sessions, with modifications
to accept the color of the vertex and pass it to the fragment shader.

fragment.shader
    This shader code is used from the COMP371 Lab sessions, with modifications
to accept the color from the vertex shader, and use that to define the color
of the vertex.

////////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named CG_Assignment1.pch and a precompiled types file named StdAfx.obj.

////////////////////////////////////////////////////////////////////////////////
Application Controls:

This section will outline the controls of the application to allow the user
to navigate the functionality.

KEY [SPACE] | ACTION [PRESS]
    Toggles the animation between static and dynamic.

KEY [S] | ACTION [PRESS]
    Scales the spinner. Uppercase S scales larger, lowercase s scales smaller.

KEY [R] | ACTION [PRESS]
    Scales the revolver. Uppercase S scales larger, lowercase s scales smaller.

KEY [A] | ACTION [PRESS]
    Toggles between changing the world orientation or the axis of rotation.

KEY [LEFT_ARROW] | ACTION [HOLD]
    Rotates the world orientation around the Z axis (+) OR Rotates the axis
    of rotation around the X axis (-).

KEY [RIGHT_ARROW] | ACTION [HOLD]
    Rotates the world orientation around the Z axis (-) OR Rotates the axis
    of rotation around the X axis (+).

KEY [UP_ARROW] | ACTION [HOLD]
    Rotates the world orientation around the X axis (+).

KEY [DOWN_ARROW] | ACTION [HOLD]
    Rotates the world orientation around the X axis (-).

KEY [D] | ACTION [PRESS]
    Returns the orientation defaults for all axes.

KEY [P] | ACTION [PRESS]
    Renders the scene in GL_POINTS render mode.

KEY [W] | ACTION [PRESS]
    Renders the scene in GL_LINE_STRIP render mode.

KEY [T] | ACTION [PRESS]
    Renders the scene in GL_TRIANGLES render mode.

KEY [B] | ACTION [PRESS]
    Swaps the cube for teddy bear.

KEY [M] | ACTION [PRESS]
    Changes the revolver's path to modulate on sine wave.

KEY [+] | ACTION [PRESS]
    Increase the number of waves the sine path follows.

KEY [-] | ACTION [PRESS]
    Decrease the number of waves the sine path follows.

MOUSE [RIGHT_BUTTON] | ACTION [PRESS & HOLD]
    Move the mouse in the x direction to pan the camera.

MOUSE [LEFT_BUTTON] | ACTION [PRESS & HOLD]
    Move the mouse in the y direction to zoom the camera.

MOUSE [MIDDLE_BUTTON] | ACTION [PRESS & HOLD]
    Move the mouse in the y direction to tilt the camera.

////////////////////////////////////////////////////////////////////////////////
