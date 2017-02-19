
#include "AnimationScene.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;

#include "defines.h"

using glm::vec3;


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////
// Default constructor
/////////////////////////////////////////////////////////////////////////////////////////////
AnimationScene::AnimationScene()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Initialise the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::initScene(QuatCamera camera)
{
	prog = new GLSLProgram();

    //|Compile and link the shader  
	compileAndLinkShader();

	prog->initialiseBoneUniforms();

    gl::Enable(gl::DEPTH_TEST);

 	//Set up the lighting
	setLightParams(camera);

	// Initialise skeletal mesh. 
	m_AnimatedModel = new SkeletalModel(prog);

	m_AnimatedModel->LoadMesh("Assets/Minotaur@Attack.FBX");
	//m_AnimatedModel->LoadMesh("Assets/Majora.fbx");

	//m_AnimatedModel->setPosition(0, 0, -5);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Update
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::update(long long f_StartTime, float f_Interval)
{
	//m_AnimationModel.Update(t);
	// Vector of bone transformation matrices. 
	std::vector<Matrix4f> Transforms;

	float RunningTime = (float)(glfwGetTime() - (double)f_StartTime);


	m_AnimatedModel->BoneTransform(RunningTime, Transforms);

	for (unsigned int i = 0; i < Transforms.size(); i++) {
		m_AnimatedModel->SetBoneTransform(i, Transforms[i]);
	}

}


/////////////////////////////////////////////////////////////////////////////////////////////
// Set up the lighting variables in the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::setLightParams(QuatCamera camera)
{
	vec3 worldLight = vec3(0.0f, 20.0f, 0.0f);
	vec3 spotlightDirection = vec3(0.0f, -1.0f,  0.0f);
	vec3 spotlightIntensity = vec3(0.3f, 0.3f, 0.3f);

	prog->setUniform("Spotlight.position", worldLight );
	prog->setUniform("Spotlight.direction", spotlightDirection);
	prog->setUniform("Spotlight.intensity", spotlightIntensity);
	prog->setUniform("Spotlight.exponent", 2.0f);
	prog->setUniform("Spotlight.cutoff", 20.0f);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Render the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::render(QuatCamera camera)
{
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	// Model matrix 
	model = mat4(1.0f);
	//model = glm::translate(glm::vec3(0.0, -20.0, 0.0));
	//model = glm::scale(glm::vec3(0.2));

	setMatrices(camera);

	 //Set the Teapot material properties in the shader and render
	prog->setUniform("Ka", vec3(0.225f, 0.125f, 0.0f));
	prog->setUniform("Kd", vec3(1.0f, 0.6f, 0.0f));
	prog->setUniform("Ks", vec3(1.0f, 1.0f, 1.0f));
	prog->setUniform("specularShininess", 1.0f);

	m_AnimatedModel->render();
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Send the MVP matrices to the GPU
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::setMatrices(QuatCamera camera)
{
    mat4 mv = camera.view() * model;
    prog->setUniform("ModelViewMatrix", mv);

    prog->setUniform("MVP", camera.projection() * mv);

	// the correct matrix to transform the normal is the transpose of the inverse of the M matrix
	mat3 normMat = glm::transpose(glm::inverse(mat3(model)));

	prog->setUniform("M", model);
	prog->setUniform("NormalMatrix", normMat);
	prog->setUniform("V", camera.view() );
	prog->setUniform("P", camera.projection() );
}

/////////////////////////////////////////////////////////////////////////////////////////////
// resize the viewport
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::resize(QuatCamera camera, int w, int h)
{
    gl::Viewport(0,0,w,h);
    width = w;
    height = h;
	camera.setAspectRatio((float)w/h);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compile and link the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::compileAndLinkShader()
{
   
	try {
    	prog->compileShader("Shaders/diffuse.vert");
    	prog->compileShader("Shaders/diffuse.frag");
    	prog->link();
    	prog->validate();
    	prog->use();
    } catch(GLSLProgramException & e) {
 		cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}