#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <ctype.h>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Samantha Egge

// title of these windows:

const char *WINDOWTITLE = "OpenGL / GLUT Final Project -- Samantha Egge";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 900;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE     = GL_LINEAR;
const GLfloat FOGDENSITY  = 0.30f;
const GLfloat FOGSTART    = 1.5f;
const GLfloat FOGEND      = 4.f;

// for lighting:

const float	WHITE[ ] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 40000;		// 10000 milliseconds = 10 seconds


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
GLuint	SphereDL;				// object display list
GLuint  GridDL;
GLuint  PineTreeDL;
GLuint  CampFireDL;
GLuint  TentDL;
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		// ORTHO or PERSP
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
bool    textureTesting;
bool    Frozen;
bool    ViewToggle;

//handle wasd key movement
float eyeD = 0.1f;
float eyeX = 0.f;
float eyeY = 2.f;
float eyeZ = -6.f;

// global grass variables
int UNUMXZ = 1000;


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );
void			HsvRgb( float[3], float [3] );
void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);
float			Unit(float [3]);


// utility to create an array from 3 separate values:

float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3( float factor, float array0[ ] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float *
MulArray3(float factor, float a, float b, float c )
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
//#include "osucone.cpp"
//#include "osutorus.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
#include "glslprogram.cpp"
//#include "vertexbufferobject.cpp"


enum TextureIDNames
{
	MOON,
	DIRT,
	PINETREE,
	CAMPFIRE,
	TENT
};

char* TextureFiles[] =
{
	(char*)"moon.bmp",
	(char*)"dirtTexture.bmp",
	(char*)"pineTreeTex.bmp",
	(char*)"campFireTexture.bmp",
	(char*)"tentTex.bmp"
};

int	   NumTextureIDs = 5;
GLuint TextureIDs[5];


unsigned char* Texture;
GLuint		   DaySkyTex;
GLuint		   NightSkyTex;
GLSLProgram	   Pattern;
GLSLProgram	   Grass;
GLSLProgram    FirefliesShader;
VertexBufferObject Blade;
VertexBufferObject FireflyVBO;


// FireFly class definition

class FireFly {

private:
	float    ffMinXZ = -10.f;
	float    ffMaxXZ = 10.f;
	float    ffMinY = 1.5f;
	float    ffMaxY = 10.f;
public:
	float glowVariation;
	float startX;
	float startZ;
	float startY;
	Keytimes ktX;
	Keytimes ktY;
	Keytimes ktZ;

	void Initialize() {
		// generate random values for startX, startZ, and startY
		startX = ffMinXZ + (float)(rand()) / (RAND_MAX / (ffMaxXZ - ffMinXZ));
		startZ = ffMinXZ + (float)(rand()) / (RAND_MAX / (ffMaxXZ - ffMinXZ));
		startY = ffMinY + (float)(rand()) / (RAND_MAX / (ffMaxY - ffMinY));

		float variation = -1.f + 2.f * ((float)(rand()) / RAND_MAX);
		float glowVariation = (float)(rand()) / (RAND_MAX / 5.f);

		ktX.Init();
		ktX.AddTimeValue(0.0, startX);
		ktX.AddTimeValue(10.0, startX + variation);
		ktX.AddTimeValue(15.0, startX);
		ktX.AddTimeValue(20.0, startX - variation);
		ktX.AddTimeValue(25.0, startX);
		ktX.AddTimeValue(30.0, startX - variation);
		ktX.AddTimeValue((float)(MS_PER_CYCLE/1000), startX);


		ktY.Init();
		ktY.AddTimeValue(0.0, startY);
		ktY.AddTimeValue(10.0, startY + variation);
		ktY.AddTimeValue(15.0, startY);
		ktY.AddTimeValue(20.0, startY - variation);
		ktY.AddTimeValue(25.0, startY);
		ktY.AddTimeValue(30.0, startY + variation);
		ktY.AddTimeValue((float)(MS_PER_CYCLE / 1000), startY);

		ktZ.Init();
		ktZ.AddTimeValue(0.0, startZ);
		ktZ.AddTimeValue(5.0, startZ - variation - 0.2f);
		ktZ.AddTimeValue(10.0, startZ + variation);
		ktZ.AddTimeValue(15.0, startZ - variation);
		ktZ.AddTimeValue(20.0, startZ);
		ktZ.AddTimeValue(25.0, startZ - variation + 0.2f);
		ktZ.AddTimeValue(35.0, startZ + variation);
		ktZ.AddTimeValue((float)(MS_PER_CYCLE / 1000), startZ);
	};
};

int     NumFireFlies = 50;
FireFly fireflies[50];


// Fire Particle definition
struct FireParticle {
	float x, y, z;
	float vx, vy, vz;
	float lifetime;
};

// this will simulate the flames of the campfire
class Fire {

private:
	int maxParticles = 50000;
	int particleIncrement = 75;
	int numParticles;
	const float originX = 0.f;
	const float originY = 0.3f;
	const float originZ = 0.f;
	const float maxRadius = 0.6f;
	const float minLife = 1.0f;
	const float maxLife = 6.f;
	const float maxVXZ = 0.1f;
	const float minVY = 0.05f;
	const float maxVY = 0.2f;
	float deltaTime = 0.025f;
	bool daytime = false;

	void ResetParticle(FireParticle* particle) {
		float angle = (float)rand() / RAND_MAX * (2 * M_PI);
		float radius = (float)rand() / RAND_MAX * maxRadius;

		particle->x = radius * cos(angle);
		particle->y = originY;
		particle->z = radius * sin(angle);
		particle->vx = (float)rand() / RAND_MAX * (maxVXZ * 2) - maxVXZ;
		particle->vy = (float)rand() / RAND_MAX * minVY + maxVY;
		particle->vz = (float)rand() / RAND_MAX * (maxVXZ * 2) - maxVXZ;
		particle->lifetime = (float)(rand()) / RAND_MAX * maxLife + minLife;
	};

	void InitParticles(int num) {
		for (int i = 0; i < num; i++)
		{
			FireParticle particle;
			ResetParticle(&particle);
			flames.push_back(particle);
		}
	};

public:
	std::vector<FireParticle> flames;

	void Init() 
	{
		flames.clear();
		int numParticles = particleIncrement;
		InitParticles(numParticles);
	};

	void Switch()
	{
		if (daytime)
		{
			daytime = false;
			Init();
		}
		else
		{
			daytime = true;
			numParticles = 0;
		}
		
	}

	void Update() 
	{
		// if night time, keep incrementing fire particles so fire "grows"
		if (!daytime && (numParticles < maxParticles)) 
		{
			InitParticles(particleIncrement);
			numParticles += particleIncrement;
		}


		for (auto& particle : flames) 
		{
			float distanceFromOrigin = sqrt((particle.x * particle.x)
											+ (particle.y * particle.y)
											+ (particle.z * particle.z));

			// update position based on velocity and time multiplier
			particle.x += particle.vx * deltaTime;
			particle.y += particle.vy * deltaTime;
			particle.z += particle.vz * deltaTime;

			particle.lifetime -= deltaTime;

			// at end of lifetime, reset particle if it's night, remove if it's day
			if (particle.lifetime <= 0) 
			{
				if (!daytime)
					ResetParticle(&particle);
			}
		}

		if (daytime)
		{ // a little bit hard to read, this removes particles that reach their lifetime during the day
			//see: https://stackoverflow.com/questions/4713131/removing-item-from-vector-while-iterating 
			flames.erase(std::remove_if(flames.begin(), flames.end(),
				[](const FireParticle& particle) { return particle.lifetime <= 0.0f; }),
				flames.end());
		}

	};

	void Draw()
	{

		for (const auto& particle : flames) {

			float distanceFromOrigin = sqrt((particle.x * particle.x) 
											+ (particle.y * particle.y) 
											+ (particle.z * particle.z));

			// set a warm light pointing down at the campfire to simulate the fire's glow
			// might need to rework this
			glEnable(GL_LIGHT1);
			GLfloat lightPosition[] = { 0.0f, 1.0f, 0.0f, 1.0f };
			GLfloat lightColor[] = { 1.0f, 0.8f, 0.2f, 1.0f };
			glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor);
			glDisable(GL_LIGHTING);

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glColor4f(1.0f, (1.7f - distanceFromOrigin), 0.0f, 0.2f);
			glTranslatef(particle.x, particle.y, particle.z);
			glScalef(0.2, 0.2, 0.2);
			FireflyVBO.DrawInstanced(1);
			glPopMatrix();


		}

		glEnable(GL_LIGHTING);
	};
};

Fire CampFireFlames;
bool fireOn = true;

// main program:

int
main( int argc, char *argv[ ] )
{
	// seed random generator
	srand(time(0));

	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display lists that **will not change**:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow( MainWindow );

	// erase the background:
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
#ifdef DEMO_DEPTH_BUFFER
	if( DepthBufferOn == 0 )
		glDisable( GL_DEPTH_TEST );
#endif


	// specify shading to be flat:

	glShadeModel( GL_FLAT );

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( NowProjection == ORTHO )
		glOrtho( -2.f, 2.f,     -2.f, 2.f,     0.1f, 1000.f );
	else
		gluPerspective( 70.f, 1.f,	0.1f, 1000.f );

	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );


	// set the eye position, look-at position, and up-vector:
	if (ViewToggle)
		gluLookAt(-4.f, 0.3f, -4.f, 
				  10.f, 10.f, 10.f, 
				  0.f, 1.f, 0.f);
	else
		gluLookAt(eyeX, eyeY, eyeZ, 
			      0.f, 0.f, 0.f, 
			      0.f, 1.f, 0.f);


	// rotate the scene:
	glRotatef((GLfloat)Yrot, 0.f, 1.f, 0.f);
	glRotatef((GLfloat)Xrot, 1.f, 0.f, 0.f);

	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[NowColor][0] );
		glCallList( AxesList );
	}

	// since we are using glScalef( ), be sure the normals get unitized:
	glEnable( GL_NORMALIZE );

	if (Frozen)
		return;

	// ----------------------------------------- display top ------------------------ //

	float RADIUS = 3.f;
	float angle = 2.0 * F_PI * Time + (RADIUS * F_PI);

	float lightZ = 0.f;
	float lightX = 15.f * cos(angle); 
	float lightY = 15.f * sin(angle);

	// lighting
	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);


	SetSpotLight(GL_LIGHT0, lightX, lightY, lightZ, -lightX, -lightY, -lightZ, 1., 1., 1.);

	GLfloat ambient[] = { .1f, .1f, .1f, .1f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	GLfloat specular[] = { 0.1, 0.1, 0.1, 1.0 };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glEnable(GL_TEXTURE_2D); // -- texture issue, moved this here from down below sky sphere

	// Objs:

	//if (textureTesting) {
	Pattern.Use();

	// large sky sphere & ground
	glPushMatrix();

	// bind two separate textures to two separate TUNs
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, DaySkyTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, NightSkyTex);
	Pattern.SetUniformVariable("uTexUnit0", 1);
	Pattern.SetUniformVariable("uTexUnit1", 2);

	glRotatef(360.f * Time, 0., 0., 1.);
	glRotatef(90, 1.f, 0.f, 0.f);
	glScalef(15., 15., 15.);
	glCallList( SphereDL );

	glPopMatrix();

	Pattern.UnUse();
	//}
	
	//glEnable(GL_TEXTURE_2D); // --texture issue, moved this from here to up above sky sphere
	glActiveTexture(GL_TEXTURE0); // -- texture issue, added this here

	// ground
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, TextureIDs[DIRT]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// darken underneath so it's less reflective
	SetMaterial(0.2, 0.14, 0.07, 0.f);
	glCallList(GridDL);
	glPopMatrix();


	// tree texture
	glBindTexture(GL_TEXTURE_2D, TextureIDs[PINETREE]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// trees
	int treeRadius = 14.f;
	int numTrees = 40;

	for (int i = 0; i < 5; i++) {
		numTrees -= i;
		treeRadius -= (i * 0.5f);

		for (int x = 0; x < numTrees; x++) {
			float angle = 2.0 * F_PI * x / numTrees;

			float treeZ = (treeRadius + (x % 2)) * cos(angle);
			float treeX = (treeRadius + (x % 2)) * sin(angle);
			float treeY;

			if (x % 2 == 0) {
				treeY = 0.0f;
			}

			else
				treeY = -0.4f;

			if (i % 2 == 0) {
				treeX -= 0.2f;
				treeZ -= 0.2f;
			}

			glPushMatrix();

			glTranslatef(treeX, treeY, treeZ);
			glCallList(PineTreeDL);

			glPopMatrix();
		}
	}

	// campfire
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, TextureIDs[CAMPFIRE]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glCallList(CampFireDL);

	glPopMatrix();

	// tent
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, TextureIDs[TENT]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTranslatef(0.f, 0.f, 5.f);
	glCallList(TentDL);

	glPopMatrix();

	// moon
	glPushMatrix();
	SetMaterial(1., 1., 1., 1.);
	glTranslatef((-15.f * cos(angle)), (-15.f * sin(angle)), 0.f);

	glBindTexture(GL_TEXTURE_2D, TextureIDs[MOON]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glScalef(0.5, 0.5, 0.5);
	glCallList(SphereDL);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	// "fire flies"

	// # msec into the cycle ( 0 - MSEC-1 ):
	int msec = glutGet(GLUT_ELAPSED_TIME) % MS_PER_CYCLE;
	// turn that into a time in seconds:
	float nowSecs = (float)msec / 1000.f;

	// show the fireflies and turn fire on if it's nighttime
	if ((-15.f * sin(angle)) > -5.f)
	{
		if (!fireOn)
		{
			CampFireFlames.Switch();
			fireOn = true;
		}

		float minXZ = 0.f;
		float maxXZ = 10.f;
		float minY = 1.5f;
		float maxY = 10.f;

		for (int i = 0; i < NumFireFlies; i++)
		{
			float x = fireflies[i].ktX.GetValue(nowSecs);
			float y = fireflies[i].ktY.GetValue(nowSecs);
			float z = fireflies[i].ktZ.GetValue(nowSecs);

			// draw fireflies
			glPushMatrix();
			FirefliesShader.Use();
			FirefliesShader.SetUniformVariable("uTime", Time);
			FirefliesShader.SetUniformVariable("ffGlowVariation", fireflies[i].glowVariation);
			glTranslatef(x, y, z);
			FireflyVBO.DrawInstanced(1);
			FirefliesShader.UnUse();
			glPopMatrix();

		}
		
	}
	else if (fireOn)
	{
		CampFireFlames.Switch();
		fireOn = false;
	}

	// turn fire "off" if it's day time
	// fire ? testing here
	CampFireFlames.Draw();
	CampFireFlames.Update();
	


	// grass, turn on the shader and set the time:
	glPushMatrix();
	Grass.Use();
	Grass.SetUniformVariable("uTime", Time);
    Blade.DrawInstanced(UNUMXZ * UNUMXZ);
	Grass.UnUse();
	glPopMatrix();


	glDisable(GL_LIGHTING);
	// sun
	glPushMatrix();
	
	glColor3f(1.f, 1.f, 0.31f);
	glTranslatef((15.f * cos(angle)), (15.f * sin(angle)), 0.f);
	glScalef(0.8, 0.8, 0.8);
	glCallList(SphereDL);
	glPopMatrix();
	

#ifdef DEMO_Z_FIGHTING
	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.f,   0.f, 1.f, 0.f );
			glCallList( SphereDL);
		glPopMatrix( );
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	glDisable( GL_DEPTH_TEST );
	glColor3f( 0.f, 1.f, 1.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1.f, 1.f, 1.f );
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}

// bind a texture file to a texure ID
void CreateGlobalTextureID(GLuint& textureID, char* filename) 
{
	int width, height;
	unsigned char* textureArray0 = BmpToTexture(filename, &width, &height);

	fprintf(stderr, "Texture %s created, width/height: %d/%d\n", filename, width, height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureArray0);
}

// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE + 200, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc( Animate );

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

	for (int x = 0; x < NumTextureIDs; x++)
	{
		CreateGlobalTextureID(TextureIDs[x], TextureFiles[x]);
	}

	// -------- shader textures ----------//

	// bind day sky texture
	glGenTextures(1, &DaySkyTex);
	int nums, numt;
	Texture = BmpToTexture("daySkyTexture.bmp", &nums, &numt);
	glBindTexture(GL_TEXTURE_2D, DaySkyTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, nums, numt, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// bind night sky texture
	glGenTextures(1, &NightSkyTex);
	Texture = BmpToTexture("nightSkyTexture.bmp", &nums, &numt);
	glBindTexture(GL_TEXTURE_2D, NightSkyTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, nums, numt, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);


	//------------------- shader files ------------------ //

	// -- this is for the sky/environment sphere, I didn't change the original file name from the tutorial...
	// -- TODO: fix the naming here
	Pattern.Init();
	bool valid = Pattern.Create("pattern.vert", "pattern.frag");
	if (!valid) {
		fprintf(stderr, "Something went wrong with loading pattern.vert or pattern.frag\n");
	}
	else {
		Pattern.SetUniformVariable("uKa", 0.4f);
		Pattern.SetUniformVariable("uKd", 0.2f);
		Pattern.SetUniformVariable("uKs", 0.2f);
		Pattern.SetUniformVariable("uSpecularColor", 1.f, 1.f, 1.f);
		Pattern.SetUniformVariable("uShininess", (1.f));
	}

	// grass shader
	Grass.Init();
	valid = Grass.Create("grass.vert", "grass.frag");
	if (!valid)
		fprintf(stderr, "Shader cannot be created!\n");
	else {
		fprintf(stderr, "Shader created.\n");

		Grass.SetVerbose(false);
		Grass.SetUniformVariable("uNumx", UNUMXZ);
		Grass.SetUniformVariable("uNumz", UNUMXZ);
		Grass.SetUniformVariable("uXmin", -15.f);
		Grass.SetUniformVariable("uXmax", 15.f);
		Grass.SetUniformVariable("uZmin", -15.f);
		Grass.SetUniformVariable("uZmax", 15.f);
		Grass.SetUniformVariable("uPeriodx", 30.f);
		Grass.SetUniformVariable("uPeriodz", 30.f);
		Grass.SetUniformVariable("centerRadius", 6.f);
	}


	// single blade of grass VBO 

	int NUMPOINTS = 10;
	float YMIN = 0.f;
	float YMAX = 0.2f;

	Blade.Init();
	Blade.glBegin(GL_LINE_STRIP);
	for (int i = 0; i < NUMPOINTS; i++)
	{
		float y = YMIN + (float)i * (YMAX - YMIN) / (float)(NUMPOINTS - 1);
		Blade.glVertex3f(0., y, 0.);
	}
	Blade.glEnd();


	//  fireflies shader

	FirefliesShader.Init();
	valid = FirefliesShader.Create("fireflies.vert", "fireflies.frag");
	if (!valid)
		fprintf(stderr, "Shader cannot be created!\n");
	else {
		fprintf(stderr, "Shader created.\n");

		FirefliesShader.SetVerbose(false);
		FirefliesShader.SetUniformVariable("uKa", 0.2f);
		FirefliesShader.SetUniformVariable("uKd", 0.4f);
		FirefliesShader.SetUniformVariable("uKs", 0.4f);
		FirefliesShader.SetUniformVariable("uSpecularColor", 1.f, 1.f, 1.f);
		FirefliesShader.SetUniformVariable("uShininess", (1.f));
	}


	//  single firefly 
	FireflyVBO.Init();
	OsuSphereVBO(0.05f, 40, 40, &FireflyVBO);
	FireflyVBO.glEnd();


	// ------------- Firefly initial position set-up ---------------//
	for (int i = 0; i < NumFireFlies; i++)
	{
		fireflies[i].Initialize();
	}

	//initialize the fire
	CampFireFlames.Init();
}




// ------- grid variables
#define XSIDE	30	// length of the x side of the grid
#define X0   (-XSIDE/2.)		// where one side starts
#define NX	500		// how many points in x
#define DX	( XSIDE/(float)NX )	// change in x between the points

#define YGRID	0.f

#define ZSIDE	30			// length of the z side of the grid
#define Z0      (-ZSIDE/2.)		// where one side starts
#define NZ	500			// how many points in z
#define DZ	( ZSIDE/(float)NZ )	// change in z between the points




// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow( MainWindow );

	// create the sphere:

    SphereDL = glGenLists( 1 );
	glNewList( SphereDL, GL_COMPILE );
		OsuSphere(1., 300, 300);
	glEndList( );

	// ------- grid floor ---------- //

	GridDL = glGenLists(1);
	glNewList(GridDL, GL_COMPILE);
	SetMaterial(1.f, 1.f, 1.f, 1.f);
	glNormal3f(0., 1., 0.);
	for (int i = 0; i < NZ; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NX; j++)
		{
			glTexCoord2f((float)j / (float)NX, (float)i / (float)NZ);
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 0));

			glTexCoord2f((float)j / (float)NX, (float)(i + 1) / (float)NZ);
			glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 1));
		}
		glEnd();
	}
	glEndList();

	// pine tree
	PineTreeDL = glGenLists(1);
	glNewList(PineTreeDL, GL_COMPILE);
	LoadObjFile((char*)"pineTree.obj");
	glEndList();

	// camp fire
	CampFireDL = glGenLists(1);
	glNewList(CampFireDL, GL_COMPILE);
	LoadObjFile((char*)"campFire.obj");
	glEndList();

	// tent
	TentDL = glGenLists(1);
	glNewList(TentDL, GL_COMPILE);
	LoadObjFile((char*)"tent.obj");
	glEndList();


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'a':
		case 'A':
			eyeX -= eyeD;
			break;
		case 'd':
		case 'D':
			eyeX += eyeD;
			break;
		case 'w':
		case 'W':
			eyeZ -= eyeD;
			break;
		case 's':
		case 'S':
			eyeZ += eyeD;
			break;

		case 'f':
		case 'F':
			Frozen = !Frozen;
			if (Frozen)
				glutIdleFunc(NULL);
			else
				glutIdleFunc(Animate);
			break;

		case 'v':
		case 'V':
			Xrot = 0;
			Yrot = 0;
			Scale = 1;
			ViewToggle = !ViewToggle;
			break;

		case 'q':
		case 'Q':
			eyeY -= eyeD;
			break;
		case 'e':
		case 'E':
			eyeY += eyeD;
			break;

		case 'o':
		case 'O':
			NowProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			NowProjection = PERSP;
			break;

		case 't':
		case 'T':
			textureTesting = !textureTesting;
			break;

		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
