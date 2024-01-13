#version 330 compatibility

uniform float   uKa, uKd, uKs;   // coefficients of each type of lighting -- make sum to 1.0
uniform vec3    uColor;          // object color
uniform vec3    uSpecularColor;  // light color
uniform float   uShininess;      // specular exponent

uniform float uTime;
uniform float ffGlowVariation;

// in variables from the vertex shader and interpolated in the rasterizer:
in  vec3  vN;              // normal vector
in  vec3  vL;              // vector from point to light
in  vec3  vE;              // vector from point to eye
in  vec2  vST;             // (s,t) texture coordinates

void 
main() {
    vec3 Normal = normalize(vN);
    vec3 Light  = normalize(vL);
    vec3 Eye    = normalize(vE);

	vec3 myColor = vec3(0.72, 1, 0);

    // apply the per-fragment lighting to myColor:
    vec3 ambient = uKa * myColor;

    float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
    vec3 diffuse = uKd * dd * myColor;

    float ss = 0.;
    if( dot(Normal,Light) > 0. )          // only do specular if the light can see the point
    {
            vec3 ref = normalize(  reflect( -Light, Normal )  );
            ss = pow( max( dot(Eye,ref),0. ), uShininess );
    }

    vec3 specular = uKs * ss * uSpecularColor;

    float glow = 1.5 + 0.5 * sin(uTime * 2.0 * 3.14159 * (5 + ffGlowVariation));

    gl_FragColor = vec4( (ambient + diffuse + specular) * glow,  1. );
}