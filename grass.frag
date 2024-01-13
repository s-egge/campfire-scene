#version 330 compatibility

flat in int colorChange;
vec3 myColor = vec3(0.09, 0.38, 0.05);


void 
main() {

   if (colorChange == 1)
		myColor = vec3(0.03, 0.44, 0.09);

   if (colorChange == 2)
		myColor = vec3(0.06, 0.22, 0.06);


   gl_FragColor = vec4( myColor,  1. );
}