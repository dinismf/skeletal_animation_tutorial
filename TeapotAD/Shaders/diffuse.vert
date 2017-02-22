#version 430

layout (location = 0) in vec3 VertexPosition; // Stream of vertex positions 
layout (location = 1) in vec3 VertexNormal; // Stream of vertex normals 

layout (location=2) in ivec4 BoneIDs; // Stream of vertex bone IDs
layout (location=3) in vec4 Weights; // Stream of vertex weights

out vec3 vertPos; // Vertex position in eye coords
out vec3 N; // Transformed normal

uniform mat3 NormalMatrix; // Normal matrix 
uniform mat4 M; // Model matrix 
uniform mat4 V; // View matrix 
uniform mat4 P; // Projection matrix 

const int MAX_BONES = 70; // Max number of bones
uniform mat4 gBones[MAX_BONES]; // Bone transformations 

void main()
{
	// Multiply each bone transformation by the particular weight
	// and combine them. 
   	mat4 BoneTransform = gBones[ BoneIDs[0] ] * Weights[0];
	BoneTransform += gBones[ BoneIDs[1] ] * Weights[1];
    BoneTransform += gBones[ BoneIDs[2] ] * Weights[2];
    BoneTransform += gBones[ BoneIDs[3] ] * Weights[3];

	// Transformed vertex position 
	vec4 tPos = BoneTransform * vec4(VertexPosition, 1.0);

	gl_Position = (P * V * M) * tPos;

	// Transformed normal 
	vec4 tNormal = BoneTransform * vec4(VertexNormal, 0.0);

	N = normalize( mat4(NormalMatrix) * tNormal).xyz;

    vec4 worldPos = M * tPos;

	vertPos = worldPos.xyz;     
}