/* !
@file    glapp.cpp
@author  Jia Min / j.jiamin@digipen.edu
@date    16/07/2021

This file contains the declaration of namespace Helper that encapsulates the
functionality required to create an OpenGL context using GLFW; use GLEW
to load OpenGL extensions; initialize OpenGL state; and finally initialize
the OpenGL application by calling initalization functions associated with
objects participating in the application.

*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLHELPER_H
#define GLHELPER_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <string>

/*  _________________________________________________________________________ */
struct GLHelper
  /*! GLHelper structure to encapsulate initialization stuff ...
  */
{
  static bool init(GLint w, GLint h, std::string t);
  static void cleanup();
  
  // callbacks ...
  static void error_cb(int error, char const* description);
  static void fbsize_cb(GLFWwindow *ptr_win, int width, int height);
  // I/O callbacks ...
  static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
  static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
  static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
  static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);

  static double update_time(double fpsCalcInt = 1.0);


  static GLboolean mouseLeft;

  static GLboolean keystateA;
  static GLboolean keystateH;
  static GLboolean keystateK;
  static GLboolean keystateM;
  static GLboolean keystateR;
  static GLboolean keystateT;
  static GLboolean keystateU;
  static GLboolean keystateV;
  static GLboolean keystateW;
  static GLboolean keystateZ;


  static GLint width, height;
  static GLdouble fps;
  static std::string title;
  static GLFWwindow *ptr_window;
  static void print_specs ();
};



#endif /* GLHELPER_H */
