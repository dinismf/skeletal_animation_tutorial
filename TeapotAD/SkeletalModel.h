#ifndef SKELETALMODEL_H
#define SKELETALMODEL_H

#include "gl_core_4_3.hpp"
#include "glm\glm.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla
#include <vector>
#include <map>
#include "Drawable.h"
#include "Math3D.h"
#include "glslprogram.h"

struct VertexStruct 
{
	glm::vec3 position; //!< Vertex position 
	glm::vec3 normal; //!< Vertex normal
	glm::vec2 uvs; //!< Vertex uv's
};

struct VertexBoneData
{
	unsigned int IDs[4]; //!< An array of 4 bone Ids that influence a single vertex.
	float Weights[4]; //!< An array of the weight influence per bone. 

	VertexBoneData()
	{
		// 0's out the arrays. 
		Reset();
	}

	void Reset()
	{
		memset(IDs, 0, 4 * sizeof(IDs[0]));
		memset(Weights, 0, 4 * sizeof(Weights[0]));
	}

	void AddBoneData(unsigned int BoneID, float Weight)
	{
		for (unsigned int i = 0; i < 4; i++) {

			// Check to see if there are any empty weight values. 
			if (Weights[i] == 0.0) {
				// Add ID of bone. 
				IDs[i] = BoneID;

				// Set the vertex weight influence for this bone ID. 
				Weights[i] = Weight;
				return;
			}

		}
		// should never get here - more bones than we have space for
		assert(0);
	}
};

// Stores bone information
struct BoneInfo
{
	Matrix4f FinalTransformation; // Final transformation to apply to vertices 
	Matrix4f BoneOffset; // Initial offset from local to bone space. 

	BoneInfo()
	{
		BoneOffset.SetZero();
		FinalTransformation.SetZero();
	}
};

// A mesh entry for each mesh read in from the Assimp scene. A model is usually consisted of a collection of these. 
#define INVALID_MATERIAL 0xFFFFFFFF
struct MeshEntry {


	unsigned int BaseVertex; //!< Total number of mesh indices. 
	unsigned int BaseIndex; //!< The base vertex of this mesh in the vertices array for the entire model.
	unsigned int NumIndices; //!< The base index of this mesh in the indices array for the entire model. 
	unsigned int MaterialIndex; 

	MeshEntry()
	{

		NumIndices = 0; 
		BaseVertex = 0;  
		BaseIndex = 0; 
		MaterialIndex = INVALID_MATERIAL;
	}

	~MeshEntry() {}
};

class SkeletalModel : public Drawable
{
public:

	SkeletalModel(GLSLProgram* shaderProgIn); //!< Constructor 

	~SkeletalModel(); //!< Destructor 

	void LoadMesh(const std::string& Filename); //!< Loads an animated mesh from a given file path

	void BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms); //!< Traverses the scene hierarchy and fetches the matrix transformation for each bone given the time. 

	void SetBoneTransform(unsigned int Index, const Matrix4f& Transform); //!< Inserts a bone transformation in the uniform array at the given index. 

	void render() const override; //!< Renders each mesh in the model. 

private:
	
	void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones); //!< Loads the bone data from a given mesh. 
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Calculates the interpolated quaternion between two keyframes. 
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Calculates the interpolated scaling vector between two keyframes. 
	void CalcInterpolatedTranslation(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Calculates the interpolated translation vector between two keyframes. 

	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim); //!< Finds a rotation key given the current animation time. 
	unsigned int FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim); // Finds a scaling key given the current animation time. 
	unsigned int FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim); // Finds a translation key given the current animation time. 

	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform); //!< Recursive function that traverses the scene's node hierarchy and combines the matrix transformations. 


	void InitFromScene(const aiScene* pScene, const std::string& Filename); //!< Prepares the model for rendering. 
	void InitMesh(unsigned int index, const aiMesh* paiMesh, std::vector<VertexStruct>& Vertices, 
		std::vector<GLuint>& Indices, std::vector<VertexBoneData>& Bones); //!< Fetches mesh data from given Assimp mesh. 

	void Clear(); //!< Deletes the vertex array object. 

	GLSLProgram* m_pShaderProg;

	GLuint m_VAO; // Vertex array object. 
	GLuint vbo; //!< Vertex buffer object. 
	GLuint ebo; //!< Indices buffer object. 
	GLuint boneBo; //!< Bone data buffer object. 

	const aiScene* pScene; //!< The Assimp aiScene object. 

	Assimp::Importer Importer; //!< Assimp importer object for reading in files into the aiScene. 

	unsigned int m_NumBones; //!< Total number of bones in the model. 

	std::map<std::string, unsigned int> m_BoneMapping; //!< Map of bone names to ids

	std::vector<BoneInfo> m_BoneInfo; //!< Array containing bone information such as offset matrix and final transformation. 
	
	Matrix4f GlobalTransformation; //!< Root node transformation. 
	Matrix4f m_GlobalInverseTransform; 

	std::vector<MeshEntry> m_Entries; //!< Array of mesh entries 
};


#endif