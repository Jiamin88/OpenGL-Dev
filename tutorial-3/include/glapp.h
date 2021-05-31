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
	};

	struct GLViewport
	{
		GLint x , y;
		GLsizei width , height;
	};

	struct GLObject
	{
		GLfloat angle_speed{ 0.0f };
		GLfloat angle_disp{ 0.0f };
		glm::vec2 scaling{ 0.0f, 0.0f };
		glm::vec2 position{ 0.0f, 0.0f };
		glm::mat3 mdl_to_ndc_xform ;
		GLuint mdl_ref ;
		GLuint shd_ref ;
		void init () ;
		void draw () const ;
		void update ( GLdouble delta_time ) ;
	};

	// container for models and helper function(s) ...
	static std::vector <GLModel> models;
	static GLApp::GLModel box_model ();
	static GLApp::GLModel mystery_model ();
	static void init_models_cont ();

	// container for objects ...
	static std::list <GLApp::GLObject> objects ; // singleton 

	// container for shader programs and helper function(s) ...
	static std::vector<GLSLShader> shdrpgms;
	using VPSS = std::vector <std::pair<std::string , std::string>>;
	static void init_shdrpgms_cont ( GLApp::VPSS const& vpss );


};

#endif /* GLAPP_H */
