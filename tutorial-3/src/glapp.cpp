/*!
@file    glapp.cpp
@author  pghali@digipen.edu
@date    10/11/2016

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and to extract function pointers
to OpenGL implementations.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glapp.h>
#include <glhelper.h>
#include <array>
#include <vector>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */

std::list < GLApp::GLObject> GLApp::objects ;
std::vector < GLSLShader > GLApp::shdrpgms;
std::vector < GLApp::GLModel > GLApp::models;

void GLApp::init ()
{
	// Print GPU specs ...
	GLHelper::print_specs ();

	glClearColor ( 1.f , 1.f , 1.f , 1.f );

	GLint w{ GLHelper::width } , h{ GLHelper::height };
	glViewport ( w , h , w , h );

	GLApp::VPSS shdr_files_name{ std::make_pair < std::string , std::string > ( "../shaders/tutorial-3.vert", "../shaders/tutorial-3.frag" ) };
	GLApp::init_shdrpgms_cont ( shdr_files_name );

}

void GLApp::update ( double delta_time )
{
	/*if( GLHelper::keystateP )
	{

	}*/

	// Part 1: Update polygon rasterization mode ...
	// Check if key 'P' is pressed
	// If pressed, update polygon rasterization mode
	// Part 2: Spawn or kill objects ...
	// Check if left mouse button is pressed
	// If maximum object limit is not reached, spawn new object(s)
	// Otherwise, kill objects that have been most recently spawned
	// Part 3:
	// for each object in container GLApp::objects
	// Update object's orientation
	// A more elaborate implementation would animate the object's movement
	// A much more elaborate implementation would animate the object's size
	// Using updated attributes, compute world-to-ndc transformation matrix

}

void GLApp::draw ()
{
	// write window title with stuff similar to sample ...
	// how? collect everything you want written to title bar in a
	// std::string object named stitle
	std::string stitle{ GLHelper::title };
	glfwSetWindowTitle ( GLHelper::ptr_window , stitle.c_str () );
	// clear back buffer as before ...
	glClear ( GL_COLOR_BUFFER_BIT );
}

void GLApp::cleanup ()
{
	// empty for now
}

GLApp::GLModel GLApp::box_model ()
{

	return GLApp::GLModel ();
}

GLApp::GLModel GLApp::mystery_model ()
{
	return GLApp::GLModel ();
}

void GLApp::init_models_cont ()
{
	GLApp::models.push_back ( GLApp::box_model () );
	GLApp::models.push_back ( GLApp::mystery_model () );
}

void GLApp::init_shdrpgms_cont ( GLApp::VPSS const& vpss )
{
	for( auto const& x : vpss )
	{
		std::vector<std::pair<GLenum , std::string>>shdr_files ;
		shdr_files.push_back ( { GL_VERTEX_SHADER , x.first } );
		shdr_files.push_back ( { GL_FRAGMENT_SHADER , x.second } );


		GLSLShader shdr_prgm;
		shdr_prgm.CompileLinkValidate ( shdr_files );

		if( GL_FALSE == shdr_prgm.IsLinked () )
		{
			std::cout << "Unable to compile/link/validate shader programs\n";
			std::cout << shdr_prgm.GetLog () << "\n";
			std::exit ( EXIT_FAILURE );
		}

		// insert shader program into container
		GLApp::shdrpgms.push_back ( shdr_prgm );

	}



}

void GLApp::GLObject::init ()
{}

void GLApp::GLObject::draw () const
{

}

void GLApp::GLObject::update ( GLdouble delta_time )
{}
