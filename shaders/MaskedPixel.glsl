#version 410

uniform sampler2D MainTex;
uniform sampler2D MaskTex;
uniform vec2 ScreenSize;

uniform mat4 camProjection;

in vec2 vTexCoord;

out vec4 color;

void main()
{
    // I tried other methods but the accuracy was not quite high enough so fragment coords was the soulution that seems to have the least shit looking lines
    // Other methods resulted very slight but noticable cutting and bleeding on the masks edges
    if (texture2D(MaskTex, vec2(gl_FragCoord.x / ScreenSize.x, gl_FragCoord.y / ScreenSize.y)).r >= 0.95f)
    {
        discard;
    }

    color = texture2D(MainTex, vTexCoord.xy);

    if (color.a == 0)
    {
        discard;
    }
}
