#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 vertPos; //Vertex position in eye coords
out vec3 N; //Transformed normal

uniform mat3 NormalMatrix;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
   N = normalize( NormalMatrix * VertexNormal);

   vertPos = vec3(M * vec4(VertexPosition,1.0)); 
     
   gl_Position = P * V * M * vec4(VertexPosition,1.0);
}
