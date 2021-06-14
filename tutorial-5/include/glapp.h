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

	struct Camera2D
	{
		GLApp::GLObject* pgo{ nullptr }; // pointer to game object that embeds camera
		glm::vec2 right{ 0 } , up{ 0 };

		GLint height{ 1000 };
		GLint width{ 0 };
		GLfloat ar{ 0 };

		glm::mat3 view_xform{ 0 };
		glm::mat3 camwin_to_ndc_xform{ 0 };
		glm::mat3 world_to_ndc_xform{ 0 };

		// window change parameters ...
		GLint min_height{ 500 } , max_height{ 2000 };

		// height is increasing if 1 and decreasing if -1
		GLint height_chg_dir{ 1 };

		// increments by which window height is changed per Z key press
		GLint height_chg_val{ 5 };

		// camera's speed when button U is pressed
		GLfloat linear_speed{ 2.f };

		// keyboard button press flags
		GLboolean camtype_flag{ GL_FALSE }; // button V
		GLboolean zoom_flag{ GL_FALSE }; // button Z
		GLboolean left_turn_flag{ GL_FALSE }; // button H
		GLboolean right_turn_flag{ GL_FALSE }; // button K
		GLboolean move_flag{ GL_FALSE }; // button U


		void init ( GLFWwindow* pWindow , GLApp::GLObject* ptr );
		void update ( GLFWwindow* pWindow , GLdouble delta_time );
	};


	// define object of type Camera2D ...
	static Camera2D camera2d;

	static std::map<std::string , GLSLShader> shdrpgms; // singleton
	static std::map<std::string , GLModel> models; // singleton
	static std::map<std::string , GLObject> objects; // singleton

	// function to insert shader program into container GLApp::shdrpgms ...
	static void insert_shdrpgm ( std::string shdr_pgm_name ,
								 std::string vtx_shdr ,
								 std::string frg_shdr );

	// function to parse scene file ...
	static void init_scene ( std::string );

};
#endif /* GLAPP_H */
