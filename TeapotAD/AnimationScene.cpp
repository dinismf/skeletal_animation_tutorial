
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


namespace imat3111
{
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
    //|Compile and link the shader  
	compileAndLinkShader();

    gl::Enable(gl::DEPTH_TEST);

 	//Set up the lighting
	setLightParams(camera);


	//Create the plane to represent the ground
	plane = new VBOPlane(100.0,100.0,100,100);

	m_AnimationModel = SkeletalModel();
	loadAnimatedModel();
	m_AnimationModel.PrepareToDraw();

}

/////////////////////////////////////////////////////////////////////////////////////////////
//Update
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::update(float t)
{
	m_AnimationModel.Update(t);
}

void AnimationScene::loadAnimatedModel()
{
	Assimp::Importer Importer;
	//const aiScene* pScene = Importer.ReadFile("Assets/Fan_Done5_Rigged.fbx",
	const aiScene* pScene = Importer.ReadFile("Assets/Minotaur@Attack.FBX",
	//const aiScene* pScene = Importer.ReadFile("Assets/spidercollada.dae", 
	//const aiScene* pScene = Importer.ReadFile("Assets/Majora.fbx",

		aiProcess_LimitBoneWeights |
		aiProcess_Triangulate);

	AssimpConverter::Convert(pScene, m_AnimationModel);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set up the lighting variables in the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::setLightParams(QuatCamera camera)
{
	vec3 worldLight = vec3(0.0f, 20.0f, 0.0f);
	vec3 spotlightDirection = vec3(0.0f, -1.0f,  0.0f);
	vec3 spotlightIntensity = vec3(0.3f, 0.3f, 0.3f);

	prog.setUniform("Spotlight.position", worldLight );
	prog.setUniform("Spotlight.direction", spotlightDirection);
	prog.setUniform("Spotlight.intensity", spotlightIntensity);
	prog.setUniform("Spotlight.exponent", 2.0f);
	prog.setUniform("Spotlight.cutoff", 20.0f);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Render the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::render(QuatCamera camera)
{
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

	//Initialise the model matrix for the plane
	model = mat4(1.0f);

	//setLightParams(camera);
    setMatrices(camera);
	
	//Set the plane's material properties in the shader and render
	prog.setUniform("Ka", vec3(0.0f, 0.25f, 0.0f));
	prog.setUniform("Kd", vec3(0.0f, 1.0f, 0.0f));
	prog.setUniform("Ks", vec3(1.0f, 1.0f, 1.0f));
	prog.setUniform("specularShininess", 0.2f);

	plane->render();

	//Now set up the teapot 
	model = mat4(1.0f);
	//model = glm::translate(glm::vec3(0.0, -20.0, 0.0));
	model = glm::scale(glm::vec3(0.3));

	setMatrices(camera);

	 //Set the Teapot material properties in the shader and render
	prog.setUniform("Ka", vec3(0.225f, 0.125f, 0.0f));
	prog.setUniform("Kd", vec3(1.0f, 0.6f, 0.0f));
	prog.setUniform("Ks", vec3(1.0f, 1.0f, 1.0f));
	prog.setUniform("specularShininess", 1.0f);


	m_AnimationModel.UpdateBuffers();
	m_AnimationModel.Render();
	
}


/////////////////////////////////////////////////////////////////////////////////////////////
//Send the MVP matrices to the GPU
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::setMatrices(QuatCamera camera)
{
    mat4 mv = camera.view() * model;
    prog.setUniform("ModelViewMatrix", mv);

    //prog.setUniform("NormalMatrix", mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));

    prog.setUniform("MVP", camera.projection() * mv);

	// the correct matrix to transform the normal is the transpose of the inverse of the M matrix
	mat3 normMat = glm::transpose(glm::inverse(mat3(model)));

	prog.setUniform("M", model);
	prog.setUniform("NormalMatrix", normMat);
	prog.setUniform("V", camera.view() );
	prog.setUniform("P", camera.projection() );
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
    	prog.compileShader("Shaders/diffuse.vert");
    	prog.compileShader("Shaders/diffuse.frag");
    	prog.link();
    	prog.validate();
    	prog.use();
    } catch(GLSLProgramException & e) {
 		cerr << e.what() << endl;
 		exit( EXIT_FAILURE );
    }
}

}
