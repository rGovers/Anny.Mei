#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;

uniform mat4 model;

out vec2 vTexCoord;

void main()
{
    vTexCoord = texCoords;

    gl_Position = model * position;
}
