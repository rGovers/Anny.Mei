#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec2[4] morphPlaneWeights;

uniform mat4 Model;
uniform uint MorphSize;
uniform sampler2D MorphTex;

out vec2 vTexCoord;

void main()
{
    vTexCoord = texCoords;

    mat4 morphMatrix = mat4(0);
    mat4 transformMatrix = mat4(0);

    vec2 morphPos = texture2D(MorphTex, vec2(morphPlaneWeights[0].x / MorphSize, 0)).xy;
    transformMatrix[3] = vec4(morphPos, 0, 1);
    morphMatrix += transformMatrix * morphPlaneWeights[0].y;

    morphPos = texture2D(MorphTex, vec2(morphPlaneWeights[1].x / MorphSize, 0)).xy;
    transformMatrix[3] = vec4(morphPos, 0, 1);
    morphMatrix += transformMatrix * morphPlaneWeights[1].y;

    morphPos = texture2D(MorphTex, vec2(morphPlaneWeights[2].x / MorphSize, 0)).xy;
    transformMatrix[3] = vec4(morphPos, 0, 1);
    morphMatrix += transformMatrix * morphPlaneWeights[2].y;

    morphPos = texture2D(MorphTex, vec2(morphPlaneWeights[3].x / MorphSize, 0)).xy;
    transformMatrix[3] = vec4(morphPos, 0, 1);
    morphMatrix += transformMatrix * morphPlaneWeights[3].y;

    gl_Position = Model * morphMatrix * position;
}
