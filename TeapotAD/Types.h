#pragma once

// GLM
//#include <glm/glm.hpp>

#include <glm\glm.hpp>
#include <glm\ext.hpp>

// STD
#include <vector>
#include <string>

namespace SA
{

	struct VertexStruct
	{
		glm::vec3 position; //!< Vertex position 
		glm::vec3 normal; //!< Vertex normal
		//glm::vec2 uvs; //!< Vertex uv's
	};

	struct sWeight
	{
		unsigned int VertexID;
		float Weight;
	};

	struct sBone
	{
		std::string Name;

		glm::mat4x4 NodeTransform;
		glm::mat4x4 OffsetMatrix; // T-Pose to local bone space
		glm::mat4x4 FinalTransformation;

		unsigned int NumWeights;
		sWeight* pWeights;

		unsigned int NumChildren;
		unsigned int* pChildren;
	};

	// An animated mesh structure which stores mesh related data 
	struct sAnimatedMesh
	{

		sAnimatedMesh()
		{

			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
		}

		~sAnimatedMesh() {}


		GLuint m_VertexArrayObject; 
		GLuint m_VertexBufferObject;
		GLuint m_IndexBufferObject;

		unsigned int NumVertices;
		unsigned int NumIndices;

		unsigned int BaseVertex;
		unsigned int BaseIndex;

		//glm::vec3* pVertices;
		//glm::vec3* pNormals;

		//glm::vec3* pTransformedVertices;
		//glm::vec3* pTransformedNormals;


		std::vector<VertexStruct> aVertexData;
		std::vector<VertexStruct> aTransformedVertexData;

		std::vector<GLuint> vIndices;
	};

	// The skeleton of bones stored as a vector. 
	struct sSkeleton
	{
		std::vector<sBone> Bones;
	};

	template <typename _Ty>
	struct sNodeAnimationKey
	{
		_Ty Value;
		float Time;
	};

	// 
	struct sNodeAnimation
	{
		std::string Name;

		std::vector<sNodeAnimationKey<glm::vec3> > PositionKeys;
		std::vector<sNodeAnimationKey<glm::quat> > RotationKeys;
	};

	struct sAnimation
	{
		std::vector<sNodeAnimation> NodeAnimations;

		float TicksPerSecond;
		float Duration;
	};
}