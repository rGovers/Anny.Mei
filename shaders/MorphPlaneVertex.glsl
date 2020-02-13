#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec2[4] morphPlaneWeights;

uniform mat4 model;
uniform int morphSize;
uniform sampler2D morphTex;

out vec2 vTexCoord;

void main()
{
    vTexCoord = texCoords;

    mat4 morphMatrix;

    vec2 morphPos = texture2D(morphTex, vec2(morphPlaneWeights[0].x, 0)).xy;
    morphMatrixA[3] = vec4(morphPos, 0, 1);
    vec4 vertex = (morphMatrix * position) * morphPlaneWeights[0].y;

    morphPos = texture2D(morphTex, vec2(morphPlaneWeights[1].x, 0)).xy;
    morphMatrixA[3] = vec4(morphPos, 0, 1);
    vertex = (morphMatrix * position) * morphPlaneWeights[1].y + vertex;

    morphPos = texture2D(morphTex, vec2(morphPlaneWeights[2].x, 0)).xy;
    morphMatrixA[3] = vec4(morphPos, 0, 1);
    vertex = (morphMatrix * position) * morphPlaneWeights[2].y + vertex;

    morphPos = texture2D(morphTex, vec2(morphPlaneWeights[3].x, 0)).xy;
    morphMatrixA[3] = vec4(morphPos, 0, 1);
    vertex = (morphMatrix * position) * morphPlaneWeights[3].y + vertex;
    
    gl_Position = model * vec4(vertex.xyz, 1);
}
