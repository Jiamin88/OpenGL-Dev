/* !
@file    glapp.h
@author  pghali@digipen.edu
@date    10/11/2016

This file contains the declaration of namespace GLApp that encapsulates the
functionality required to implement an OpenGL application including
compiling, linking, and validating shader programs
setting up geometry and index buffers,
configuring VAO to present the buffered geometry and index data to
vertex shaders,
configuring textures (in later labs),
configuring cameras (in later labs),
and transformations (in later labs).
*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLAPP_H
#define GLAPP_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <glhelper.h>
#include <glslshader.h>

#include <iostream>



struct GLApp
{
	static void init ();
	static void update ( double delta_time );
	static void draw ();
	static void cleanup ();

	struct GLModel
	{
		GLenum		primitive_type{ 0 };		// which OpenGL primitive to be rendered?
		GLuint		primitive_cnt{ 0 };			// added for tutorial 2
		GLuint		vaoid{ 0 };					// same as tutorial 1
		GLuint		draw_cnt{ 0 };				// added for tutorial 2
		GLSLShader	shdr_pgm;					// same as tutorial 1
		void		setup_shdrpgm ( std::string vtx_shdr , std::string frg_shdr );
		void		draw ();
	};

	struct GLViewport
	{
		GLint x , y;
		GLsizei width , height;
	};

	static std::vector<GLViewport> vps;
	static std::vector<GLModel> models;
	static GLApp::GLModel points_model ( int slices , int stacks , std::string vtx_shdr , std::string frg_shdr );
	static GLApp::GLModel lines_model ( int slices , int stacks ,
										std::string vtx_shdr , std::string frg_shdr );
	static GLApp::GLModel trifans_model ( int slices , std::string vtx_shdr ,
										  std::string frg_shdr );

	static GLApp::GLModel tristrip_model ( int slices , int stacks ,
										   std::string vtx_shdr ,
										   std::string frg_shdr );
};

#endif /* GLAPP_H */
