#ifndef ANIMATIONSCENE_H
#define ANIMATIONSCENE_H

#include "gl_core_4_3.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <assimp\Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

#include "scene.h"
#include "glslprogram.h"
#include "SkeletalModel.h"

using glm::mat4;

class AnimationScene : public Scene
{
private:
    GLSLProgram* prog; //!< Shader program 

    int width, height;

	mat4 model; // Model matrix

	SkeletalModel* m_AnimatedModel; //!< The skeletal model 

    void setMatrices(QuatCamera camera); //Set the camera matrices

    void compileAndLinkShader(); //Compile and link the shader

public:
	AnimationScene(); //Constructor

	void setLightParams(QuatCamera camera); //Setup the lighting

    void initScene(QuatCamera camera); //Initialise the scene

    void update(long long f_startTime, float f_Interval); //Update the scene

    void render(QuatCamera camera);	//Render the scene

    void resize(QuatCamera camera, int, int); //Resize
};

#endif
