#version 330 compatibility

//lighting
out  vec3  vN;    // normal vector
out  vec3  vL;    // vector from point to light
out  vec3  vE;    // vector from point to eye

flat out int colorChange;

uniform float uTime;
uniform float uXmin, uXmax;
uniform float uZmin, uZmax;
uniform float uPeriodx, uPeriodz;
uniform int uNumx, uNumz;
uniform float centerRadius;
const float TWOPI = 2.*3.14159265;

void 
main( )
{
    colorChange = 0;

    // ix/iz represent where in the "grid" the current blade is going to be drawn
    int ix = gl_InstanceID % uNumx;
    int iz = gl_InstanceID / uNumx;

    // these are the x/z world coordinates
    float x = uXmin + float(ix) * (uXmax-uXmin) / float(uNumx-1);
    float z = uZmin + float(iz) * (uZmax-uZmin) / float(uNumz-1);

    vec4 vert = vec4( x, gl_Vertex.y, z, gl_Vertex.w );

    // generate a pseudo-random value -0.1 - 0.2 based on x,z position to 
    // adjust to grass height and reduce uniformity
    vec2 seed = vec2(x, z);
    float randHeight = fract(sin(dot(seed, vec2(x, z))) * 43758.5453);
    vert.y += -0.1 + randHeight * 0.3;

    //change some of the grass so it's less uniform
    if(ix % 4 == 0 && iz % 2 == 0){
        colorChange = 1;
    }

    else if(ix % 2 == 1 && iz % 3 == 0){
        colorChange = 2;
    }

    
    // the next five lines make the grass "bend" and "wave" based on time
    float kx = cos( TWOPI * uTime * float(ix) / uPeriodx );
    float kz = sin( TWOPI * uTime * float(iz) / uPeriodz );

    float ysq = vert.y*vert.y;
    vert.x += kx * ysq;
    vert.z += kz * ysq;
    

    // if the blade of grass is within the origin circle of the radius provided,
    // clear the position to nothing and return
    if (length(vec2(x, z)) < centerRadius) {
        gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    gl_Position = gl_ModelViewProjectionMatrix * vert;
}