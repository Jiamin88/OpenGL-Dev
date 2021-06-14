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


#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>

#include <glapp.h>
#include <glhelper.h>
#include <array>
#include <vector>
#include <random>
#include <chrono>

//#include "Renderer.h"

/*                                                   objects with file scope
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

std::list < GLApp::GLObject> GLApp::objects ;
std::vector < GLSLShader > GLApp::shdrpgms;
std::vector < GLApp::GLModel > GLApp::models;
int keystatePCounter{ 0 };
GLboolean keystatePlast{ false };
GLboolean mouseLeftlast{ false };
bool createobjects{ true };
int objectCounter{ 0 };
int objectMax{ 32768 };
int boxcount{ 0 };
int mysteryboxcount{ 0 };

void GLApp::init ()
{
	init_models_cont () ;

	// Print GPU specs ...
	GLHelper::print_specs ();

	glClearColor ( 1.f , 1.f , 1.f , 1.f );

	GLint w{ GLHelper::width } , h{ GLHelper::height };
	glViewport ( 0 , 0 , w , h );

	GLApp::VPSS shdr_files_name{ std::make_pair < std::string , std::string > ( "../shaders/tutorial-3.vert", "../shaders/tutorial-3.frag" ) };
	GLApp::init_shdrpgms_cont ( shdr_files_name );
}

void GLApp::update ( double delta_time )
{
	// Part 1: Update polygon rasterization mode ...
	// Check if key 'P' is pressed
	// If pressed, update polygon rasterization mode
	if( GLHelper::keystateP && !keystatePlast )
	{
		switch( keystatePCounter )
		{
			case 0:
				glPolygonMode ( GL_FRONT_AND_BACK , GL_LINE );
				break ;
			case 1:
				glPolygonMode ( GL_FRONT_AND_BACK , GL_POINT );
				break ;
			case 2:
				glPolygonMode ( GL_FRONT_AND_BACK , GL_FILL );
				break ;
		}

		keystatePCounter++ ;

		if( keystatePCounter == 3 )
		{
			keystatePCounter = 0;
		}
	}


	// Part 2: Spawn or kill objects ...
	// Check if left mouse button is pressed
	// If maximum object limit is not reached, spawn new object(s)
	// Otherwise, kill objects that have been most recently spawned
	std::default_random_engine dre;
	dre.seed ( ( unsigned int ) std::chrono::system_clock::now ().time_since_epoch ().count () );
	std::uniform_real_distribution<float> urdf ( -1.0 , 1.0 );
	double const WorldRange{ 5000.0 };

	if( GLHelper::mouseLeft && GLHelper::mouseLeft != mouseLeftlast )
	{
		// create first object
		if( objects.size () <= 0 )
		{
			GLApp::GLObject go;
			go.position = glm::vec2 ( urdf ( dre ) * WorldRange ,
									  urdf ( dre ) * WorldRange );
			go.angle_disp = GLfloat ( urdf ( dre ) * 3.142f * 2.0f );
			go.angle_speed = GLfloat ( urdf ( dre ) * ( 30.0f / 180.0f ) * 3.142f );
			std::uniform_real_distribution<float> urdf2 ( 50.0f , 400.0f );
			go.scaling = glm::vec2{ urdf2 ( dre ), urdf2 ( dre ) };
			if( urdf ( dre ) < 0.0f )
			{
				go.mdl_ref = 0;
				++boxcount;
			}
			else
			{
				go.mdl_ref = 1;
				++mysteryboxcount;
			}

			objects.push_back ( go );


		}
		else
		{
			int iterations{ 0 };
			// if 1 create more objects at random
			if( objects.size () <= 1 )
			{
				createobjects = true;
			}
			// else remove half
			else if( objects.size () >= 32768 )
			{
				createobjects = false;
			}
			if( createobjects )
			{
				iterations = objects.size ();
				// create new objects at back of object list
				for( int i = 0; i < iterations; ++i )
				{
					GLApp::GLObject go;
					go.position = glm::vec2 ( urdf ( dre ) * WorldRange ,
											  urdf ( dre ) * WorldRange );
					go.angle_disp = GLfloat ( urdf ( dre ) * 3.142f * 2.0f );
					go.angle_speed = GLfloat ( urdf ( dre ) * ( 30.0f / 180.0f ) * 3.142f );
					std::uniform_real_distribution<float> urdf2 ( 50.0f , 400.0f );
					go.scaling = glm::vec2{ urdf2 ( dre ), urdf2 ( dre ) };
					if( urdf ( dre ) < 0.0f )
					{
						go.mdl_ref = 0;
						++boxcount;
					}
					else
					{
						go.mdl_ref = 1;
						++mysteryboxcount;
					}
					objects.push_back ( go );
				}
			}
			else
			{
				iterations = objects.size () / 2;
				// remove objects at front of object list
				for( int i = 0; i < iterations; ++i )
				{
					if( objects.front ().mdl_ref != 1 )
					{
						--boxcount;
					}
					else
					{
						--mysteryboxcount;
					}
					objects.pop_front ();
				}
			}
		}
	}

	// Part 3:
	// for each object in container GLApp::objects
	// Update object's orientation
	// A more elaborate implementation would animate the object's movement
	// A much more elaborate implementation would animate the object's size
	// Using updated attributes, compute world-to-ndc transformation matrix
	for( auto& obj : objects )
	{
		obj.update ( delta_time );
	}

	keystatePlast = GLHelper::keystateP;
	mouseLeftlast = GLHelper::mouseLeft;
}

void GLApp::draw ()
{
	// write window title with stuff similar to sample ...
	// how? collect everything you want written to title bar in a
	// std::string object named stitle

	std::stringstream ss;
	ss << GLHelper::title << " | "
		<< objects.size () << " objects | "
		<< boxcount << " box | "
		<< mysteryboxcount << " mystery stuff | "
		<< "fps: " << GLHelper::fps << " | ";
	glfwSetWindowTitle ( GLHelper::ptr_window , ss.str ().c_str () );

	// clear back buffer as before ...
	glClear ( GL_COLOR_BUFFER_BIT );

	for( auto& obj : objects )
	{
		obj.draw ();
	}

}

void GLApp::cleanup ()
{
	// empty for now
}

GLApp::GLModel GLApp::box_model ()
{
	GLModel BoxModel ;
	std::array<glm::vec2 , 4> pos_vtx
	{
		glm::vec2 ( 0.5f, -0.5f ), glm::vec2 ( 0.5f, 0.5f ),
		glm::vec2 ( -0.5f, 0.5f ), glm::vec2 ( -0.5f, -0.5f )
	};
	std::array<glm::vec3 , 4> clr_vtx
	{
	glm::vec3 ( 1.f, 0.f, 0.f ), glm::vec3 ( 0.f, 1.f, 0.f ),
	glm::vec3 ( 0.f, 0.f, 1.f ), glm::vec3 ( 1.f, 1.f, 1.f )
	};

	GLuint vbo ;
	glCreateBuffers ( 1 , &vbo ) ;
	glNamedBufferStorage ( vbo , sizeof ( glm::vec2 ) * pos_vtx.size () + sizeof ( glm::vec3 ) * clr_vtx.size () , nullptr , GL_DYNAMIC_STORAGE_BIT ) ;
	glNamedBufferSubData ( vbo , 0 , sizeof ( glm::vec2 ) * pos_vtx.size () , pos_vtx.data () ) ;
	glNamedBufferSubData ( vbo , sizeof ( glm::vec2 ) * pos_vtx.size () , sizeof ( glm::vec3 ) * clr_vtx.size () , clr_vtx.data () );

	GLuint vao ;
	glCreateVertexArrays ( 1 , &vao );

	GLCall ( glEnableVertexArrayAttrib ( vao , 0 ) );
	GLCall ( glVertexArrayVertexBuffer ( vao , 0 , vbo , 0 , sizeof ( glm::vec2 ) ) );
	GLCall ( glVertexArrayAttribFormat ( vao , 0 , 2 , GL_FLOAT , GL_FALSE , 0 ) );
	GLCall ( glVertexArrayAttribBinding ( vao , 0 , 0 ) );

	GLCall ( glEnableVertexArrayAttrib ( vao , 1 ) );
	GLCall ( glVertexArrayVertexBuffer ( vao , 1 , vbo , sizeof ( glm::vec2 ) * pos_vtx.size () , sizeof ( glm::vec3 ) ) );
	GLCall ( glVertexArrayAttribFormat ( vao , 1 , 3 , GL_FLOAT , GL_FALSE , 0 ) );
	GLCall ( glVertexArrayAttribBinding ( vao , 1 , 1 ) );

	std::array<GLushort , 6> idx_vtx{ 0, 1, 2, 2, 3, 0 };

	GLuint ebo ;
	GLCall ( glCreateBuffers ( 1 , &ebo ) );
	GLCall ( glNamedBufferStorage ( ebo , sizeof ( GLushort ) * idx_vtx.size () , idx_vtx.data () , GL_DYNAMIC_STORAGE_BIT ) );
	GLCall ( glVertexArrayElementBuffer ( vao , ebo ) );

	GLCall ( glBindVertexArray ( 0 ) );

	BoxModel.vaoid = vao;
	BoxModel.primitive_cnt = idx_vtx.size ();
	BoxModel.primitive_type = GL_TRIANGLES ;
	BoxModel.draw_cnt = idx_vtx.size ();

	return BoxModel;
}

GLApp::GLModel GLApp::mystery_model ()
{
	GLModel BoxModel ;
	std::array<glm::vec2 , 4> pos_vtx{
	glm::vec2 ( -1.0f, 1.0f ), glm::vec2 ( 0.0f, -1.0f ),
	glm::vec2 ( 0.0f, 0.5f ), glm::vec2 ( 1.0f, 1.0f )
	};
	std::array<glm::vec3 , 4> clr_vtx
	{
	glm::vec3 ( 1.f, 0.f, 0.f ), glm::vec3 ( 0.f, 1.f, 0.f ),
	glm::vec3 ( 0.f, 0.f, 1.f ), glm::vec3 ( 1.f, 1.f, 1.f )
	};

	GLuint vbo ;
	glCreateBuffers ( 1 , &vbo );
	glNamedBufferStorage ( vbo , sizeof ( glm::vec2 ) * pos_vtx.size () + sizeof ( glm::vec3 ) * clr_vtx.size () , nullptr , GL_DYNAMIC_STORAGE_BIT );
	glNamedBufferSubData ( vbo , 0 , sizeof ( glm::vec2 ) * pos_vtx.size () , pos_vtx.data () );
	glNamedBufferSubData ( vbo , sizeof ( glm::vec2 ) * pos_vtx.size () , sizeof ( glm::vec3 ) * clr_vtx.size () , clr_vtx.data () );

	GLuint vao ;
	glCreateVertexArrays ( 1 , &vao );

	glEnableVertexArrayAttrib ( vao , 0 );
	glVertexArrayVertexBuffer ( vao , 0 , vbo , 0 , sizeof ( glm::vec2 ) );
	glVertexArrayAttribFormat ( vao , 0 , 2 , GL_FLOAT , GL_FALSE , 0 );
	glVertexArrayAttribBinding ( vao , 0 , 0 );

	glEnableVertexArrayAttrib ( vao , 1 );
	glVertexArrayVertexBuffer ( vao , 1 , vbo , sizeof ( glm::vec2 ) * pos_vtx.size () , sizeof ( glm::vec3 ) );
	glVertexArrayAttribFormat ( vao , 1 , 3 , GL_FLOAT , GL_FALSE , 0 );
	glVertexArrayAttribBinding ( vao , 1 , 1 );



	std::array<GLushort , 6> idx_vtx{ 0, 1, 2, 2, 1, 3 };

	GLuint ebo ;
	glCreateBuffers ( 1 , &ebo );
	glNamedBufferStorage ( ebo , sizeof ( GLushort ) * idx_vtx.size () , idx_vtx.data () , GL_DYNAMIC_STORAGE_BIT );
	glVertexArrayElementBuffer ( vao , ebo );

	glBindVertexArray ( 0 );

	BoxModel.vaoid = vao;
	BoxModel.primitive_cnt = idx_vtx.size ();
	BoxModel.primitive_type = GL_TRIANGLES ;
	BoxModel.draw_cnt = idx_vtx.size ();

	return BoxModel;
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
		shdrpgms.push_back ( shdr_prgm );
	}
}

void GLApp::GLObject::init ()
{}

void GLApp::GLObject::draw () const
{
	shdrpgms[ shd_ref ].Use ();
	glBindVertexArray ( models[ mdl_ref ].vaoid );
	shdrpgms[ shd_ref ].SetUniform ( "uModel_to_NDC" , mdl_to_ndc_xform );
	glDrawElements ( models[ mdl_ref ].primitive_type , models[ mdl_ref ].draw_cnt , GL_UNSIGNED_SHORT , NULL );
	glBindVertexArray ( 0 );
	shdrpgms[ shd_ref ].UnUse ();
}

void GLApp::GLObject::update ( GLdouble delta_time )
{
	glm::mat3 Scale
	{
		scaling.x	, 0			, 0 ,
		0			, scaling.y	, 0 ,
		0			, 0			, 1
	};

	angle_disp += angle_speed * ( float ) delta_time;

	glm::mat3 Rotate
	{
		 cos ( angle_disp ) , sin ( angle_disp ) ,	0 ,
		-sin ( angle_disp ) , cos ( angle_disp ) ,	0 ,
		 0					, 0				     , 	1
	};

	glm::mat3 Translate
	{
		1			,	0			,	0 ,
		0			,	1			,	0 ,
		position.x	,	position.y	,	1
	};

	glm::mat3 Extend
	{
		1.0f / 5000.0f	, 0					,	0	,
		0				, 1.0f / 5000.0f	,	0	,
		0				, 0					,	1
	};

	mdl_to_ndc_xform = Extend * Translate * Rotate * Scale ;
}
