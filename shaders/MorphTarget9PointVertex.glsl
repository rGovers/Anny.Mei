#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec4 positionN;
layout (location = 3) in vec4 positionS;
layout (location = 4) in vec4 positionE;
layout (location = 5) in vec4 positionW;
layout (location = 6) in vec4 positionNE;
layout (location = 7) in vec4 positionSE;
layout (location = 8) in vec4 positionSW;
layout (location = 9) in vec4 positionNW;

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

    vec2 dNE = positionNE.xy - position.xy;
    vec2 dSE = positionSE.xy - position.xy;
    vec2 dSW = positionSW.xy - position.xy;
    vec2 dNW = positionNW.xy - position.xy;

    float lenE = length(dE);
    float lenW = length(dW);
    float lenN = length(dN);
    float lenS = length(dS);

    float lenNE = length(dNE);
    float lenSE = length(dSE);
    float lenSW = length(dSW);
    float lenNW = length(dNW);

    dE /= lenE;
    dW /= lenW;
    dN /= lenN;
    dS /= lenS;

    dNE /= lenNE;
    dSE /= lenSE;
    dSW /= lenSW;
    dNW /= lenNW;

    float lrpN = max(0.0f, 1.0f - length(vec2(0.0f, 1.0f) - Lerp));
    float lrpS = max(0.0f, 1.0f - length(vec2(0.0f, -1.0f) - Lerp));
    float lrpE = max(0.0f, 1.0f - length(vec2(1.0f, 0.0f) - Lerp));
    float lrpW = max(0.0f, 1.0f - length(vec2(-1.0f, 0.0f) - Lerp));

    float lrpNE = max(0.0f, 1.0f - length(vec2(1.0f, 1.0f) - Lerp));
    float lrpSE = max(0.0f, 1.0f - length(vec2(1.0f, -1.0f) - Lerp));
    float lrpSW = max(0.0f, 1.0f - length(vec2(-1.0f, -1.0f) - Lerp));
    float lrpNW = max(0.0f, 1.0f - length(vec2(-1.0f, 1.0f) - Lerp));

    vec2 sN = lenN != 0 ? dN * (lenN * lrpN) : vec2(0);
    vec2 sS = lenS != 0 ? dS * (lenS * lrpS) : vec2(0);
    vec2 sE = lenE != 0 ? dE * (lenE * lrpE) : vec2(0);
    vec2 sW = lenW != 0 ? dW * (lenW * lrpW) : vec2(0);

    vec2 sNE = lenNE != 0 ? dNE * (lenNE * lrpNE) : vec2(0);
    vec2 sSE = lenSE != 0 ? dSE * (lenSE * lrpSE) : vec2(0);
    vec2 sSW = lenSW != 0 ? dSW * (lenSW * lrpSW) : vec2(0);
    vec2 sNW = lenNW != 0 ? dNW * (lenNW * lrpNW) : vec2(0);

    vec2 p = position.xy + sE + sW + sN + sS + sNE + sSE + sSW + sNW; 

    gl_Position = Model * vec4(p.xy, position.z, 1);
}
