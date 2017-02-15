#ifndef ANIMATIONSCENE_H
#define ANIMATIONSCENE_H

#include "gl_core_4_3.hpp"

#include <GLFW/glfw3.h>
#include "scene.h"
#include "glslprogram.h"

#include "vboplane.h"

#include <glm/glm.hpp>

#include <assimp\Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

#include "AssimpConverter.h"
#include "SkeletalModel.h"

using glm::mat4;
using namespace SA;


namespace imat3111
{

class AnimationScene : public Scene
{
private:
    GLSLProgram prog;

    int width, height;

	SkeletalModel m_AnimationModel;

	VBOPlane *plane;  //Plane VBO

    mat4 model; //Model matrix

	void loadAnimatedModel();

    void setMatrices(QuatCamera camera); //Set the camera matrices

    void compileAndLinkShader(); //Compile and link the shader

public:
	AnimationScene(); //Constructor

	void setLightParams(QuatCamera camera); //Setup the lighting

    void initScene(QuatCamera camera); //Initialise the scene

    void update( float t ); //Update the scene

    void render(QuatCamera camera);	//Render the scene

    void resize(QuatCamera camera, int, int); //Resize
};

}

#endif // SCENEDIFFUSE_H
