
#include "vboplane.h"
#include "defines.h"
#include "glutils.h"

#include "gl_core_4_3.hpp"

#include "glutils.h"




#include <cstdio>
#include <cmath>

VBOPlane::VBOPlane(float xsize, float zsize, int xdivs, int zdivs)
{


    faces = xdivs * zdivs;
    float * v = new float[3 * (xdivs + 1) * (zdivs + 1)];
	float * n = new float[3 * (xdivs + 1) * (zdivs + 1)];
    float * tex = new float[2 * (xdivs + 1) * (zdivs + 1)];
    unsigned int * el = new unsigned int[6 * xdivs * zdivs];

    float x2 = xsize / 2.0f;
    float z2 = zsize / 2.0f;
    float iFactor = (float)zsize / zdivs;
    float jFactor = (float)xsize / xdivs;
    float texi = 1.0f / zdivs;
    float texj = 1.0f / xdivs;
    float x, z;
    int vidx = 0, tidx = 0;
    for( int i = 0; i <= zdivs; i++ ) {
        z = iFactor * i - z2;
        for( int j = 0; j <= xdivs; j++ ) {
            x = jFactor * j - x2;
            v[vidx] = x;
            v[vidx+1] = 0.0f;
            v[vidx+2] = z;
			n[vidx] = 0.0f;
			n[vidx+1] = 1.0f;
			n[vidx+2] = 0.0f;
            vidx += 3;
            tex[tidx] = j * texi;
            tex[tidx+1] = i * texj;
            tidx += 2;
        }
    }

    unsigned int rowStart, nextRowStart;
    int idx = 0;
    for( int i = 0; i < zdivs; i++ ) {
        rowStart = i * (xdivs+1);
        nextRowStart = (i+1) * (xdivs+1);
        for( int j = 0; j < xdivs; j++ ) {
            el[idx] = rowStart + j;
            el[idx+1] = nextRowStart + j;
            el[idx+2] = nextRowStart + j + 1;
            el[idx+3] = rowStart + j;
            el[idx+4] = nextRowStart + j + 1;
            el[idx+5] = rowStart + j + 1;
            idx += 6;
        }
    }

    unsigned int handle[4];
    gl::GenBuffers(4, handle);

	gl::GenVertexArrays( 1, &vaoHandle );
    gl::BindVertexArray(vaoHandle);

    gl::BindBuffer(gl::ARRAY_BUFFER, handle[0]);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * (xdivs+1) * (zdivs+1) * sizeof(float), v, gl::STATIC_DRAW);
    gl::VertexAttribPointer( (GLuint)0, 3, gl::FLOAT, FALSE, 0, ((GLubyte *)NULL + (0)) );
    gl::EnableVertexAttribArray(0);  // Vertex position

	gl::BindBuffer(gl::ARRAY_BUFFER, handle[1]);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * (xdivs+1) * (zdivs+1) * sizeof(float), n, gl::STATIC_DRAW);
    gl::VertexAttribPointer( (GLuint)1, 3, gl::FLOAT, FALSE, 0, ((GLubyte *)NULL + (0)) );
    gl::EnableVertexAttribArray(1);  // Vertex normal

    gl::BindBuffer(gl::ARRAY_BUFFER, handle[2]);
    gl::BufferData(gl::ARRAY_BUFFER, 2 * (xdivs+1) * (zdivs+1) * sizeof(float), tex, gl::STATIC_DRAW);
    gl::VertexAttribPointer( (GLuint)2, 2, gl::FLOAT, FALSE, 0, ((GLubyte *)NULL + (0)) );
    gl::EnableVertexAttribArray(2);  // Texture coords

    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, handle[3]);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, 6 * xdivs * zdivs * sizeof(unsigned int), el, gl::STATIC_DRAW);

    gl::BindVertexArray(0);
    
    delete [] v;
	delete [] n;
    delete [] tex;
    delete [] el;
}

void VBOPlane::render() const {
    //GLUtils::checkForOpenGLError(__FILE__,__LINE__);
    gl::BindVertexArray(vaoHandle);
    gl::DrawElements(gl::TRIANGLES, 6 * faces, gl::UNSIGNED_INT, ((GLubyte *)NULL + (0)));
    //GLUtils::checkForOpenGLError(__FILE__,__LINE__);
	gl::BindVertexArray(0);

}
