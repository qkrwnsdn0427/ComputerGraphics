#version 330

layout (location = 0) in  vec4 vPosition;
layout (location = 1) in  vec3 vNormal;
layout (location = 2) in  vec2 vTexCoord;

out vec4 color;
out vec2 texCoord;

uniform mat4 ModelView;
uniform mat4 Projection;

// Lighting variables
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;
uniform mat4 ModelViewLight;

void main() 
{    
    // Transform vertex position into eye coordinates
    vec3 pos = (ModelView * vPosition).xyz;
    vec3 L = normalize( LightPosition.xyz - pos );
    //vec3 L = normalize( (ModelView * LightPosition).xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( (ModelView * vec4(vNormal, 0.0)).xyz );

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd * DiffuseProduct;

    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    color = ambient + diffuse + specular;
    color.a = 1.0;
    texCoord = vTexCoord;

    gl_Position = Projection * ModelView * vPosition;
}