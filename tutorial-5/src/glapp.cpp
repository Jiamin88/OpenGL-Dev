/*!
@file    glapp.cpp
@author  Jia Min / j.jiamin@digipen.edu
@date    16/06/2021

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
#include <fstream> 


/* Debugging tool
----------------------------------------------------------------------------- */

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x , __FILE__ , __LINE__))

/**
 * @brief 
 *	The glGetError function returns one of the following error codes, or GL_NO_ERROR if no (more) errors are available. Each error code represents a category of user error.
*/
void GLClearError ()
{
	while( glGetError () != GL_NO_ERROR );
}
/**
 * @brief 
 *	A simple function to extract the current OpenGL errors.
 * @param function 
 *	It will print out the name of the function .
 * @param file 
 *	Which file is located will be printed out.
 * @param line 
 *	In which line is the error located at ?
 * @return 
 *	Check the console for the errors.
*/
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

GLApp::GLModel GLApp::mdl{};

int uTileSize = 32 ;
int s_min = 16 ;
int s_max = 256;
int step = 30;

GLuint images{ 0 };
float changes{ 0.0f };

GLboolean Tlast = false;
GLboolean Mlast = false;
GLboolean Alast = false;
int shaderMode = 0;
bool uModulate = false;
bool uBlend = false;

void GLApp::init ()
{
	glClearColor ( 1.f , 1.f , 1.f , 1.f );

	GLint w{ GLHelper::width } , h{ GLHelper::height };
	glViewport ( 0 , 0 , w , h );

	GLHelper::print_specs ();

	mdl.setup_vao ();
	mdl.setup_shdrpgm ();

	images = setup_texobj ( "../images/duck-rgba-256.tex" );
}

void GLApp::update ( double delta_time )
{
	mdl.update ( delta_time );
}

void GLApp::draw ()
{
	std::stringstream ss;
	ss << std::fixed;
	
	ss << std::fixed;
	ss.precision ( 2 );
	ss << GLHelper::title << " | Use T to change task | Use M to modulate mode | Use A to toggle alpha blending | FPS: " <<
		GLHelper::fps;

	glfwSetWindowTitle ( GLHelper::ptr_window , ss.str ().c_str () );

	// clear back buffer as before
	glClear ( GL_COLOR_BUFFER_BIT );

	// now, render rectangular model from NDC coordinates to viewport
	mdl.draw ();
}

void GLApp::cleanup ()
{}

GLuint GLApp::setup_texobj ( std::string pathname )
{
	GLuint width{ 256 } , height{ 256 } , bytes_per_texel{ 4 };
	GLuint byte_size = width * height * bytes_per_texel ;
	char* ptr_texels = new char[ byte_size ];
	// open binary file
	std::ifstream ifs{ pathname, std::ios::in | std::ios::binary };
	if( !ifs )
	{
		std::cout << "ERROR: Unable to open image file: "
			<< pathname << "\n";
		exit ( EXIT_FAILURE );
	}
	ifs.seekg ( 0 , std::ios::beg );
	int index = 0;
	// copy contents to heap memory pointed by ptr_texels
	while( !ifs.eof () )
	{
		ifs.read ( ptr_texels , byte_size );
	}
	ifs.close ();

	GLuint texobj_hdl;
	// define and initialize a handle to texture object that will
	// encapsulate two-dimensional textures
	glCreateTextures ( GL_TEXTURE_2D , 1 , &texobj_hdl );
	// allocate GPU storage for texture image data loaded from file
	glTextureStorage2D ( texobj_hdl , 1 , GL_RGBA8 , width , height );
	// copy image data from client memory to GPU texture buffer memory
	glTextureSubImage2D ( texobj_hdl , 0 , 0 , 0 , width , height , GL_RGBA , GL_UNSIGNED_BYTE , ptr_texels );
	// client memory not required since image is buffered in GPU memory
	delete [] ptr_texels;

	return texobj_hdl;
}


void GLApp::GLModel::setup_vao ()
{
	std::array <glm::vec2 , 4 > pos_vtx
	{
		glm::vec2 ( 1.0f, -1.0f ) , glm::vec2 ( 1.0f, 1.0f ),
		glm::vec2 ( -1.0f, 1.0f ) , glm::vec2 ( -1.0f, -1.0f )
	};
	std::array <glm::vec3 , 4 > clr_vtx
	{
		glm::vec3 ( 1.0f , 0.0f , 0.0f ) , glm::vec3 ( 0.0f , 1.0f , 0.0f ),
		glm::vec3 ( 0.0f , 0.0f , 1.0f ) , glm::vec3 ( 1.0f , 1.0f , 1.0f )
	};

	std::array <glm::vec2 , 4 > tex_coor
	{
		glm::vec2 ( 1.0f, 0.0f ) , glm::vec2 ( 1.0f, 1.0f ),
		glm::vec2 ( 0.0f, 1.0f ) , glm::vec2 ( 0.0f, 0.0f )
	};

	GLuint vbo ;
	glCreateBuffers ( 1 , &vbo ) ;
	glNamedBufferStorage ( vbo , sizeof ( glm::vec2 ) * pos_vtx.size () + sizeof ( glm::vec3 ) * clr_vtx.size () + sizeof ( glm::vec2 ) * tex_coor.size () , nullptr , GL_DYNAMIC_STORAGE_BIT ) ;
	glNamedBufferSubData ( vbo , 0 , sizeof ( glm::vec2 ) * pos_vtx.size () , pos_vtx.data () ) ;
	glNamedBufferSubData ( vbo , sizeof ( glm::vec2 ) * pos_vtx.size () , sizeof ( glm::vec3 ) * clr_vtx.size () , clr_vtx.data () );
	glNamedBufferSubData ( vbo , sizeof ( glm::vec2 ) * pos_vtx.size () + sizeof ( glm::vec3 ) * clr_vtx.size () ,
						   sizeof ( glm::vec2 ) * tex_coor.size () , tex_coor.data () );

	GLuint vao;
	glCreateVertexArrays ( 1 , &vao );

	GLCall ( glEnableVertexArrayAttrib ( vao , 0 ) );
	GLCall ( glVertexArrayVertexBuffer ( vao , 0 , vbo , 0 , sizeof ( glm::vec2 ) ) );
	GLCall ( glVertexArrayAttribFormat ( vao , 0 , 2 , GL_FLOAT , GL_FALSE , 0 ) );
	GLCall ( glVertexArrayAttribBinding ( vao , 0 , 0 ) );

	GLCall ( glEnableVertexArrayAttrib ( vao , 1 ) );
	GLCall ( glVertexArrayVertexBuffer ( vao , 1 , vbo , sizeof ( glm::vec2 ) * pos_vtx.size () , sizeof ( glm::vec3 ) ) );
	GLCall ( glVertexArrayAttribFormat ( vao , 1 , 3 , GL_FLOAT , GL_FALSE , 0 ) );
	GLCall ( glVertexArrayAttribBinding ( vao , 1 , 1 ) );

	GLCall ( glEnableVertexArrayAttrib ( vao , 2 ) );
	GLCall ( glVertexArrayVertexBuffer ( vao , 2 , vbo , sizeof ( glm::vec2 ) * pos_vtx.size () + sizeof ( glm::vec3 ) * clr_vtx.size () , sizeof ( glm::vec2 ) ) );
	GLCall ( glVertexArrayAttribFormat ( vao , 2 , 2 , GL_FLOAT , GL_FALSE , 0 ) );
	GLCall ( glVertexArrayAttribBinding ( vao , 2 , 2 ) );

	std::array<GLushort , 6> idx_vtx
	{
		0, 1, 2,
		2, 3, 0
	};

	GLuint ebo_hdl;
	glCreateBuffers ( 1 , &ebo_hdl );
	glNamedBufferStorage ( ebo_hdl , sizeof ( GLshort ) * idx_vtx.size () , idx_vtx.data () , GL_DYNAMIC_STORAGE_BIT );
	glVertexArrayElementBuffer ( vao , ebo_hdl );
	glBindVertexArray ( 0 );

	vaoid = vao;
	primitive_type = GL_TRIANGLES;
	primitive_cnt = idx_vtx.size ();
	draw_cnt = idx_vtx.size ();
}

void GLApp::GLModel::setup_shdrpgm ()
{
	std::vector<std::pair<GLenum , std::string>> shdr_files;
	shdr_files.push_back ( std::make_pair (
		GL_VERTEX_SHADER ,
		"../shaders/tutorial-5.vert" ) );
	shdr_files.push_back ( std::make_pair (
		GL_FRAGMENT_SHADER ,
		"../shaders/tutorial-5.frag" ) );
	shdr_pgm.CompileLinkValidate ( shdr_files );
	if( GL_FALSE == shdr_pgm.IsLinked () )
	{
		std::cout << "Unable to compile/link/validate shader programs" << "\n";
		std::cout << shdr_pgm.GetLog () << std::endl;
		std::exit ( EXIT_FAILURE );
	}
}


void GLApp::GLModel::update ( double delta_time )
{
	changes += ( float ) delta_time / 5.0f;
	if( changes > 3.14f )
	{
		changes = 0.0f;
	}
	uTileSize = s_min + ( int ) ( sin ( changes ) * s_max );

	if( GLHelper::keystateT && Tlast != GLHelper::keystateT )
	{
		shaderMode = shaderMode + 1 > 6 ? 0 : (shaderMode + 1);
	}

	// The user can toggle the modulate mode by pressing the keyboard button .
	if( GLHelper::keystateM && Mlast != GLHelper::keystateM )
	{
		uModulate = !uModulate;
	}

	// The user can toggle the transparency mode by pressing the keyboard button .
	if( GLHelper::keystateA && Alast != GLHelper::keystateA )
	{ 
		uBlend = !uBlend;
	}

	Tlast = GLHelper::keystateT;
	Mlast = GLHelper::keystateM;
	Alast = GLHelper::keystateA;
}

void GLApp::GLModel::draw ()
{
	glBindTextureUnit ( 6 , images );

	if( shaderMode == 4 )
	{
		// Repeating textures with modulated fragment color
		glTextureParameteri ( images , GL_TEXTURE_WRAP_S , GL_REPEAT );
		glTextureParameteri ( images , GL_TEXTURE_WRAP_T , GL_REPEAT );
	}
	else if( shaderMode == 5 )
	{
		// Repeating and mirroring texture mapping with modulated fragment color
		glTextureParameteri ( images , GL_TEXTURE_WRAP_S , GL_MIRRORED_REPEAT );
		glTextureParameteri ( images , GL_TEXTURE_WRAP_T , GL_MIRRORED_REPEAT );
	}
	else if( shaderMode == 6 )
	{
		// Clamping texture coordinates to edge of image
		glTextureParameteri ( images , GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE );
		glTextureParameteri ( images , GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE );
	}

	// suppose shdrpgm is the handle to shader program object
	// that will render the rectangular model
	shdr_pgm.Use ();
	glBindVertexArray ( vaoid );

	if( uBlend )
	{
		glEnable ( GL_BLEND );
		glBlendFunc ( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
	}
	else
	{
		glDisable ( GL_BLEND );
	}
	
	shdr_pgm.SetUniform ( "uTileSize" , uTileSize );
	shdr_pgm.SetUniform ( "uShaderMode" , shaderMode );
	shdr_pgm.SetUniform ( "uModulate" , uModulate );

	// tell fragment shader sampler uTex2d will use texture image unit 6
	GLuint tex_loc = glGetUniformLocation ( shdr_pgm.GetHandle () , "uTex2d" );
	glUniform1i ( tex_loc , 6 );

	glDrawElements ( primitive_type , draw_cnt , GL_UNSIGNED_SHORT , NULL );
	glBindVertexArray ( 0 );
	shdr_pgm.UnUse ();
}
