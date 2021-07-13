/* !
@file    glpbo.h
@author  pghali@digipen.edu
@date    10/11/2016

This file contains the declaration of structure GLPbo that encapsulates the
functionality required to stream images generated by a 3D graphics pipe 
emulator executed on the CPU for display by the GPU.

Begin by implementing:
1) Overloaded functions GLPbo::set_clear_color() which emulate GL command
glClearColor(). These functions set GLPbo::clear_clr with the RGBA values 
specified by the function parameters.
2) Implement GLPbo::clear_color_buffer() which emulates GL command 
glClear(GL_COLOR_BUFFER_BIT).
This function uses the pointer GLPbo::ptr_to_pbo to fill the PBO with the
RGBA value specified by GLPbo::clear_clr.  Think of how this can be done
as efficiently as possible - std::fill() or std::memset() are candidates.
3) Implement GLPbo::init().
a) This function will set GLPbo::width, GLPbo::height, GLPbo::pixel_cnt, and
GLPbo::byte_cnt with appropriate values so that the PBO and texture object 
will have the same dimensions as the GL context's framebuffer. This means
that GLPbo::width and GLPbo::height must be equivalent to App::width and
App::height.
b) Set the clear color GLPbo::clear_clr to white using set_clear_color().
c) Initialize GLPbo::pboid by creating a PBO with an image store having 
dimensions width x height where each pixel (I use the term pixel rather 
than texel to conform to the GL spec) is a 32-bit RGBA value. Research 
GL commands glCreateBuffers() and glNamedBufferStorage().
d) Set GLPbo::ptr_to_pbo with PBO's address by calling glMapNamedBuffer().
e) Call GLPbo::clear_color_buffer() to fill the PBO memory store with 
value in GLPbo::clear_clr.
f) After GLPbo::clear_color_buffer() returns, release the pointer back to 
the GPU driver by calling glUnmapNamedBuffer().
g) Initialize GLPbo::texid by creating a texture object with storage for a 
texture image having the same dimensions as the PBO (which are both 
equivalent to the dimension of the GL context's colorbuffer). Research 
GL commands glCreateTextures() and glTextureStorage2D().
Now, initialize the texture image with the contents of the PBO. Carefully
research GL command glTextureSubImage2D().
h) Before the texture image can be rendered, GLPbo::vaoid must be initialized
with vertex buffers containing position and texture coordinates for a full-
window quad. The position vertices must be defined in NDC coordinates.
i) Likewise, GLPbo::shdr_pgm must be initialized with a shader program object.
Read the interface for GLSLShader in ./include/glslshader.h.
You must encode your vertex and fragment shaders in C-strings - please do not
submit shader files. The vertex shader ./shader/pass_thru_pos2d_tex2d.vert doesn't do
much - it just writes input NDC vertex to gl_Position and passes
the texture coordinate unchanged to subsequent stages.
The fragment shader obtains a color by sampling the texture image attached to GLPbo::texid.
4) Implement GLPbo::emulate(). In the future, this function will be augmented
with the 3D graphics pipe emulator. For now, to get images similar to the sample,
compute the RGBA value for each frame using time (obtained from GLFW) as the domain to 
sin() and cos() functions whose outputs (which can be negative values) are mapped to range [0, 255] -
the sin and cos mapped values are then mapped to the red and green components, 
respectively with blue set to 0. Use set_clear_color() to set the
color to fill the PBO. Next, set GLPbo::ptr_to_pbo with a call to glMapNamedBuffer() 
to map PBO address to client address space. Call clear_color_buffer() to fill the
PBO with the previously computed color. Release the address returned by
glMapNamedBuffer() back to the graphics driver by calling glUnmapNamedBuffer().
Use glTextureSubImage2D() to DMA the contents of the PBO to GLPbo::texid's
texture image store.
5) In the game loop's draw(), you go thro' the regular process of rendering
a textured quad with the texture object whose texture memory was updated in
the previous step. This is done by calling GLPbo::draw_fullwindow_quad().
*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLPBO_H
#define GLPBO_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glslshader.h> // GLSLShader class definition

/*  _________________________________________________________________________ */
struct GLPbo
  /*! GLPbo structure to encapsulate 3D graphics pipe emulator which will write
  an image composed of RGBA values to PBO, copy the image from PBO to image 
  store of a texture object, and finally use this texture image to render a 
  full-window quad that will display the emulator's output (phew).
  */
{
  // forward declaration
  union Color;

  //-------------- static member function declarations here -----------------
  
  // this is where the emulator does its work of emulating the graphics pipe
  // it generates images using set_pixel to write to the PBO
  static void emulate();
  
  // render quad using the texture image generated by render()
  static void draw_fullwindow_quad();

  // initialization and cleanup stuff ...
  static void init(GLsizei w, GLsizei h);
  static void setup_quad_vao();
  static void setup_shdrpgm();
  static void cleanup();

  // ---------------static data members are declared here ----------------
  
  // Storage requirements common to emulator, PBO and texture object
  static GLsizei width, height; // dimensions of buffers
  // rather than computing these values many times, compute once in
  // GLPbo::init() and then forget ...
  static GLsizei pixel_cnt, byte_cnt; // how many pixels and bytes
  // pointer to PBO's memory chunk - must be set every frame with the value 
  // returned by glMapNamedBuffer()
  static Color *ptr_to_pbo;

  // geometry and material information ...
  static GLuint vaoid;        // with GL 4.5, VBO & EBO are not required
  static GLuint elem_cnt;     // how many indices in element buffer
  static GLuint pboid;        // id for PBO
  static GLuint texid;        // id for texture object
  static GLSLShader shdr_pgm; // object that abstracts away nitty-gritty
                              // details of shader management

// --- here we're trying to emulate GL's functions for clearing colorbuffer ---

  // overloaded functions akin to glClearcolor that set GLPbo::clear_clr
  // with user-supplied parameters ...
  static void set_clear_color(GLPbo::Color);
  static void set_clear_color(GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255);

  // akin to glClear(GL_COLOR_BUFFER_BIT) - uses the pointer
  static void clear_color_buffer();

  // what value to use for clearing color buffer?
  static Color clear_clr;

  union Color
    /*! glm doesn't have type unsigned char [4] - therefore we declare our
    own type.
    Since Color is such a common type name in graphics applications, make
    sure to encapsulate the name in scope GLPbo!!!
    */
  {
    struct {
      GLubyte r, g, b, a; // a, b, g, r
    };
    GLubyte val[4]; // treat <r,g,b,a> as array of 8-bit unsigned values
    GLuint raw;     // treat <r,g,b,a> as 32-bit unsigned value

    Color(GLubyte re = 0, GLubyte gr = 0, GLubyte bl = 0, GLubyte al = 255) :
      r(re), g(gr), b(bl), a(al) {}
  };
  
};

#endif /* GLPBO_H */
