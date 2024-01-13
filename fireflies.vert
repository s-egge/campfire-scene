#version 330 compatibility

out  vec3  vN;    // normal vector
out  vec3  vL;    // vector from point to light
out  vec3  vE;    // vector from point to eye
out  vec2  vST;   // (s,t) texture coordinates

// where the light is:
uniform float lightX, lightY, lightZ;
const vec3 LightPosition = vec3(0., 0., 0.);

void
main( )
{
        vST = gl_MultiTexCoord0.st;
        vec4 ECposition = gl_ModelViewMatrix * gl_Vertex;
        vN = normalize(gl_NormalMatrix * gl_Normal);
        vL = LightPosition - ECposition.xyz;
        vE = vec3(0., 0., 0.) - ECposition.xyz;
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
