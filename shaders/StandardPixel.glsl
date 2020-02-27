#version 410

uniform sampler2D MainTex;

in vec2 vTexCoord;

out vec4 color;

void main()
{
    color = texture2D(MainTex, vTexCoord.xy);

    if (color.a == 0)
    {
        discard;
    }
}
