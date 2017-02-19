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

class SkeletalModel : public Drawable
{
public:
	SkeletalModel(GLSLProgram* shaderProgIn);

	~SkeletalModel();

	void LoadMesh(const std::string& Filename);

	//static const unsigned int MAX_BONES = 100;

	void BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms);

	void SetBoneTransform(unsigned int Index, const Matrix4f& Transform);

	void render() const override;

private:

	struct BoneInfo
	{
		Matrix4f FinalTransformation;
		Matrix4f BoneOffset;

		BoneInfo()
		{
			BoneOffset.SetZero();
			FinalTransformation.SetZero(); 
		}
	};

	struct VertexBoneData
	{
		unsigned int IDs[4];
		float Weights[4];

		VertexBoneData()
		{
			Reset();
		}

		void Reset()
		{
			memset(IDs, 0, 4 * sizeof(IDs[0]));
			memset(Weights, 0, 4 * sizeof(Weights[0]));
		}

		void AddBoneData(unsigned int BoneID, float Weight);
	};
	
	void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedTranslation(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform /*const glm::mat4& ParentTransform*/);


	void InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(unsigned int index, const aiMesh* paiMesh, std::vector<VertexStruct>& Vertices,
		std::vector<GLuint>& Indices, std::vector<VertexBoneData>& Bones);

	bool InitMaterials(const aiScene* pScene, const std::string& Filename);

	void Clear();

	GLSLProgram* m_pShaderProg;


#define INVALID_MATERIAL 0xFFFFFFFF

	GLuint m_VAO;
	GLuint vbo;
	GLuint ebo;
	GLuint boneBo;
	//GLuint m_boneLocation[100];
	struct MeshEntry {

		MeshEntry()
		{

			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		~MeshEntry(){}


		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};

	const aiScene* pScene;

	Assimp::Importer Importer;

	unsigned int m_NumBones;

	std::map<std::string, unsigned int> m_BoneMapping; //!< Map of bone names to ids

	std::vector<BoneInfo> m_BoneInfo;
	
	Matrix4f GlobalTransformation;
	Matrix4f m_GlobalInverseTransform;

	glm::mat4 toGlmMat4(const aiMatrix4x4* ai);

	std::vector<MeshEntry> m_Entries;
};

#endif