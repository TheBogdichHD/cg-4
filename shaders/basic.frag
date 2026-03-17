#version 330 core
in vec2 vTexCoord;

uniform sampler2D uNumberTex;
uniform sampler2D uMaterialTex;
uniform vec3 uColor;
uniform float uNumberMix;
uniform float uTextureMix;

out vec4 FragColor;

void main()
{
    vec3 materialColor = texture(uMaterialTex, vTexCoord).rgb;
    vec3 numberColor = texture(uNumberTex, vTexCoord).rgb;

    vec3 mergedTextures = mix(materialColor, numberColor, clamp(uNumberMix, 0.0, 1.0));
    vec3 colorTintedTextures = uColor * mergedTextures;
    vec3 finalColor = mix(uColor, colorTintedTextures, clamp(uTextureMix, 0.0, 1.0));

    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}
