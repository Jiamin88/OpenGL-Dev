/*!
@file    glapp.cpp
@author  pghali@digipen.edu
@date    10/11/2016

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and to extract function pointers
to OpenGL implementations.

*//*__________________________________________________________________________*/

/* Includes
----------------------------------------------------------------------------- */

#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>

#include <glapp.h>
#include <glhelper.h>
#include <array>
#include <vector>
#include <random>
#include <chrono>


/* Debugging tool
----------------------------------------------------------------------------- */

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x , __FILE__ , __LINE__))

void GLClearError ()
{
	while( glGetError () != GL_NO_ERROR );
}

bool GLLogCall ( const char* function , const char* file , int line )
{
	while( GLenum error = glGetError () )
	{
		std::cout
			<< "[OpenGL Error] ( " << error << " )" << std::endl
			<< " Function : " << function << std::endl
			<< " File : " << file << std::endl
			<< " Line : " << line << std::endl;

		return false;
	}
	return true;
}

/* Objects with file scope
----------------------------------------------------------------------------- */

std::map<std::string , GLSLShader> GLApp::shdrpgms;
std::map<std::string , GLApp::GLModel> GLApp::models;
std::map<std::string , GLApp::GLObject> GLApp::objects;
GLApp::Camera2D GLApp::camera2d;
GLboolean keystateVlast = false;
GLboolean heightchange = false;

void GLApp::init ()
{

	glClearColor ( 1.f , 1.f , 1.f , 1.f );

	GLint w{ GLHelper::width } , h{ GLHelper::height };
	glViewport ( 0 , 0 , w , h );

	GLHelper::print_specs ();

	GLApp::init_scene ( "../scenes/tutorial-4.scn" );

	GLApp::camera2d.init ( GLHelper::ptr_window ,
						   &GLApp::objects.at ( "Camera" ) );


}

void GLApp::update ( double delta_time )
{
	GLApp::camera2d.update ( GLHelper::ptr_window , delta_time );

	for( std::map <std::string , GLObject> ::iterator obj = objects.begin (); obj != objects.end (); ++obj )
	{
		obj->second.update ( delta_time );
	}

	keystateVlast = GLHelper::keystateV;
}

void GLApp::draw ()
{
	// write window title with stuff similar to sample ...
	// how? collect everything you want written to title bar in a
	// std::string object named stitle

	std::stringstream ss;
	ss << std::fixed;
	ss.precision ( 2 );
	ss << GLHelper::title << " | "
		<< "FPS: " << GLHelper::fps << " | "
		<< "Camera position: (" << camera2d.pgo->position.x << "," << camera2d.pgo->position.y << ") | "
		<< "Camera orientation: " << ( int ) ( camera2d.pgo->orientation.x * ( 180.0f / 3.14f ) ) << " degrees | "
		<< "Window height: " << camera2d.height;
	glfwSetWindowTitle ( GLHelper::ptr_window , ss.str ().c_str () );

	// clear back buffer as before ...
	glClear ( GL_COLOR_BUFFER_BIT );

	for( std::map <std::string , GLObject> ::iterator obj = objects.begin (); obj != objects.end (); ++obj )
	{
		if( obj->first != "Camera" )
		{
			obj->second.draw ();
		}
	}

	objects[ "Camera" ].draw ();

}

void GLApp::cleanup ()
{}

void GLApp::insert_shdrpgm ( std::string shdr_pgm_name , std::string vtx_shdr , std::string frg_shdr )
{
	std::vector<std::pair<GLenum , std::string>> shdr_files
	{
		std::make_pair ( GL_VERTEX_SHADER, vtx_shdr ),
		std::make_pair ( GL_FRAGMENT_SHADER, frg_shdr )
	};

	GLSLShader shdr_pgm;
	shdr_pgm.CompileLinkValidate ( shdr_files );
	if( GL_FALSE == shdr_pgm.IsLinked () )
	{
		std::cout << "Unable to compile/link/validate shader programs\n";
		std::cout << shdr_pgm.GetLog () << "\n";
		std::exit ( EXIT_FAILURE );
	}

	// add compiled, linked, and validated shader program to
	// std::map container GLApp::shdrpgms
	GLApp::shdrpgms[ shdr_pgm_name ] = shdr_pgm;
}


void GLApp::init_scene ( std::string scene_filename )
{
	std::ifstream ifs ( scene_filename , std::ios::in );
	if( !ifs )
	{
		std::cout <<
			"Error : Unable to open scene file : " <<
			scene_filename << std::endl;
		exit ( EXIT_FAILURE );
	}
	ifs.seekg ( 0 , std::ios::beg );
	std::string line ;
	getline ( ifs , line );
	std::istringstream line_sstm{ line };
	int obj_cnt;
	line_sstm >> obj_cnt;
	while( obj_cnt-- )
	{
		GLObject Object;

		// read each object's parameters
		getline ( ifs , line );
		std::istringstream line_model_name{ line };
		std::string model_name;
		line_model_name >> model_name;
		//std::cout << "Name :" << model_name << std::endl;

		getline ( ifs , line );
		std::istringstream line_model_obj_num{ line };
		std::string model_object;
		line_model_obj_num >> model_object;
		//std::cout << "Object Number :" << model_object << std::endl;

		getline ( ifs , line );
		std::istringstream line_model_obj_shader{ line };
		std::string object_shader_program , object_shader_vertex , object_shader_fragment ;
		line_model_obj_shader >> object_shader_program ;
		line_model_obj_shader >> object_shader_vertex ;
		line_model_obj_shader >> object_shader_fragment ;
		//std::cout << "Object shader program :" << object_shader_program << std::endl;
		//std::cout << "Object shader vertex :" << object_shader_vertex << std::endl;
		//std::cout << "Object shader fragment :" << object_shader_fragment << std::endl;

		getline ( ifs , line );
		std::istringstream line_model_obj_color{ line };
		line_model_obj_color >> Object.color.r >> Object.color.g >> Object.color.b;
		//std::cout << "Object color_r: " << Object.color.r << std::endl;
		//std::cout << "Object color_g: " << Object.color.g << std::endl;
		//std::cout << "Object color_b: " << Object.color.b << std::endl;

		getline ( ifs , line );
		std::istringstream line_model_obj_scale{ line };
		line_model_obj_scale >> Object.scaling.x >> Object.scaling.y;
		//std::cout << "Object scale_horizontal: " << Object.scaling.x << std::endl;
		//std::cout << "Object scale_vertices: " << Object.scaling.y << std::endl;

		getline ( ifs , line );
		std::istringstream line_model_obj_orientation{ line };
		line_model_obj_orientation >> Object.orientation.x >> Object.orientation.y;
		Object.orientation.x *= 3.1425f / 180.0f;
		Object.orientation.y *= 3.1425f / 180.0f;
		//std::cout << "Object orientation_x: " << Object.orientation.x << std::endl;
		//std::cout << "Object orientation_y: " << Object.orientation.y << std::endl;

		getline ( ifs , line );
		std::istringstream line_model_obj_pos{ line };
		line_model_obj_pos >> Object.position.x >> Object.position.y;
		//std::cout << "Object position_x: " << Object.position.x << std::endl;
		//std::cout << "Object position_y: " << Object.position.y << std::endl;


		if( models.find ( model_name ) != models.end () )
		{
			Object.mdl_ref = models.find ( model_name );
		}
		else
		{
			GLModel Model ;

			/* Read from meshes files.*/
			std::ifstream ifs{ "../meshes/" + model_name + ".msh" , std::ios::in };
			if( !ifs )
			{
				std::cout << "ERROR: Unable to open mesh file: "
					<< model_name << "\n";
				exit ( EXIT_FAILURE );
			}
			ifs.seekg ( 0 , std::ios::beg );
			std::string line_mesh ;
			getline ( ifs , line_mesh );
			std::istringstream line_sstm{ line_mesh };
			char obj_prefix;
			std::string mesh_name;
			line_sstm >> obj_prefix >> mesh_name;
			//std::cout << "Obj Prefix : " << obj_prefix << " Mesh name: " << mesh_name << std::endl;

			std::vector < float > pos_vtx;
			std::vector < GLushort > gl_tri_primitives;

			GLuint vbo , vao , ebo ;

			while( getline ( ifs , line_mesh ) )
			{
				std::istringstream line_sstm{ line_mesh };
				line_sstm >> obj_prefix;
				float float_data ;
				GLushort glushort_data;

				if( obj_prefix == 'v' )
				{
					while( line_sstm >> float_data )
					{
						pos_vtx.push_back ( float_data );
						//std::cout << "vertex pos : " << float_data << std::endl;
					}
				}
				if( obj_prefix == 't' )
				{
					while( line_sstm >> glushort_data )
					{
						gl_tri_primitives.push_back ( glushort_data );
						//std::cout << "GL_TRIANGLES primitives : " << glushort_data << std::endl;
					}
					Model.primitive_type = GL_TRIANGLES ;
				}
				if( obj_prefix == 'f' )
				{
					while( line_sstm >> glushort_data )
					{
						gl_tri_primitives.push_back ( glushort_data );
						//std::cout << "GL_TRIANGLES_FAN primitives : " << glushort_data << std::endl;
					}
					Model.primitive_type = GL_TRIANGLE_FAN ;
				}

			}

			glCreateBuffers ( 1 , &vbo ) ;
			glNamedBufferStorage ( vbo , sizeof ( float ) * pos_vtx.size () , pos_vtx.data () , GL_DYNAMIC_STORAGE_BIT ) ;

			glCreateVertexArrays ( 1 , &vao );
			GLCall ( glEnableVertexArrayAttrib ( vao , 0 ) );
			GLCall ( glVertexArrayVertexBuffer ( vao , 0 , vbo , 0 , 2 * sizeof ( float ) ) );
			GLCall ( glVertexArrayAttribFormat ( vao , 0 , 2 , GL_FLOAT , GL_FALSE , 0 ) );
			GLCall ( glVertexArrayAttribBinding ( vao , 0 , 0 ) );

			GLCall ( glCreateBuffers ( 1 , &ebo ) );
			GLCall ( glNamedBufferStorage ( ebo , sizeof ( GLushort ) * gl_tri_primitives.size () , gl_tri_primitives.data () , GL_DYNAMIC_STORAGE_BIT ) );
			GLCall ( glVertexArrayElementBuffer ( vao , ebo ) );
			GLCall ( glBindVertexArray ( 0 ) );

			Model.vaoid = vao;
			Model.primitive_cnt = gl_tri_primitives.size ();
			Model.draw_cnt = gl_tri_primitives.size ();

			models[ model_name ] = Model ;
			Object.mdl_ref = models.find ( model_name );
		}

		if( shdrpgms.find ( object_shader_program ) != shdrpgms.end () )
		{
			Object.shd_ref = shdrpgms.find ( object_shader_program );
		}
		else
		{
			insert_shdrpgm ( object_shader_program , object_shader_vertex , object_shader_fragment );
			Object.shd_ref = shdrpgms.find ( object_shader_program );
		}
		objects[ model_object ] = Object;
	}
}

void GLApp::GLObject::init ()
{}

void GLApp::GLObject::draw () const
{
	shd_ref->second.Use ();
	GLCall ( glBindVertexArray ( mdl_ref->second.vaoid ) );

	shd_ref->second.SetUniform ( "uColor" , color );
	shd_ref->second.SetUniform ( "uModel_to_NDC" , mdl_to_ndc_xform );
	GLCall ( glDrawElements ( mdl_ref->second.primitive_type , mdl_ref->second.draw_cnt , GL_UNSIGNED_SHORT , NULL ) );

	glBindVertexArray ( 0 );
	shd_ref->second.UnUse ();
}

void GLApp::GLObject::update ( GLdouble delta_time )
{
	glm::mat3 Scale
	{
		scaling.x	, 0			, 0 ,
		0			, scaling.y	, 0 ,
		0			, 0			, 1
	};

	orientation.x += orientation.y * ( float ) delta_time;

	glm::mat3 Rotate
	{
		 cos ( orientation.x )	, sin ( orientation.x ) ,	0 ,
		-sin ( orientation.x )	, cos ( orientation.x ) ,	0 ,
		 0						, 0						, 	1
	};

	glm::mat3 Translate
	{
		1			,	0			,	0 ,
		0			,	1			,	0 ,
		position.x	,	position.y	,	1
	};

	glm::mat3 Extend
	{
		1.0f / 20000.0f	, 0					,	0	,
		0				, 1.0f / 20000.0f	,	0	,
		0				, 0					,	1

	};

	mdl_to_ndc_xform = camera2d.world_to_ndc_xform * Translate * Rotate * Scale ;
}

void GLApp::Camera2D::init ( GLFWwindow* pWindow , GLApp::GLObject* ptr )
{
	pgo = ptr;

	GLsizei fb_width , fb_height;
	glfwGetFramebufferSize ( pWindow , &fb_width , &fb_height );
	ar = static_cast< GLfloat >( fb_width ) / fb_height;

	width = ( int ) ar * height;

	camwin_to_ndc_xform =
	{
		2.0f / ( float ) width	, 0							, 0 ,
		0						, 2.0f / ( float ) height	, 0 ,
		0						, 0							, 1
	};


	up = { -sin ( pgo->orientation.x ), cos ( pgo->orientation.x ) };

	right = { cos ( pgo->orientation.x ) , sin ( pgo->orientation.x ) };

	view_xform =
	{
		1					, 0					, 0 ,
		0					, 1					, 0 ,
		-ptr->position.x	, -ptr->position.y	, 1
	};

	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;

}

void GLApp::Camera2D::update ( GLFWwindow* pWindow , GLdouble delta_time )
{
	GLsizei fb_width , fb_height;
	glfwGetFramebufferSize ( pWindow , &fb_width , &fb_height );
	ar = static_cast< GLfloat >( fb_width ) / fb_height;

	width = ( int ) ar * height;

	camwin_to_ndc_xform =
	{
		2.0f / ( float ) width	, 0							, 0 ,
		0						, 2.0f / ( float ) height	, 0 ,
		0						, 0							, 1
	};

	up = { -sin ( pgo->orientation.x ), cos ( pgo->orientation.x ) };

	right = { cos ( pgo->orientation.x ) , sin ( pgo->orientation.x ) };
	if( GLHelper::keystateV && GLHelper::keystateV != keystateVlast )
	{
		if( camtype_flag == GL_FALSE )
		{
			camtype_flag = GL_TRUE;
		}
		else
		{
			camtype_flag = GL_FALSE;
		}
	};
	if( !camtype_flag )
	{
		view_xform =
		{
			1					, 0					, 0 ,
			0					, 1					, 0 ,
			-pgo->position.x	, -pgo->position.y	, 1
		};
	}
	else
	{
		float right_dot_position
		{ ( right.x * pgo->position.x ) + ( right.y * pgo->position.y ) };

		float up_dot_position
		{ ( up.x * pgo->position.x ) + ( up.y * pgo->position.y ) };

		view_xform =
		{
			right.x					, up.x					, 0 ,
			right.y					, up.y					, 0 ,
			-right_dot_position		, -up_dot_position		, 1
		};
	}



	if( GLHelper::keystateZ )
	{
		if( height <= min_height )
			height_chg_dir = 1;
		else if( height >= max_height )
			height_chg_dir = -1;

		height += height_chg_val * height_chg_dir;
	}

	if( GLHelper::keystateH )
	{
		pgo->orientation.x += 1.0f * ( float ) delta_time;
	}

	if( GLHelper::keystateK )
	{
		pgo->orientation.x -= 1.0f * ( float ) delta_time;
	}


	if( GLHelper::keystateU )
	{
		pgo->position += up * linear_speed ;
	}

	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
}
