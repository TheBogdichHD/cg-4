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
    vec4 numberSample = texture(uNumberTex, vTexCoord);

    float numberMask = clamp(uNumberMix, 0.0, 1.0) * numberSample.a;
    vec3 mergedTextures = mix(materialColor, numberSample.rgb, numberMask);
    vec3 tintedColor = uColor * mergedTextures;
    vec3 finalColor = mix(mergedTextures, tintedColor, clamp(uTextureMix, 0.0, 1.0));

    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}
