/* !
@file    glapp.cpp
@author  Jia Min / j.jiamin@digipen.edu
@date    16/06/2021

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
#include <list>

struct GLApp
{

	static void init ();
	static void update ( double delta_time );
	static void draw ();
	static void cleanup ();

	// container for shader programs and helper function(s) ...
	struct GLModel
	{
		GLenum		primitive_type{ 0 };		// which OpenGL primitive to be rendered?
		GLuint		primitive_cnt{ 0 };			// added for tutorial 2
		GLuint		vaoid{ 0 };					// same as tutorial 1
		GLuint		draw_cnt{ 0 };				// added for tutorial 2
		GLSLShader	shdr_pgm;					// which shader program?


		// member functions defined in glapp.cpp
		void setup_vao ();
		void setup_shdrpgm ();
		void update ( double delta_time );
		void draw ();

	};

	static GLModel mdl;
	static GLuint setup_texobj ( std::string pathname );

	struct GLViewport
	{
		GLint x , y;
		GLsizei width , height;
	};

	struct GLObject
	{
		glm::vec2 scaling{ 100.0f, 100.0f };

		// orientation.x is angle_disp and
		// orientation.y is angle_speed
		// both values specified in degrees
		glm::vec2 orientation{ 0 };
		glm::vec2 position{ 0.0f, 0.0f };
		glm::vec3 color{ 0 };
		glm::mat3 mdl_xform{ 0 };
		glm::mat3 mdl_to_ndc_xform{ 0 };

		std::map<std::string , GLApp::GLModel>::iterator mdl_ref;
		std::map<std::string , GLSLShader>::iterator shd_ref;

		void init () ;
		void draw () const ;
		void update ( GLdouble delta_time ) ;
	};

	static std::map<std::string , GLSLShader> shdrpgms; // singleton
	static std::map<std::string , GLModel> models; // singleton
	static std::map<std::string , GLObject> objects; // singleton
};
#endif /* GLAPP_H */
