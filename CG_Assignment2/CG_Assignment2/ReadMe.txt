===========================================================================
    CONSOLE APPLICATION : CG_Assignment2 Project Overview
===========================================================================

This file contains a summary of what you will find in each of the files 
that make up the CG_Assignment2 application.

CG_Assignment2.cpp
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
the camera. This class is responsible for creating the View matrix used by
the OpenGL calls.

Shape.h
    This struct is used to maintain the parameters of a shape. The vertices,
scale, rotation and translation values are all contained in one structure for
simple manipulation.

ShapeGenerator.cpp/h
    This static class generates shapes for the OpenGL program. The four
shapes in this program include Point, Line, Terrain and Track. The functions
construct the vertices, associate a color, designate the normals, generate
the UV coordinates, assign a scale, rotation and translation, and return
the shape for the application to use.

objloader.cpp/h
    This file contains the loadOBJ function implemented in the COMP371 Lab
sessions. The code was modified slightly to function with my implementation
of vertices.

Vertex.h
    This struct is used to define the necessary values for each vertex and
its attributes. For this application, the position, color, surface normal
and UV coordinates of the vertex are all necessary.

CatmullRom.h
    This file contains the computation functions for the Catmull Rom Spline
generation. Using the subdivision method, based on distance or curvature,
the recursive function determines the vertices of the spline in 3D space.

vertex.shader
    This shader code is used from the COMP371 Lab sessions, with modifications
to accept the color of the vertex and pass it to the fragment shader.

lightVertex.shader
    This shader is a modification of the original shader, and is used for
the terrain of the 3D environment. It has the same position and color from
the original shader, adds the normal and UV values. The offset is instanced
for the terrain pieces, and allows for a tiled terrain. This shader passes
the position, color, normal and UV values to the fragment shader.

trackVertex.shader
    This shader is a modification of the original shader, and is used for
the track of the 3D environment. It has the same position and color from
the original shader, adds the normal and UV values. The model matrix is
instanced for the track pieces, and allows for a reoriented shapes.
This shader passes the position, color, normal and UV values to the 
fragment shader.

fragment.shader
    This shader code is used from the COMP371 Lab sessions, with modifications
to accept the color from the vertex shader, and use that to define the color
of the vertex.

lightFragment.shader
    This shader code is a modification of the original fragment shader, but
adds all the lighting computation for ambient, diffuse, and specular light.
The diffuse light acts as the flashlight on the Camera, and contains specular
light. The spot light is a light shining at the origin. The sun light comes
from one corner of the 3D environment. Ambient light is set low to show the
use of attenuation in light. The light attenuation is calculated using distance.

////////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named CG_Assignment1.pch and a precompiled types file named StdAfx.obj.

grass.jpg, brick.jpg
    These are the image files used for the textures in the application.
    Both are taken from the Labs of COMP371.

////////////////////////////////////////////////////////////////////////////////
Application Controls:

This section will outline the controls of the application to allow the user
to navigate the functionality.

KEY [SPACE] | ACTION [PRESS]
    During spline generation, this accepts the spline and generates the 3D scene.
    Toggles the animation between static and dynamic, when in the 3D environment.

KEY [ENTER] | ACTION [PRESS]
    During spline generation, this uses the current points on the screen and
    generates the spline using Catmull Rom.
    when in the 3D scene, this toggles the lights on and off.

KEY [BACKSPACE] | ACTION [PRESS]
    During spline generation, this will clear the points and spline, and ask
    for a new number of points.
    When in the 3D scene, this will restart the camera at the beginning of
    the spline.

KEY [C] | ACTION [PRESS]
    Toggles between using curvature or distance for the subdivision algorithm.

KEY [P] | ACTION [PRESS]
    Renders the spline scene in GL_POINTS render mode.

KEY [L] | ACTION [PRESS]
    Renders the spline scene in GL_LINE_STRIP render mode.

KEY [T] | ACTION [PRESS]
    Renders the scene with textures applied to objects.

////////////////////////////////////////////////////////////////////////////////
