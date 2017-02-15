#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace imat3111
{

class QuatCamera 
{
public:

	QuatCamera();  //Constructor

	const glm::vec3& position() const; //position getter method
	void setPosition(const glm::vec3& position); //position setter method

	float fieldOfView() const; //fieldOfView getter method
	void setFieldOfView(float fieldOfView); //fieldOfView setter method

	float aspectRatio() const; //aspectRatio getter method
	void setAspectRatio(float aspectRatio);  //aspectRatio setter method

	float nearPlane() const;  //nearPlane getter method
	float farPlane() const;   //farPlane getter method
	void setNearAndFarPlanes(float nearPlane, float farPlane); //nearPlane and farPLane setter method


	void rotate(const float yaw, const float pitch); //Rotate camera
	void pan(const float x, const float y);  //Pan camera
	void roll(const float z); //Roll camera
	void zoom(const float z); //Zoom camera


	void updateView();  //Update the camera

	void reset(void); //Reset the camera

	glm::mat4 view(); //Get the View matrix

	glm::mat4 projection(); //Get the Projection matrix
	

private:

	float _fieldOfView;
	float _nearPlane;
	float _farPlane;
	float _aspectRatio; 

	//The camera coordinate axes
	glm::vec3 _xaxis;
	glm::vec3 _yaxis;
	glm::vec3 _zaxis;

	//Camera position vector and Quaternoin to represent camera orientation
	glm::vec3 _position;
	glm::quat _orientation;

	glm::mat4 _view;
	glm::mat4 _projection;


};

}
