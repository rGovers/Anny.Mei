#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec4 positionE;
layout (location = 3) in vec4 positionW;
layout (location = 4) in vec4 positionN;
layout (location = 5) in vec4 positionS;

uniform mat4 Model;
uniform vec2 Lerp;

out vec2 vTexCoord;

void main()
{
    vTexCoord = texCoords;

    vec2 dE = positionE.xy - position.xy;
    vec2 dW = positionW.xy - position.xy;
    vec2 dN = positionN.xy - position.xy;
    vec2 dS = positionS.xy - position.xy;

    float lenE = length(dE);
    float lenW = length(dW);
    float lenN = length(dN);
    float lenS = length(dS);

    dE /= lenE;
    dW /= lenW;
    dN /= lenN;
    dS /= lenS;

    vec2 sE = lenE != 0 ? dE * (lenE * max(0, Lerp.x))  : vec2(0);
    vec2 sW = lenW != 0 ? dW * (lenW * max(0, -Lerp.x)) : vec2(0);
    vec2 sN = lenN != 0 ? dN * (lenN * max(0, Lerp.y))  : vec2(0);
    vec2 sS = lenS != 0 ? dS * (lenS * max(0, -Lerp.y)) : vec2(0);

    vec2 p = position.xy + sE + sW + sN + sS; 

    gl_Position = Model * vec4(p.xy, position.z, 1);
}
