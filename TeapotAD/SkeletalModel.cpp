#include "SkeletalModel.h"


SkeletalModel::SkeletalModel(GLSLProgram* shaderProgIn)
{
	m_VAO = 0;
	pScene = NULL;

	//setShader(AssetManager::AssetManagerInstance()->getShaderManager()->getShaderProgram("deferred"));
	//setPosition(glm::vec3(0.0f, 0.0f, 0.0f)); //!< Sets the position of the object using a vec3
	//setRotation(glm::quat(0.0f, glm::vec3(1.0f, 0.0f, 0.0f))); //!< Sets the rotation of the object using a quaternion
	//setScale(glm::vec3(0.7f, 0.7f, 0.7f)); //!< Sets the scale of the object 
	m_NumBones = 0;

	m_pShaderProg = shaderProgIn;

}

SkeletalModel::~SkeletalModel()
{

}


void SkeletalModel::Clear()
{

	//if (m_Buffers[0] != 0) {
	//	glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	//}

	if (m_VAO != 0) {
		gl::DeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}


void SkeletalModel::LoadMesh(const std::string& Filename)
{
	// Release the previously loaded mesh (if it exists)
	Clear();

	// Create the VAO
	gl::GenVertexArrays(1, &m_VAO);
	gl::BindVertexArray(m_VAO);

	gl::GenBuffers(1, &vbo);
	gl::GenBuffers(1, &ebo);
	gl::GenBuffers(1, &boneBo);

	// Create the buffers for the vertices atttributes

	pScene = Importer.ReadFile(Filename.c_str(), 
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType | 
		aiProcess_Triangulate | 
		aiProcess_GenSmoothNormals | 
		aiProcess_FlipUVs |
		aiProcess_LimitBoneWeights);

	if (pScene) {

		m_GlobalInverseTransform = pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();

		// No global inverse transform, use youtube comment method to transpose assimp matrices to glm correctly 
		InitFromScene(pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
	}

	gl::BindVertexArray(0);
}

void SkeletalModel::InitFromScene(const aiScene* pScene, const std::string& Filename)
{	

	m_Entries.resize(pScene->mNumMeshes);
	//m_Textures.resize(pScene->mNumMaterials);

	// Prepare vectors for vertex attributes and indices
	std::vector<VertexStruct> vertices;
	std::vector<VertexBoneData> bones;
	std::vector<unsigned int> Indices;

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;

	// Count the number of vertices and indices
	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;

		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices

	vertices.reserve(NumVertices); // Check this if problems occur
	bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, vertices, Indices, bones);
	}

	// Generate and populate the buffers with vertex attributes and the indices
	gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
	gl::BufferData(gl::ARRAY_BUFFER, vertices.size() * sizeof(VertexStruct), &vertices[0],
		gl::STATIC_DRAW);

	// Vertex positions 
	gl::EnableVertexAttribArray(0);
	gl::VertexAttribPointer(0, 3, gl::FLOAT, FALSE, sizeof(VertexStruct), (GLvoid*)0);

	// Vertex Normals
	gl::EnableVertexAttribArray(1);
	gl::VertexAttribPointer(1, 3, gl::FLOAT, FALSE, sizeof(VertexStruct), (GLvoid*)offsetof(VertexStruct, normal));

	//// Vertex Texture Coords
	//gl::EnableVertexAttribArray(2);
	//gl::VertexAttribPointer(2, 2, gl::FLOAT, FALSE, sizeof(VertexStruct), (GLvoid*)offsetof(VertexStruct, uvs));


	// Bind the bone data buffer object
	gl::BindBuffer(gl::ARRAY_BUFFER, boneBo);
	gl::BufferData(gl::ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], gl::STATIC_DRAW);

	gl::EnableVertexAttribArray(2);
	gl::VertexAttribIPointer(2, 4, gl::INT, sizeof(VertexBoneData), (const GLvoid*)0);

	gl::EnableVertexAttribArray(3);
	gl::VertexAttribPointer(3, 4, gl::FLOAT, FALSE, sizeof(VertexBoneData), (const GLvoid*)16);



	gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ebo);
	gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0],
		gl::STATIC_DRAW);

	//gl::GetError() == GL_NO_ERROR;

}

const aiNodeAnim* SkeletalModel::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

void SkeletalModel::InitMesh(unsigned int index, const aiMesh* paiMesh, std::vector<VertexStruct>& Vertices, std::vector<GLuint>& Indices, std::vector<VertexBoneData>& Bones)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ?
			&(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		glm::vec3 glmTempPos = glm::vec3(pPos->x, pPos->y, pPos->z);
		glm::vec3 glmTempNormal = glm::vec3(pNormal->x, pNormal->y, pNormal->z);
		glm::vec2 glmTempUV = glm::vec2(pTexCoord->x, pTexCoord->y);


		VertexStruct v;
		v.position = glmTempPos;
		v.normal = glmTempNormal;
		v.uvs = glmTempUV;

		Vertices.push_back(v);
	}

	if (paiMesh->HasBones()){
		LoadBones(index, paiMesh, Bones);

	}

	// Populate the index buffer
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
	

}

void SkeletalModel::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{

	for (unsigned int i = 0; i < pMesh->mNumBones; i++) {

		unsigned int BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {

			BoneIndex = m_NumBones;
			m_NumBones++;

			BoneInfo bi;
			m_BoneInfo.push_back(bi);
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		m_BoneMapping[BoneName] = BoneIndex;

		//m_BoneInfo[BoneIndex].BoneOffset = toGlmMat4(&pMesh->mBones[i]->mOffsetMatrix);
		m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;

		// Corrected to conversion to glm matrix 

		for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {

			unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;

			Bones[VertexID].AddBoneData(BoneIndex, Weight);

		}

	}
}


void SkeletalModel::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{

	//unsigned int numIDs = sizeof(IDs) / sizeof(*IDs);
	
	//sizeof(array) / sizeof(array[0]
	for (unsigned int i = 0; i < 4; i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}

	}

	// should never get here - more bones than we have space for
	assert(0);
}

void SkeletalModel::BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms)
{
	Matrix4f Identity;
	Identity.InitIdentity();

	
	//glm::mat4 Identity = glm::mat4(1.0f); //Identity;
	//Identity.InitIdentity();

	float TicksPerSecond = pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, pScene->mAnimations[0]->mDuration);

	ReadNodeHeirarchy(AnimationTime, pScene->mRootNode, Identity);

	Transforms.resize(m_NumBones);

	for (unsigned int i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}

}




unsigned int SkeletalModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned int SkeletalModel::FindScale(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned int SkeletalModel::FindTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumPositionKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}



void SkeletalModel::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);

	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);

	Out = Out.Normalize();
}

void SkeletalModel::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScale(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End= pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	//aiVector3D::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void SkeletalModel::CalcInterpolatedTranslation(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}


	unsigned int PositionIndex = FindTranslation(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;

	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void SkeletalModel::SetBoneTransform(unsigned int Index, const Matrix4f& Transform )
{
	assert(Index < 100);

	m_pShaderProg->setUniformIndex(Index, Transform);
}

void SkeletalModel::render() const
{
	gl::BindVertexArray(m_VAO);

	for (unsigned int i = 0; i < m_Entries.size(); i++) {

		const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

		gl::DrawElementsBaseVertex(gl::TRIANGLES,
			m_Entries[i].NumIndices,
			gl::UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
			m_Entries[i].BaseVertex);
	}

	// Make sure the VAO is not changed from the outside 
	gl::BindVertexArray(0);
}

void SkeletalModel::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = pScene->mAnimations[0];


	Matrix4f NodeTransformation(pNode->mTransformation);

	// Function to check if problems
	const aiNodeAnim* pNodeAnim = NULL;

	for (unsigned i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnimIndex = pAnimation->mChannels[i];

		if (std::string(pNodeAnimIndex->mNodeName.data) == NodeName) {
			pNodeAnim = pNodeAnimIndex;
		}
	}


	//const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {

		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		Matrix4f ScalingM;
		ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());


		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedTranslation(Translation, AnimationTime, pNodeAnim);
		Matrix4f TranslationM;
		TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	
	Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;
	
	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		unsigned int BoneIndex = m_BoneMapping[NodeName];
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation *
			m_BoneInfo[BoneIndex].BoneOffset;
	}


	for (unsigned i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

bool SkeletalModel::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
	bool Ret = true;
	return Ret;
}

glm::mat4 SkeletalModel::toGlmMat4(const aiMatrix4x4* ai)
{
	glm::mat4 mat;

	mat[0][0] = ai->a1; mat[1][0] = ai->a2; mat[2][0] = ai->a3; mat[3][0] = ai->a4;
	mat[0][1] = ai->b1; mat[1][1] = ai->b2; mat[2][1] = ai->b3; mat[3][1] = ai->b4;
	mat[0][2] = ai->c1; mat[1][2] = ai->c2; mat[2][2] = ai->c3; mat[3][2] = ai->c4;
	mat[0][3] = ai->d1; mat[1][3] = ai->d2; mat[2][3] = ai->d3; mat[3][3] = ai->d4;

	return mat;
}
