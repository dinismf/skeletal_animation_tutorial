
#include "glutils.h"
#include "gl_core_4_3.hpp"


#include <cstdio>

GLUtils::GLUtils() {}

int GLUtils::checkForOpenGLError(const char * file, int line) {
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = gl::GetError();
    while (glErr != NO_ERROR)
    {
		const char * message = "";
		switch( glErr )
		{
		case gl::INVALID_ENUM:
			message = "Invalid enum";
			break;
		case gl::INVALID_VALUE:
			message = "Invalid value";
			break;
		case gl::INVALID_OPERATION:
			message = "Invalid operation";
			break;
		case gl::INVALID_FRAMEBUFFER_OPERATION:
			message = "Invalid framebuffer operation";
			break;
		case gl::OUT_OF_MEMORY:
			message = "Out of memory";
			break;
		default:
			message = "Unknown error";
		}

        printf("glError in file %s @ line %d: %s\n", file, line, message);
        retCode = 1;
        glErr = gl::GetError();
    }
    return retCode;

}

void GLUtils::dumpGLInfo(bool dumpExtensions) {
    const GLubyte *renderer = gl::GetString( gl::RENDERER );
    const GLubyte *vendor = gl::GetString( gl::VENDOR );
    const GLubyte *version = gl::GetString( gl::VERSION );
    const GLubyte *glslVersion = gl::GetString( gl::SHADING_LANGUAGE_VERSION );

    GLint major, minor;
    gl::GetIntegerv(gl::MAJOR_VERSION, &major);
    gl::GetIntegerv(gl::MINOR_VERSION, &minor);

    printf("GL Vendor    : %s\n", vendor);
    printf("GL Renderer  : %s\n", renderer);
    printf("GL Version   : %s\n", version);
    printf("GL Version   : %d.%d\n", major, minor);
    printf("GLSL Version : %s\n", glslVersion);

    if( dumpExtensions ) {
        GLint nExtensions;
        gl::GetIntegerv(gl::NUM_EXTENSIONS, &nExtensions);
        for( int i = 0; i < nExtensions; i++ ) {
            printf("%s\n", gl::GetStringi(gl::EXTENSIONS, i));
        }
    }
}
