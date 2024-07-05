
#version 330

in  vec4 color;
in  vec2 texCoord;

out vec4 fColor;

uniform sampler2D mainTex;
uniform sampler2D cloudTex;
uniform sampler2D nightTex;
uniform sampler2D boundaryTex;
uniform sampler2D terrainTex;

void main() 
{ 
    vec4 earthColor = texture2D(mainTex, texCoord);
    vec4 cloudRed = texture2D(cloudTex, texCoord);
    vec4 nightColor = texture2D(nightTex, texCoord);
    vec4 boundaryColor = texture2D(boundaryTex, texCoord);
    vec4 terrainColor = texture2D(terrainTex, texCoord);
    vec4 cloudColor = vec4(cloudRed.x, cloudRed.x, cloudRed.x, cloudRed.a);


    vec4 litEarthColor = earthColor * color;


    vec4 combinedOverlay = 0.6 * cloudColor + 1 * nightColor + 20.0 * boundaryColor + 0.0 * terrainColor;

    fColor = litEarthColor + combinedOverlay;

    fColor = clamp(fColor, 0.0, 1.0);
} 
