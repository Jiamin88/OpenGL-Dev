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

std::vector<GLApp::GLModel> GLApp::models;
std::vector<GLApp::GLViewport> GLApp::vps;

void GLApp::init ()
{
	// Part 1: clear colorbuffer with white color ...
	glClearColor ( 1.f , 1.f , 1.f , 1.f );
	// Part 2: split color buffer into four viewports ...
	GLint w = GLHelper::width , h = GLHelper::height;
	// push top-left
	vps.push_back ( { 0, h / 2, w / 2, h / 2 } );
	// push top-right
	vps.push_back ( { w / 2, h / 2, w / 2, h / 2 } );
	// push bot-left
	vps.push_back ( { 0, 0, w / 2, h / 2 } );
	// push bot-right
	vps.push_back ( { w / 2, 0, w / 2, h / 2 } );

	// Part 3: create different geometries and insert them into
	// repository container GLApp::models ...


	GLApp::models.push_back ( GLApp::points_model ( 20 , 20 ,
													"../shaders/tutorial-2.vert" ,
													"../shaders/tutorial-2.frag" ) );

	GLApp::models.push_back ( GLApp::lines_model ( 40 , 40 ,
												   "../shaders/tutorial-2.vert" ,
												   "../shaders/tutorial-2.frag" ) );

	GLApp::models.push_back ( GLApp::trifans_model ( 50 ,
													 "../shaders/tutorial-2.vert" ,
													 "../shaders/tutorial-2.frag" ) );

	GLApp::models.push_back ( GLApp::tristrip_model ( 4 , 3 ,
													  "../shaders/tutorial-2.vert" ,
													  "../shaders/tutorial-2.frag" ) );

	// Part 4: print GPU specs ...
	GLHelper::print_specs ();
}

void GLApp::update ( double delta_time )
{}

void GLApp::draw ()
{
	// write window title with stuff similar to sample ...
	// how? collect everything you want written to title bar in a
	// std::string object named stitle
	std::string stitle{ GLHelper::title };
	glfwSetWindowTitle ( GLHelper::ptr_window , stitle.c_str () );
	// clear back buffer as before ...
	glClear ( GL_COLOR_BUFFER_BIT );
	// render points in top-left viewport
	glViewport ( vps[ 0 ].x , vps[ 0 ].y , vps[ 0 ].width , vps[ 0 ].height );
	GLApp::models[ 0 ].draw ();
	// render line segments in top-right viewport
	glViewport ( vps[ 1 ].x , vps[ 1 ].y , vps[ 1 ].width , vps[ 1 ].height );
	GLApp::models[ 1 ].draw ();

	glViewport ( vps[ 2 ].x , vps[ 2 ].y , vps[ 2 ].width , vps[ 2 ].height );
	GLApp::models[ 2 ].draw ();


	glViewport ( vps[ 3 ].x , vps[ 3 ].y , vps[ 3 ].width , vps[ 3 ].height );
	GLApp::models[ 3 ].draw ();
}

void GLApp::cleanup ()
{
	// empty for now
}

GLApp::GLModel GLApp::lines_model ( int slices , int stacks , std::string vtx_shdr , std::string frg_shdr )
{

	int const count{ ( slices + stacks + 1 ) * 2 };
	std::vector<glm::vec2> pos_vtx ( count );
	float const u{ 2.f / static_cast< float >( slices ) };
	for( int col{ 0 } , index{ 0 }; col <= slices; ++col )
	{
		float x{ u * static_cast< float >( col ) - 1.f };
		pos_vtx[ index++ ] = glm::vec2 ( x , -1.f );
		pos_vtx[ index++ ] = glm::vec2 ( x , 1.f );
	}

	float const v{ 2.f / static_cast< float >( stacks ) };
	for( int row{ 0 } , index{ slices * 2 }; row <= stacks; ++row )
	{
		float y{ v * static_cast< float >( row ) - 1.f };
		pos_vtx[ index++ ] = glm::vec2 ( 1.0f , y );
		pos_vtx[ index++ ] = glm::vec2 ( -1.0f , y );
	}

	GLuint vbo_hdl;
	glCreateBuffers ( 1 , &vbo_hdl );
	glNamedBufferStorage ( vbo_hdl , sizeof ( glm::vec2 ) * pos_vtx.size () ,
						   pos_vtx.data () , GL_DYNAMIC_STORAGE_BIT );

	GLuint vaoid;
	glCreateVertexArrays ( 1 , &vaoid );
	glEnableVertexArrayAttrib ( vaoid , 0 );
	glVertexArrayVertexBuffer ( vaoid , 0 , vbo_hdl , 0 , sizeof ( glm::vec2 ) );
	glVertexArrayAttribFormat ( vaoid , 0 , 2 , GL_FLOAT , GL_FALSE , 0 );
	glVertexArrayAttribBinding ( vaoid , 0 , 0 );
	glBindVertexArray ( 0 );

	GLApp::GLModel mdl;
	mdl.vaoid = vaoid; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_LINES;
	mdl.setup_shdrpgm ( vtx_shdr , frg_shdr );
	mdl.draw_cnt = 2 * ( slices + 1 ) + 2 * ( stacks + 1 ); // number of vertices
	mdl.primitive_cnt = mdl.draw_cnt / 2; // number of primitives (not used)
	return mdl;

}

GLApp::GLModel GLApp::trifans_model ( int slices , std::string vtx_shdr , std::string frg_shdr )
{

	std::vector<glm::vec2> pos_vtx;
	float each_slice = ( 2.0f * 3.14f ) / slices;
	pos_vtx.push_back ( { 0.0f, 0.0f } );
	for( int i = 0 ; i < slices ; i++ )
	{
		pos_vtx.push_back ( { cos ( each_slice * i ), sin ( each_slice * i ) } );
	}


	std::vector<glm::vec3> clr_vtx;
	for( int i = 0; i < ( int ) pos_vtx.size (); ++i )
	{
		clr_vtx.push_back ( { ( float ) rand () / RAND_MAX, ( float ) rand () / RAND_MAX, ( float ) rand () / RAND_MAX } );
	}

	// define vbo handle
	GLuint vbo_hdl;
	glCreateBuffers ( 1 , &vbo_hdl );

	glNamedBufferStorage ( vbo_hdl ,
						   sizeof ( glm::vec2 ) * pos_vtx.size () + sizeof ( glm::vec3 ) * clr_vtx.size () ,
						   nullptr , GL_DYNAMIC_STORAGE_BIT );

	glNamedBufferSubData ( vbo_hdl , 0 ,
						   sizeof ( glm::vec2 ) * pos_vtx.size () , pos_vtx.data () );

	glNamedBufferSubData ( vbo_hdl , sizeof ( glm::vec2 ) * pos_vtx.size () ,
						   sizeof ( glm::vec3 ) * clr_vtx.size () , clr_vtx.data () );

	// define vao handle
	GLuint vaoid;
	glCreateVertexArrays ( 1 , &vaoid );

	// for position
	glEnableVertexArrayAttrib ( vaoid , 0 );
	glVertexArrayVertexBuffer ( vaoid , 4 , vbo_hdl ,
								0 ,
								sizeof ( glm::vec2 ) );
	glVertexArrayAttribFormat ( vaoid , 0 , 2 , GL_FLOAT , GL_FALSE , 0 );
	glVertexArrayAttribBinding ( vaoid , 0 , 4 );

	// for color
	glEnableVertexArrayAttrib ( vaoid , 1 );
	glVertexArrayVertexBuffer ( vaoid , 5 , vbo_hdl ,
								sizeof ( glm::vec2 ) * pos_vtx.size () ,
								sizeof ( glm::vec3 ) );
	glVertexArrayAttribFormat ( vaoid , 1 , 3 , GL_FLOAT , GL_FALSE , 0 );
	glVertexArrayAttribBinding ( vaoid , 1 , 5 );

	std::vector<GLushort> idx;
	for( int i = 0; i <= slices; ++i )
	{
		idx.push_back ( i );
	}
	idx.push_back ( 1 );

	GLuint ebo_hdl;
	glCreateBuffers ( 1 , &ebo_hdl );
	glNamedBufferStorage ( ebo_hdl , sizeof ( GLushort ) * idx.size () , idx.data () , GL_DYNAMIC_STORAGE_BIT );
	glVertexArrayElementBuffer ( vaoid , ebo_hdl );

	glBindVertexArray ( 0 );

	GLApp::GLModel mdl;
	mdl.vaoid = vaoid; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_TRIANGLE_FAN;
	mdl.setup_shdrpgm ( vtx_shdr , frg_shdr );
	mdl.draw_cnt = idx.size (); // number of vertices
	mdl.primitive_cnt = mdl.draw_cnt / 2; // number of primitives (not used)
	return mdl;

}

GLApp::GLModel GLApp::tristrip_model ( int slices , int stacks , std::string vtx_shdr , std::string frg_shdr )
{
	// vertices
	std::vector<glm::vec2> pos_vtx;
	float stack_interval = 2.0f / stacks , slice_interval = 2.0f / slices;
	for( int i = 0; i <= stacks; ++i )
	{
		for( int j = 0; j <= slices; ++j )
		{
			pos_vtx.push_back ( { -1.0f + i * stack_interval, -1.0f + j * slice_interval } );
		}
	}

	// indices
	std::vector<GLushort> triangle_strip_combine;
	for( int row = 0; row < stacks; ++row )
	{
		if( row != 0 )
		{
			triangle_strip_combine.push_back ( ( row + 1 ) * ( slices + 1 ) );
		}
		for( int col = 0; col <= slices; ++col )
		{
			triangle_strip_combine.push_back ( ( slices + 1 ) * ( row + 1 ) + col );
			triangle_strip_combine.push_back ( ( slices + 1 ) * row + col );
		}
		if( row != stacks - 1 )
		{
			triangle_strip_combine.push_back ( ( row ) * ( slices + 1 ) + slices );
		}
	}

	// color
	std::vector<glm::vec3> clr_vtx;
	for( int i = 0; i < ( int ) pos_vtx.size (); ++i )
	{
		clr_vtx.push_back ( { ( float ) rand () / RAND_MAX, ( float ) rand () / RAND_MAX, ( float ) rand () / RAND_MAX } );
	}

	// define vbo handle
	GLuint vbo_hdl;
	glCreateBuffers ( 1 , &vbo_hdl );

	glNamedBufferStorage ( vbo_hdl ,
						   sizeof ( glm::vec2 ) * pos_vtx.size () + sizeof ( glm::vec3 ) * clr_vtx.size () ,
						   nullptr , GL_DYNAMIC_STORAGE_BIT );

	glNamedBufferSubData ( vbo_hdl , 0 ,
						   sizeof ( glm::vec2 ) * pos_vtx.size () , pos_vtx.data () );

	glNamedBufferSubData ( vbo_hdl , sizeof ( glm::vec2 ) * pos_vtx.size () ,
						   sizeof ( glm::vec3 ) * clr_vtx.size () , clr_vtx.data () );

	// define vao handle
	GLuint vaoid;
	glCreateVertexArrays ( 1 , &vaoid );

	// for position
	glEnableVertexArrayAttrib ( vaoid , 0 );
	glVertexArrayVertexBuffer ( vaoid , 4 , vbo_hdl ,
								0 ,
								sizeof ( glm::vec2 ) );
	glVertexArrayAttribFormat ( vaoid , 0 , 2 , GL_FLOAT , GL_FALSE , 0 );
	glVertexArrayAttribBinding ( vaoid , 0 , 4 );

	// for color
	glEnableVertexArrayAttrib ( vaoid , 1 );
	glVertexArrayVertexBuffer ( vaoid , 5 , vbo_hdl ,
								sizeof ( glm::vec2 ) * pos_vtx.size () ,
								sizeof ( glm::vec3 ) );
	glVertexArrayAttribFormat ( vaoid , 1 , 3 , GL_FLOAT , GL_FALSE , 0 );
	glVertexArrayAttribBinding ( vaoid , 1 , 5 );

	GLuint ebo_hdl;
	glCreateBuffers ( 1 , &ebo_hdl );
	glNamedBufferStorage ( ebo_hdl , sizeof ( GLushort ) * triangle_strip_combine.size () , triangle_strip_combine.data () , GL_DYNAMIC_STORAGE_BIT );
	glVertexArrayElementBuffer ( vaoid , ebo_hdl );

	glBindVertexArray ( 0 );

	GLApp::GLModel mdl;
	mdl.vaoid = vaoid; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_TRIANGLE_STRIP;
	mdl.setup_shdrpgm ( vtx_shdr , frg_shdr );
	mdl.draw_cnt = triangle_strip_combine.size (); // number of vertices
	mdl.primitive_cnt = mdl.draw_cnt / 2; // number of primitives (not used)
	return mdl;
}

GLApp::GLModel GLApp::points_model ( int slices , int stacks , std::string vtx_shdr , std::string frg_shdr )
{
	float length = 2.0f / slices;
	float height = 2.0f / stacks;
	std::vector<glm::vec2> pos_vtx;
	for( int j = 0 ; j <= stacks ; j++ )
	{
		for( int i = 0; i <= slices ; i++ )
		{
			pos_vtx.push_back ( { ( length * i ) - 1.0f , ( height * j ) - 1.0f } );
		}
	}

	GLuint vbo_hdl;
	glCreateBuffers ( 1 , &vbo_hdl );
	glNamedBufferStorage ( vbo_hdl , sizeof ( glm::vec2 ) * pos_vtx.size () ,
						   pos_vtx.data () , GL_DYNAMIC_STORAGE_BIT );

	GLuint vaoid;
	glCreateVertexArrays ( 1 , &vaoid );
	glEnableVertexArrayAttrib ( vaoid , 0 );
	glVertexArrayVertexBuffer ( vaoid , 0 , vbo_hdl , 0 , sizeof ( glm::vec2 ) );
	glVertexArrayAttribFormat ( vaoid , 0 , 2 , GL_FLOAT , GL_FALSE , 0 );
	glVertexArrayAttribBinding ( vaoid , 0 , 0 );
	glBindVertexArray ( 0 );

	GLApp::GLModel mdl;
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_POINTS;
	mdl.setup_shdrpgm ( vtx_shdr , frg_shdr );
	mdl.draw_cnt = pos_vtx.size (); // number of vertices
	mdl.primitive_cnt = mdl.draw_cnt; // number of primitives (unused)

	return mdl;

}

void GLApp::GLModel::setup_shdrpgm ( std::string vtx_shdr , std::string frg_shdr )
{
	std::vector<std::pair<GLenum , std::string>> shdr_files;
	shdr_files.push_back ( std::make_pair ( GL_VERTEX_SHADER , vtx_shdr ) );
	shdr_files.push_back ( std::make_pair ( GL_FRAGMENT_SHADER , frg_shdr ) );
	shdr_pgm.CompileLinkValidate ( shdr_files );
	if( GL_FALSE == shdr_pgm.IsLinked () )
	{
		std::cout << "Unable to compile/link/validate shader programs\n";
		std::cout << shdr_pgm.GetLog () << "\n";
		std::exit ( EXIT_FAILURE );
	}
}


void GLApp::GLModel::draw ()
{

	shdr_pgm.Use ();
	glBindVertexArray ( vaoid );
	switch( primitive_type )
	{
		case GL_POINTS:
			glPointSize ( 10.f );
			glVertexAttrib3f ( 1 , 1.f , 0.0f , 0.f ); // red color for points
			glDrawArrays ( primitive_type , 0 , draw_cnt );
			glPointSize ( 5.0f );
			break;
		case GL_LINES:
			glLineWidth ( 3.f );
			glVertexAttrib3f ( 1 , 0.f , 0.f , 1.f ); // blue color for lines
			glDrawArrays ( primitive_type , 0 , draw_cnt );
			glLineWidth ( 1.f );
			break;
		case GL_TRIANGLE_FAN:
			glDrawElements ( primitive_type , draw_cnt , GL_UNSIGNED_SHORT , NULL );
			break;
		case GL_TRIANGLE_STRIP:
			glDrawElements ( primitive_type , draw_cnt , GL_UNSIGNED_SHORT , NULL );
			break;
	}
	glBindVertexArray ( 0 );

	shdr_pgm.UnUse ();

}