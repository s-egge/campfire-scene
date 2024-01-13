#version 330 compatibility

// lighting uniform variables -- these can be set once and left alone:
uniform float   uKa, uKd, uKs;   // coefficients of each type of lighting -- make sum to 1.0
uniform vec3    uColor;          // object color
uniform vec3    uSpecularColor;  // light color
uniform float   uShininess;      // specular exponent

// texture variables
uniform sampler2D uTexUnit0;
uniform sampler2D uTexUnit1;

// in variables from the vertex shader and interpolated in the rasterizer:
in  vec3  vN;              // normal vector
in  vec3  vL;              // vector from point to light
in  vec3  vE;              // vector from point to eye
in  vec2  vST;             // (s,t) texture coordinates

void
main( )
{
        vec3 Normal = normalize(vN);
        vec3 Light  = normalize(vL);
        vec3 Eye    = normalize(vE);

        float s = vST.s;
        float t = vST.t;

        vec3 dayColor = texture(uTexUnit0, vST).rgb;
        vec3 nightColor = texture(uTexUnit1, vST).rgb;

        vec3 myColor;

        if ( s >= 0 && s < 0.45) {
                float beginning = smoothstep(0.0, 0.05, s);
                myColor = mix(nightColor, dayColor, beginning);
        }

        else if ( s >= 0.45 && s <= 1.) {
                float middle = smoothstep(0.5, 0.55, s);
                myColor = mix(dayColor, nightColor, middle);
        }

        // make middle-ish daytime brighter than the edges
        if (s >= 0.05 && s < 0.25) {
                myColor *= smoothstep(0.05, 0.25, s) + 1.0;
        }

        else if (s >= 0.25 && s < 0.5) {
                myColor *= smoothstep(0.5, 0.25, s) + 1.0;
        }
                

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
        gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}
