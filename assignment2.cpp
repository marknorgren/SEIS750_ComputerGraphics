/*
* SEIS750 Computer Graphics
* Assignment 2 
* Mark Norgren
* March 2012
**/

#include <GL/Angel.h>
#include <GL/CheckError.h>
#include <math.h>
#include "MRKCheckError.h"
#pragma comment(lib, "glew32.lib")

#define DEBUG 1

//store window width and height
int ww=1200, wh=800;

GLfloat tx, ty, tz;
// viewpoint camera lookat point
vec4 viewPointLookAtPoint = (0.0f, 0.0f, 0.0f, 1.0); //point
// lookat direction
vec4 viewPointLookAtDirection = (0.0f, 0.0f, 0.0f, 0.0); //vector

vec4 viewPointLookAt_At = (0.0f,0.0f,0.0f,0.0);
vec4 viewPointLookAt_Eye = (0.0f,0.0f,0.0f,0.0);

// Camera State Machine
enum state { 
	STATE_STATIC_CAMERA, 
	STATE_VIEWPOINT_CAMERA, 
	STATE_CHASE_CAMERA, 
	STATE_END 
};
enum state current_state = STATE_STATIC_CAMERA;


GLfloat rx, ry, rz;

//modelview and perspective matrices
mat4 mv, p;

//used for Dolly
GLfloat cameraPosition_Dolly = 90.0f;
//used for zooming
GLfloat cameraZoom_FOV = 90.0f;
// camera rotation
GLfloat cameraRotation = 0.0f;

// camera lookat point
bool lookAtCenterOfStage = true;
vec4 cameraLookAtPoint	= (0.0f, 0.0f, 0.0f, 1.0);
vec4 cameraLookAtEye	= vec4(0, 20, cameraPosition_Dolly, 1.0);
// camera zoom
#define DEFAULT_FOV 45.0
float fov = DEFAULT_FOV;
float fovStaticCameraSetting = DEFAULT_FOV;

enum LIGHTS
{
	HEADLIGHT_LEFT,
	HEADLIGHT_RIGHT,
	POLICE_LIGHT_RED,
	POLICE_LIGHT_GREEEN,
	NUMBER_OF_LIGHTS
};

vec4 lightPositions[NUMBER_OF_LIGHTS];
vec4 lightDirections[NUMBER_OF_LIGHTS];
vec4 lightIntensities[NUMBER_OF_LIGHTS]; // i.e. - light color
float cutoffAngles[NUMBER_OF_LIGHTS]; // RADIANS!!
float lightExponents[NUMBER_OF_LIGHTS];
vec4 lightColors[NUMBER_OF_LIGHTS];
vec4 ambientLights[NUMBER_OF_LIGHTS];

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint NormalMatrix;
GLuint vPosition;
GLuint vColor;

GLuint vAmbientDiffuseColor;
GLuint vSpecularColor;
GLuint vSpecularExponent;
GLuint vNormal;
GLuint vAmbient;
GLuint vDiffuse;
GLuint vSpecular;
GLuint ambient_light;

GLuint light_position;
GLuint light_color;
GLuint light_direction;
GLuint light_intensity;
GLuint light_exponent; 
GLuint light_cutoff; 

GLuint rightHeadlight_position;
GLuint rightHeadlight_color;
GLuint rightHeadlight_direction;
GLuint rightHeadlight_intensity;
GLuint rightHeadlight_exponent; 
GLuint rightHeadlight_cutoff; 

GLuint policeLight1_position;
GLuint policeLight1_direction;
GLuint policeLight1_intensity;
GLuint policeLight1_exponent;
GLuint policeLight1_cutoff; 
GLuint policeLight1_color;


GLuint Kd;
GLuint Ks;
GLuint Ka;
GLuint Shininess;

GLfloat carHeading = 0.0f;
GLfloat rotateHead = 0.0f;
bool drivingForward = false;
bool drivingBackward = false;
int wheelsTurned = 0;
GLfloat wheelRotation = 0.0f;

/** CAR OBJECT **/
#define CAR_WIDTH 2.0f
#define CAR_HEIGHT 2.0f
#define CAR_LENGTH 5.0f

#define STAGE_WIDTH 100.0f
#define STAGE_DEPTH 100.0f



enum VAO_OBJECTS
{
	CUBE,
	CAR,
	HEADLIGHT,
	POLICE_LIGHT,
	POLICE_LIGHT2,
	WHEEL,
	TIRETREAD,
	HUBCAP,
	HEAD,
	EYE,
	STAGE,
	NUMBER_OF_VAO_OBJECTS
};

enum VBO_OBJECTS
{
	CUBE_VERTS,
	CUBE_COLORS, //NOTE: THIS ORDER HAS TO STAY THE SAME FOR NOW VERTS - COLORS
	CUBE_NORMALS,
	CUBE_MATERIAL_AMBIENT,
	CUBE_MATERIAL_DIFFUSE,
	CUBE_SPECULAR,
	CUBE_SHININESS,
	CAR_VERTS,
	CAR_COLORS,
	CAR_NORMALS,
	CAR_MATERIAL_AMBIENT,
	CAR_MATERIAL_DIFFUSE,
	CAR_SPECULAR,
	CAR_SHININESS,
	HEADLIGHT_VERTS,
	HEADLIGHT_COLORS,
	HEADLIGHT_NORMALS,
	POLICE_LIGHT_VERTS,
	POLICE_LIGHT_COLORS,
	POLICE_LIGHT_NORMALS,
	POLICE_LIGHT_VERTS2,
	POLICE_LIGHT_COLORS2,
	POLICE_LIGHT_NORMALS2,
	WHEEL_VERTS,
	WHEEL_COLORS,
	WHEEL_NORMALS,
	TIRETREAD_VERTS,
	TIRETREAD_COLORS,
	TIRETREAD_NORMALS,
	HUBCAP_VERTS,
	HUBCAP_COLORS,
	HUBCAP_NORMALS,
	HEAD_VERTS,
	HEAD_COLORS,
	HEAD_NORMALS,
	EYE_VERTS,
	EYE_COLORS,
	EYE_NORMALS,
	STAGE_VERTS,
	STAGE_COLORS,
	STAGE_NORMALS,
	STAGE_MATERIAL_AMBIENT,
	STAGE_MATERIAL_DIFFUSE,
	STAGE_SPECULAR,
	STAGE_SHININESS,
	NUMBER_OF_VBO_OBJECTS
};

// LIGHTING
#define SHADER_ATTRIBS_PER_OBJECT 7


void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	switch(key)
	{
		/*************************************************** CAMERA POSITION STATE */
	case 'c':
		{
			switch (current_state)
			{
			case STATE_STATIC_CAMERA:
				current_state = STATE_VIEWPOINT_CAMERA;
				break;

			case STATE_VIEWPOINT_CAMERA:
				current_state = STATE_CHASE_CAMERA;
				break;
			case STATE_CHASE_CAMERA:
				current_state = STATE_STATIC_CAMERA;
				break;
			}
			printf("current_state: %d\n", current_state);
			break;
		}


		/*********************************************************** DOLLY CAMERA */
	case 'w': // DOLLY IN
		{
			printf("s pressed, cameraPosition_Dolly: %f\n", cameraPosition_Dolly);
			if (cameraPosition_Dolly < 200) 
			{
				printf("cameraPosition_Dolly < -25\n");
				cameraPosition_Dolly++;
			}
			break;
		}
	case 'q': // DOLLY OUT
		{
			printf("s pressed, cameraPosition_Dolly: %f\n", cameraPosition_Dolly);
			if (cameraPosition_Dolly > 5) {
				printf("cameraPosition_Dolly > -400\n");
				cameraPosition_Dolly--;
			}
			break;
		}
		/*********************************************************** DOLLY CAMERA */


		/*********************************************************** ZOOM CAMERA */
	case 's': // ZOOM OUT
		{
			if(current_state==STATE_STATIC_CAMERA)
			{
				if (fov<170) fov++;
				fovStaticCameraSetting = fov; //save settting
			}
			break;
		}
	case 'a': // ZOOM IN
		{
			if(current_state==STATE_STATIC_CAMERA)
			{
				if (fov>1) fov--;
				fovStaticCameraSetting = fov; //save settting
			}
			break;
		}
		//********************************************************** ZOOM CAMERA */


		/************************************************************************************** POINT CAMERA AT CAR/CENTER */
	case 'f':
		{
			//toggle look at
			lookAtCenterOfStage ? lookAtCenterOfStage=false : lookAtCenterOfStage=true;
			break;
		}



		/************************************************************************************** RESET CAMERA */
	case 'r':
		{
			// reset all Camera Settings
			fov = DEFAULT_FOV;
			fovStaticCameraSetting = DEFAULT_FOV;
			cameraZoom_FOV = 90.0f;
			cameraPosition_Dolly = 90.0f;
			lookAtCenterOfStage = true;
			cameraLookAtPoint	= (0.0f, 0.0f, 0.0f, 1.0);
			cameraLookAtEye	= vec4(0, 20, cameraPosition_Dolly, 1.0);
			current_state = STATE_STATIC_CAMERA;
			break;
		}



	case 'b':
		{
			if (cameraRotation < 360) cameraRotation++;
			else cameraRotation = 0;
			break;
		}
	case 'v':
		{
			if (cameraRotation > 0) cameraRotation--;
			else cameraRotation = 360;
			break;
		}
	case 'i':
		{
			rx += 5;
			if(rx > 360)
				rx -= 360;
			break;
		}
	case 'j':
		{
			rx -= 5;
			if(rx < 0)
				rx += 360;
			break;
		}
		/***** TURN CAR *****/
	case 'o':
		{
			ry += 5;
			if(ry > 360)
				ry -= 360;
			break;
		}
	case 'k':
		{
			ry -= 5;
			if(ry < 0)
				ry += 360;
			break;
		}
	case 'p':
		{
			rz += 5;
			if(rz > 360)
				rz -= 360;
			break;
		}
	case 'l':
		{
			rz -= 5;
			if(rz < 0)
				rz += 360;
		}
	case 'z':
		{

			if (rotateHead < 55)
				rotateHead += 5;
			break;
		}
	case 'x':
		{

			if (rotateHead > -55 )
				rotateHead -= 5;
			break;
		}
	case ' ':
		{
			drivingBackward = false;
			drivingForward = false;
			break;
		}

	} //switch end

	//don't forget to request a new frame since parameters have changed
	glutPostRedisplay();

}

void special(int key, int x, int y){
	if(key == GLUT_KEY_UP)
	{
		drivingForward = true;
		drivingBackward = false;
	}
	else if(key == GLUT_KEY_DOWN)
	{
		drivingBackward = true;
		drivingForward = false;
	}
	/***** TURN CAR *****/
	else if(key == GLUT_KEY_LEFT)
	{

		if (wheelsTurned < 5) wheelsTurned++;
		printf(" TURNING LEFT: wheelsTurned: %d\n", wheelsTurned);

	}
	/***** TURN CAR *****/ 
	else if(key == GLUT_KEY_RIGHT)
	{
		if (wheelsTurned > -5) wheelsTurned--;
		printf(" TURNING RIGHT: wheelsTurned: %d\n", wheelsTurned);

	}
	glutPostRedisplay();
}

//need some arrays to store cube attributes
GLuint vao[NUMBER_OF_VBO_OBJECTS];
GLuint vbo[NUMBER_OF_VBO_OBJECTS];

vec4 stageVerts[6];
vec4 stageColors[6];
vec3 stageNormals[6];
void generateStage() {
	for(int i=0; i<6; i++){
		stageColors[i] = vec4(0.5, 0.5, 0.5, 1.0); //front
		stageNormals[i] = vec3(0.0,1.0,0.0);
	}
	stageVerts[0] = vec4(-(STAGE_WIDTH/2.0f),	0.0f,	-(STAGE_DEPTH/2.0f),	1.0);
	stageVerts[1] = vec4(-(STAGE_WIDTH/2.0f),	0.0f,	(STAGE_DEPTH/2.0f),		1.0);
	stageVerts[2] = vec4((STAGE_WIDTH/2.0f),	0.0f,	(STAGE_DEPTH/2.0f),		1.0);
	stageVerts[3] = vec4((STAGE_WIDTH/2.0f),	0.0f,	(STAGE_DEPTH/2.0f),		1.0);
	stageVerts[4] = vec4((STAGE_WIDTH/2.0f),	0.0f,	-(STAGE_DEPTH/2.0f),	1.0);
	stageVerts[5] = vec4(-(STAGE_WIDTH/2.0f),	0.0f,	-(STAGE_DEPTH/2.0f),	1.0);
}

/** CAR OBJECT **/
vec4 carVerts[36];
vec4 carColors[36];
vec3 carNormals[36];
vec3 carMaterialAmbient[36];
vec3 carMaterialDiffuse[36];
vec3 carMaterialSpecular[36];
void generateCar() {
	for(int i=0; i<6; i++){
		carColors[i] = vec4(1.0, 1.0, 1.0, 1.0); //front

		carMaterialSpecular[i] = carMaterialDiffuse[i] = carMaterialAmbient[i] = 
			vec3(carColors[i].x,carColors[i].y,carColors[i].z);
		//carMaterialDiffuse[i] =	carColors[i].xyz;
		//carMaterialSpecular[i] = carColors[i].xyz;
		carNormals[i] = vec3(0.0,0.0,1.0);
	}
	carVerts[0] = vec4(CAR_WIDTH/2,		-(CAR_HEIGHT/2),	0.0f, 1.0);
	carVerts[1] = vec4(CAR_WIDTH/2,		CAR_HEIGHT/2,		0.0f, 1.0);
	carVerts[2] = vec4(-(CAR_WIDTH/2),	CAR_HEIGHT/2,		0.0f, 1.0);
	carVerts[3] = vec4(-(CAR_WIDTH/2),	CAR_HEIGHT/2,		0.0f, 1.0);
	carVerts[4] = vec4(-(CAR_WIDTH/2),	-(CAR_HEIGHT/2),	0.0f, 1.0);
	carVerts[5] = vec4(CAR_WIDTH/2,		-(CAR_HEIGHT/2),	0.0f, 1.0);

	for(int i=6; i<12; i++){
		carColors[i] = vec4(1.0, 1.0, 1.0, 1.0); //back
		carNormals[i] = vec3(0.0,0.0,-1.0);
	}
	//				X						Y					Z
	carVerts[6] = vec4(-(CAR_WIDTH/2),	-(CAR_HEIGHT/2),	-(CAR_LENGTH), 1.0);
	carVerts[7] = vec4(-(CAR_WIDTH/2),	CAR_HEIGHT/2,		-(CAR_LENGTH), 1.0);
	carVerts[8] = vec4(CAR_WIDTH/2,		CAR_HEIGHT/2,		-(CAR_LENGTH), 1.0);
	carVerts[9] = vec4(CAR_WIDTH/2,		CAR_HEIGHT/2,		-(CAR_LENGTH), 1.0);
	carVerts[10] = vec4(CAR_WIDTH/2,	-(CAR_HEIGHT/2),	-(CAR_LENGTH), 1.0);
	carVerts[11] = vec4(-(CAR_WIDTH/2), -(CAR_HEIGHT/2),	-(CAR_LENGTH), 1.0);

	for(int i=12; i<18; i++){
		carColors[i] = vec4(0.0, 0.0, 0.0, 1.0); //right
		carNormals[i] = vec3(1.0,0.0,0.0);
	}
	carVerts[12] = vec4(1.0f, 1.0f,		0.0f,			1.0);
	carVerts[13] = vec4(1.0f, -1.0f,	0.0f,			1.0);
	carVerts[14] = vec4(1.0f, -1.0f,	-(CAR_LENGTH),	1.0);
	carVerts[15] = vec4(1.0f, -1.0f,	-(CAR_LENGTH),	1.0);
	carVerts[16] = vec4(1.0f, 1.0f,		-(CAR_LENGTH),	1.0);
	carVerts[17] = vec4(1.0f, 1.0f,		0.0f,			1.0);

	for(int i=18; i<24; i++){
		carColors[i] = vec4(0.0, 0.0, 0.0, 1.0); //left
		carNormals[i] = vec3(-1.0,0.0,0.0);
	}
	carVerts[18] = vec4(-1.0f, 1.0f,	-(CAR_LENGTH),		1.0);
	carVerts[19] = vec4(-1.0f, -1.0f,	-(CAR_LENGTH),		1.0);
	carVerts[20] = vec4(-1.0f, -1.0f,	0.0f,				1.0);
	carVerts[21] = vec4(-1.0f, -1.0f,	0.0f,				1.0);
	carVerts[22] = vec4(-1.0f, 1.0f,	0.0f,				1.0);
	carVerts[23] = vec4(-1.0f, 1.0f,	-(CAR_LENGTH),		1.0);

	for(int i=24; i<30; i++){
		carColors[i] = vec4(0.0, 0.0, 0.0, 1.0); //top
		carNormals[i] = vec3(0.0,1.0,0.0);
	}
	carVerts[24] = vec4(1.0f,	1.0f, 0.0f, 1.0);
	carVerts[25] = vec4(1.0f,	1.0f, -(CAR_LENGTH), 1.0);
	carVerts[26] = vec4(-1.0f,	1.0f, -(CAR_LENGTH), 1.0);
	carVerts[27] = vec4(-1.0f,	1.0f, -(CAR_LENGTH), 1.0);
	carVerts[28] = vec4(-1.0f,	1.0f, 0.0f, 1.0);
	carVerts[29] = vec4(1.0f,	1.0f, 0.0f, 1.0);

	for(int i=30; i<36; i++){
		carColors[i] = vec4(0.0, 1.0, 0.0, 1.0); //bottom
		carNormals[i] = vec3(0.0,-1.0,0.0);
	}
	carVerts[30] = vec4(1.0f,	-1.0f, -5.0f, 1.0);
	carVerts[31] = vec4(1.0f,	-1.0f, 0.0f, 1.0);
	carVerts[32] = vec4(-1.0f,	-1.0f, 0.0f, 1.0);
	carVerts[33] = vec4(-1.0f,	-1.0f, 0.0f, 1.0);
	carVerts[34] = vec4(-1.0f,	-1.0f, -5.0f, 1.0);
	carVerts[35] = vec4(1.0f,	-1.0f, -5.0f, 1.0);
}

/** TRIANLE OBJECT **/
vec4 triangleVerts[3];
vec4 triangleColors[3];
void generateTriangle(){
	for(int i=0;i<3;i++){
		triangleColors[i] = vec4(1.0f,0.0f,0.0f,1.0);
	}
	triangleVerts[0] = vec4(-0.45f,-0.45f,0.0f,1.0);
	triangleVerts[1] = vec4(0.0f,0.45f,0.0f,1.0);
	triangleVerts[2] = vec4(0.45,-0.45f,0.0f,1.0);
}

/** POLICE LIGHT OBJECT **/
vec4 policeLightVerts[36];
vec4 policeLightColors[36];
vec3 policeLightNormals[36];
void generatePoliceLight(vec4 color){
	float policeLightSize = 0.3;
	vec4 policeLightColor = color;
	for(int i=0; i<6; i++){
		policeLightColors[i] = policeLightColor; //front
		policeLightNormals[i] = vec3(0.0,0.0,1.0);
	}
	policeLightVerts[0] = vec4(policeLightSize,		-policeLightSize,	policeLightSize, 1.0);
	policeLightVerts[1] = vec4(policeLightSize,		policeLightSize,	policeLightSize, 1.0);
	policeLightVerts[2] = vec4(-policeLightSize,	policeLightSize,	policeLightSize, 1.0);
	policeLightVerts[3] = vec4(-policeLightSize,	policeLightSize,	policeLightSize, 1.0);
	policeLightVerts[4] = vec4(-policeLightSize,	-policeLightSize,	policeLightSize, 1.0);
	policeLightVerts[5] = vec4(policeLightSize,		-policeLightSize,	policeLightSize, 1.0);


	for(int i=6; i<12; i++){
		policeLightColors[i] = policeLightColor; //back
		policeLightNormals[i] = vec3(0.0,0.0,-1.0);
	}
	policeLightVerts[6] = vec4(-policeLightSize, -policeLightSize,	-policeLightSize, 1.0);
	policeLightVerts[7] = vec4(-policeLightSize, policeLightSize,	-policeLightSize, 1.0);
	policeLightVerts[8] = vec4(policeLightSize, policeLightSize,	-policeLightSize, 1.0);
	policeLightVerts[9] = vec4(policeLightSize, policeLightSize,	-policeLightSize, 1.0);
	policeLightVerts[10] = vec4(policeLightSize, -policeLightSize,	-policeLightSize, 1.0);
	policeLightVerts[11] = vec4(-policeLightSize, -policeLightSize, -policeLightSize, 1.0);

	for(int i=12; i<18; i++){
		policeLightColors[i] = policeLightColor; //left
		policeLightNormals[i] = vec3(1.0,0.0,0.0);
	}
	policeLightVerts[12] = vec4(policeLightSize, policeLightSize, policeLightSize, 1.0);
	policeLightVerts[13] = vec4(policeLightSize, -policeLightSize, policeLightSize, 1.0);
	policeLightVerts[14] = vec4(policeLightSize, -policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[15] = vec4(policeLightSize, -policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[16] = vec4(policeLightSize, policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[17] = vec4(policeLightSize, policeLightSize, policeLightSize, 1.0);

	for(int i=18; i<24; i++){
		policeLightColors[i] = policeLightColor; //right
		policeLightNormals[i] = vec3(-1.0,0.0,0.0);
	}
	policeLightVerts[18] = vec4(-policeLightSize, policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[19] = vec4(-policeLightSize, -policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[20] = vec4(-policeLightSize, -policeLightSize, policeLightSize, 1.0);
	policeLightVerts[21] = vec4(-policeLightSize, -policeLightSize, policeLightSize, 1.0);
	policeLightVerts[22] = vec4(-policeLightSize, policeLightSize, policeLightSize, 1.0);
	policeLightVerts[23] = vec4(-policeLightSize, policeLightSize, -policeLightSize, 1.0);

	for(int i=24; i<30; i++){
		policeLightColors[i] = policeLightColor; //top
		policeLightNormals[i] = vec3(0.0,1.0,0.0);
	}
	policeLightVerts[24] = vec4(policeLightSize,	policeLightSize, policeLightSize, 1.0);
	policeLightVerts[25] = vec4(policeLightSize,	policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[26] = vec4(-policeLightSize,	policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[27] = vec4(-policeLightSize,	policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[28] = vec4(-policeLightSize,	policeLightSize, policeLightSize, 1.0);
	policeLightVerts[29] = vec4(policeLightSize,	policeLightSize, policeLightSize, 1.0);

	for(int i=30; i<36; i++){
		policeLightColors[i] = policeLightColor; //bottom
		policeLightNormals[i] = vec3(0.0,-1.0,0.0);
	}
	policeLightVerts[30] = vec4(policeLightSize,	-policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[31] = vec4(policeLightSize,	-policeLightSize, policeLightSize, 1.0);
	policeLightVerts[32] = vec4(-policeLightSize,	-policeLightSize, policeLightSize, 1.0);
	policeLightVerts[33] = vec4(-policeLightSize,	-policeLightSize, policeLightSize, 1.0);
	policeLightVerts[34] = vec4(-policeLightSize,	-policeLightSize, -policeLightSize, 1.0);
	policeLightVerts[35] = vec4(policeLightSize,	-policeLightSize, -policeLightSize, 1.0);
}

/** CUBE OBJECT **/
vec4 cubeVerts[36];
vec4 cubeColors[36];
vec3 cubeNormals[36];
void generateCube(){
	for(int i=0; i<6; i++){
		cubeColors[i] = vec4(0.0, 1.0, 1.0, 1.0); //front
		cubeNormals[i] = vec3(0.0,0.0,1.0);
	}
	cubeVerts[0] = vec4(1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[1] = vec4(1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[2] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[3] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[4] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[5] = vec4(1.0f, -1.0f, 1.0f, 1.0);


	for(int i=6; i<12; i++){
		cubeColors[i] = vec4(1.0, 0.0, 1.0, 1.0); //back
		cubeNormals[i] = vec3(0.0,0.0,-1.0);
	}
	cubeVerts[6] = vec4(-1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[7] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[8] = vec4(1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[9] = vec4(1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[10] = vec4(1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[11] = vec4(-1.0f, -1.0f, -1.0f, 1.0);

	for(int i=12; i<18; i++){
		cubeColors[i] = vec4(1.0, 1.0, 0.0, 1.0); //left
		cubeNormals[i] = vec3(1.0,0.0,0.0);
	}
	cubeVerts[12] = vec4(1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[13] = vec4(1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[14] = vec4(1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[15] = vec4(1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[16] = vec4(1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[17] = vec4(1.0f, 1.0f, 1.0f, 1.0);

	for(int i=18; i<24; i++){
		cubeColors[i] = vec4(1.0, 0.0, 0.0, 1.0); //right
		cubeNormals[i] = vec3(-1.0,0.0,0.0);
	}
	cubeVerts[18] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[19] = vec4(-1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[20] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[21] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[22] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[23] = vec4(-1.0f, 1.0f, -1.0f, 1.0);

	for(int i=24; i<30; i++){
		cubeColors[i] = vec4(0.0, 0.0, 1.0, 1.0); //top
		cubeNormals[i] = vec3(0.0,1.0,0.0);
	}
	cubeVerts[24] = vec4(1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[25] = vec4(1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[26] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[27] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
	cubeVerts[28] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
	cubeVerts[29] = vec4(1.0f, 1.0f, 1.0f, 1.0);

	for(int i=30; i<36; i++){
		cubeColors[i] = vec4(0.0, 1.0, 0.0, 1.0); //bottom
		cubeNormals[i] = vec3(0.0,-1.0,0.0);
	}
	cubeVerts[30] = vec4(1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[31] = vec4(1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[32] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[33] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
	cubeVerts[34] = vec4(-1.0f, -1.0f, -1.0f, 1.0);
	cubeVerts[35] = vec4(1.0f, -1.0f, -1.0f, 1.0);
}

/*****************************************************/
/* HEAD,EYES - SPHERE OBJECT */
/*****************************************************/
int spherevertcount;
vec4* sphere_verts;
vec4* sphere_colors;
vec3* sphere_normals;
int generateSphere(float radius, int subdiv, vec4 color){
	float step = (360.0/subdiv)*(M_PI/180.0);

	int totalverts = ceil(subdiv/2.0)*subdiv * 6;

	if(sphere_verts){
		delete[] sphere_verts;
	}
	sphere_verts = new vec4[totalverts];
	sphere_colors = new vec4[totalverts];
	sphere_normals = new vec3[totalverts];
	for(int i=0; i<totalverts; i++){
		sphere_colors[i] = color; //white
	}

	int k = 0;
	for(float i = -M_PI/2; i<=M_PI/2; i+=step){
		for(float j = -M_PI; j<=M_PI; j+=step){
			//triangle 1
			sphere_verts[k]		=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			sphere_normals[k]	=	vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			k++;

			sphere_verts[k]		=   vec4(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step), 1.0);
			sphere_normals[k]	=	vec3(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step));
			k++;

			sphere_verts[k]		=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			sphere_normals[k]	=	vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			k++;

			//triangle 2
			sphere_verts[k]		=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			sphere_normals[k]	=	vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			k++;

			sphere_verts[k]		=   vec4(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i), 1.0);
			sphere_normals[k]	=	vec3(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i));
			k++;

			sphere_verts[k]		=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			sphere_normals[k]	=	vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			k++;
		}
	}
	return totalverts;
}

int headlightvertcount;
vec4* headlight_verts;
vec4* headlight_colors;
vec3* headlight_normals;
int generateHeadlight(float radius, int subdiv, vec4 color){
	float step = (360.0/subdiv)*(M_PI/180.0);
	printf("step: %f\n", step);

	int totalverts = 360+2;//ceil(subdiv/2.0)*subdiv ;
	printf("totalVerts: %d\n", totalverts);
	if(headlight_verts){
		delete[] headlight_verts;
	}

	headlight_verts = new vec4[totalverts];
	headlight_colors = new vec4[totalverts];
	headlight_normals = new vec3[totalverts];
	/* COLOR, NORMALS */
	for(int i=0; i<=totalverts; i++){
		headlight_colors[i] = color; //white
		headlight_normals[i] = vec3(0.0,0.0,1.0);
	}

	/* POSITION VERTICES */
	float a=0, x=0.0,y=0.0,z=0.0;
	// initial point at origin
	headlight_verts[0] = vec4(0.0f,0.0f,0.0f,1.0);
	int vert_count = 1;
	vec4 firstOne;
	for (int i=0;i<360;i++)
	{
		float angle = i * 2 * M_PI/360;
		x = cos(angle) * radius;
		y = sin(angle) * radius;
		z = 0.0f;
		headlight_verts[vert_count] = vec4(x, y, z, 1.0);
		if (i==0) 
		{
			firstOne = vec4(x, y, z, 1.0);
			printf("firstOne: %f,%f,%f\n", x, y, z);
		}
		vert_count++;
	}

	headlight_verts[vert_count] = firstOne;
	vert_count++;
	headlight_verts[vert_count] = vec4(0.0f,0.0f,0.0f,1.0);
#define DEBUG 1
#if DEBUG
	for (int i=0;i<=totalverts;i++)
	{
		printf("headlight-index,%d,x,%f,y,%f,z,%f\n", i, headlight_verts[i][0], headlight_verts[i][1], headlight_verts[i][2]); 
		printf("  headlightNormal-index,%d,x,%f,y,%f,z,%f\n", i, headlight_normals[i][0], headlight_normals[i][1], headlight_normals[i][2]);
	}
#endif

	return totalverts;
}

int circlevertcount;
vec4* circle_verts;
vec4* circle_colors;
vec3* circle_normals;
int generateCircle(float radius, int subdiv, vec4 color){
	float step = (360.0/subdiv)*(M_PI/180.0);
	printf("step: %f\n", step);

	int totalverts = 360+2;//ceil(subdiv/2.0)*subdiv ;

	/* COLOR */
	circle_verts = new vec4[totalverts];
	circle_colors = new vec4[totalverts];
	circle_normals = new vec3[totalverts];
	for(int i=0; i<=totalverts; i++){
		circle_colors[i] = color; //white
		circle_normals[i] = vec3(0.0,0.0,1.0);
	}

	/* POSITION VERTICES */
	float a=0, x=0.0,y=0.0,z=0.0;
	// initial point at origin
	circle_verts[0] = vec4(0.0f,0.0f,0.0f,1.0);
	int vert_count = 1;
	vec4 firstOne;
	for (int i=0;i<360;i++)
	{
		float angle = i * 2 * M_PI/360;
		x = cos(angle) * radius;
		y = sin(angle) * radius;
		z = 0.0f;
		circle_verts[vert_count] = vec4(x, y, z, 1.0);
		if (i==0) 
		{
			firstOne = vec4(x, y, z, 1.0);
			printf("firstOne: %f,%f,%f\n", x, y, z);
		}
		vert_count++;
	}

	circle_verts[vert_count] = firstOne;
	vert_count++;
	circle_verts[vert_count] = vec4(0.0f,0.0f,0.0f,1.0);

#if DEBUG2
	for (int i=0;i<=totalverts;i++)
	{
		printf("Circle-index,%d,x,%f,y,%f,z,%f\n", i, circle_verts[i][0], circle_verts[i][1], circle_verts[i][2]); 
	}
#endif

	return totalverts;
}


int tireTreadvertcount;
vec4* tireTread_verts;
vec4* tireTread_colors;
vec3* tireTread_normals;
int generateTireTread(float radius, int subdiv, vec4 color){
	float step = (360.0/subdiv)*(M_PI/180.0);
	printf("step: %f\n", step);

	int totalverts = (360 * 2)+2; //two circles, connecting with triangle fan
	printf("totalVerts: %d\n", totalverts);
	if(tireTread_verts){
		delete[] tireTread_verts;
	}

	/* COLOR */
	tireTread_verts = new vec4[totalverts];
	tireTread_colors = new vec4[totalverts];
	tireTread_normals = new vec3[totalverts];
	for(int i=0; i<totalverts; i++){
		tireTread_colors[i] = color;
	}

	/* POSITION VERTICES */
	float a=0, x=0.0,y=0.0,z=0.0;
	// initial point at origin
	//tireTread_verts[0] = vec4(0.0f,0.0f,0.0f,1.0);
	int vert_count = 0;
	int i=0;
	int angleIndex = 0;
	vec4 firstOne;
	vec4 firstOneOtherHalf;
	for (i=0;i<360;i++)
	{
		float angle = angleIndex * 2 * M_PI/360;
		angleIndex++;
		x = cos(angle) * radius;
		y = sin(angle) * radius;
		z = 0.0f;

		tireTread_verts[vert_count] = vec4(x, y, z, 1.0);
		tireTread_normals[vert_count] = vec3(x,y,z);
		if (i==0) firstOne = vec4(x, y, z, 1.0);
		z = 1.0f;
		vert_count++;
		tireTread_verts[vert_count] = vec4(x, y, z, 1.0);
		tireTread_normals[vert_count] = vec3(x,y,z);
		if (i==0) firstOneOtherHalf = vec4(x, y, z, 1.0);
		vert_count++;
	}

	/* finish up triangle strips with firsts */
	tireTread_verts[vert_count] = firstOne;
	// why doesn't vec4::xyz work here???
	tireTread_normals[vert_count] = vec3(firstOne.x, firstOne.y, firstOne.z);
	vert_count++;
	tireTread_verts[vert_count] = firstOneOtherHalf;
	// why doesn't vec4::xyz work here???
	tireTread_normals[vert_count] = vec3(firstOneOtherHalf.x, firstOneOtherHalf.y, firstOneOtherHalf.z);
	vert_count++;
	tireTread_verts[vert_count] = firstOne;
	tireTread_normals[vert_count] = vec3(firstOne.x, firstOne.y, firstOne.z);

#if DEBUG2
	for (int i=0;i<=totalverts;i++)
	{
		printf("index,%d,x,%f,y,%f,z,%f\n", i, tireTread_verts[i][0], tireTread_verts[i][1], tireTread_verts[i][2]); 
	}
#endif

	return totalverts;
}

#define CAR_SPEED 0.1f
#define VIEWPOINT_LOOK_AT_DISTANCE 12.0f
#define WHEEL_TURN_FACTOR 0.1f
void my_timer (int v)
{
	GLfloat txBeforeMove = 0.0f;
	GLfloat tzBeforeMove = 0.0f;


	if (drivingForward) {
		// handle rotating wheels forward
		wheelRotation = wheelRotation += CAR_SPEED;
		if (wheelRotation>360) wheelRotation-=360;

		if (wheelsTurned > 0) // TURNING LEFT
		{
			ry += (0.5 + (wheelsTurned*WHEEL_TURN_FACTOR));
			carHeading = (ry*M_PI)/180;
			if(ry > 360)
			{
				ry -= 360;
			}
		}
		if (wheelsTurned < 0) // TURNING RIGHT
		{
			ry -= (0.5 - (wheelsTurned*WHEEL_TURN_FACTOR));
			carHeading = (ry*M_PI)/180;
			if(ry < 0)
			{
				ry += 360;
			}
		}

		// save current position
		txBeforeMove = tx;
		tzBeforeMove = tz;
		tz += CAR_SPEED * cos(carHeading);
		tx += CAR_SPEED * sin(carHeading);
		//printf("carHeading - 1: %f\n", carHeading);
		// if next position puts car outside of stage area rollback update, stop moving
		if (	(tx < -((STAGE_WIDTH-5)/2) || tx > ((STAGE_WIDTH-5)/2))
			|| // or
			(tz < -((STAGE_DEPTH-5)/2) || tz > ((STAGE_DEPTH-5)/2))
			) 
		{
			tx = txBeforeMove;
			tz = tzBeforeMove;
			drivingForward = false;
		}
	}

	if (drivingBackward) {
		// handle rotating wheels backward
		wheelRotation = wheelRotation -= CAR_SPEED;
		if (wheelRotation<0) wheelRotation+=360;
		if (wheelsTurned > 0) // WHEELS TURNED LEFT
		{	
			ry -= (0.5 + (wheelsTurned*WHEEL_TURN_FACTOR));
			carHeading = (ry*M_PI)/180;
			if(ry > 360)
			{
				ry -= 360;
			}
		}
		if (wheelsTurned < 0) // WHEELS TURNED RIGHT
		{
			ry += (0.5 - (wheelsTurned*WHEEL_TURN_FACTOR));
			carHeading = (ry*M_PI)/180;
			if(ry < 0)
			{
				ry += 360;
			}
		}
		// save current position
		txBeforeMove = tx;
		tzBeforeMove = tz;
		tz -= CAR_SPEED * cos(carHeading);
		tx -= CAR_SPEED * sin(carHeading);
		if (	
			(tx < -((STAGE_WIDTH-5)/2) || tx > ((STAGE_WIDTH-5)/2))
			||
			(tz < -((STAGE_DEPTH-5)/2) || tz > ((STAGE_DEPTH-5)/2))
			) // if next position puts car outside of stage area rollback update, stop moving
		{
			tx = txBeforeMove;
			tz = tzBeforeMove;
			drivingBackward = false;
		}
	}

	float eyeX = 0.0f;
	float eyeZ = 0.0f;
	float atX = 0.0f;
	float atZ = 0.0f;
	//printf("carHeading - 2: %f\n", carHeading);
	switch (current_state)
	{
	case STATE_STATIC_CAMERA:
		//set fov to last setting
		fov = fovStaticCameraSetting;
		// look at car/center of stage
		if (!lookAtCenterOfStage) 
			cameraLookAtPoint = vec4(tx, ty, tz, 1.0);
		else 
			cameraLookAtPoint = vec4(0.0f, 0.0f, 0.0f, 1.0);
		cameraLookAtEye	= vec4(0, 20, cameraPosition_Dolly, 1.0);
		break;

	case STATE_VIEWPOINT_CAMERA:
		fov = DEFAULT_FOV;
		eyeX = tx + (0.5 * sin(carHeading));
		eyeZ = tz + (0.5 * cos(carHeading));
		viewPointLookAt_Eye = vec4(eyeX,ty+5,eyeZ, 1.0);
		atX = tx + (20 * sin(carHeading));
		atZ = tz + (20 * cos(carHeading));
		viewPointLookAt_At = RotateY(rotateHead) * vec4(atX, ty, atZ, 1.0);
		//printf("cameraLookAtEye: %f,%f,%f\n", cameraLookAtEye.x, cameraLookAtEye.y, cameraLookAtEye.z);
		//printf("cameraLookAtPoint: %f,%f,%f\n", cameraLookAtPoint.x, cameraLookAtPoint.y, cameraLookAtPoint.z);
		//printf("tx:%f,ty:%f,tz:%f\n", tx,ty,tz);
		break;
	case STATE_CHASE_CAMERA:
		fov = DEFAULT_FOV;
		eyeX = tx - (20 * sin(carHeading));
		eyeZ = tz - (20 * cos(carHeading));
		viewPointLookAt_Eye = vec4(eyeX,ty+15,eyeZ, 1.0);
		atX = tx + (10 * sin(carHeading));
		atZ = tz + (10 * cos(carHeading));
		viewPointLookAt_At = vec4(atX, ty, atZ, 1.0);
		//printf("viewPointLookAt_Eye: %f,%f,%f\n", viewPointLookAt_Eye.x, viewPointLookAt_Eye.y, viewPointLookAt_Eye.z);
		//printf("viewPointLookAt_At: %f,%f,%f\n", viewPointLookAt_At.x, viewPointLookAt_At.y, viewPointLookAt_At.z);
		//printf("tx:%f,ty:%f,tz:%f\n", tx,ty,tz);
		break;
	}
	glutPostRedisplay();
	glutTimerFunc(1000/v, my_timer, v);
}

void setupShader(GLuint prog){

	glUseProgram( prog );
	//glLinkProgram( prog);
	model_view = glGetUniformLocation(prog, "model_view");
	projection = glGetUniformLocation(prog, "projection");

	vAmbientDiffuseColor = glGetAttribLocation(prog, "vAmbientDiffuseColor");
	vSpecularColor = glGetAttribLocation(prog, "vSpecularColor");
	vSpecularExponent = glGetAttribLocation(prog, "vSpecularExponent");
	light_position = glGetUniformLocation(prog, "light_position");
	light_direction = glGetUniformLocation(prog, "light_direction");
	light_intensity = glGetUniformLocation(prog, "light_intensity");
	light_exponent = glGetUniformLocation(prog, "light_exponent");
	light_cutoff = glGetUniformLocation(prog, "light_cutoff");
	light_color = glGetUniformLocation(prog, "light_color");
	ambient_light = glGetUniformLocation(prog, "ambient_light");

	
	rightHeadlight_position = glGetUniformLocation(prog, "rightHeadlight_position");
	rightHeadlight_direction = glGetUniformLocation(prog, "rightHeadlight_direction");
	rightHeadlight_intensity = glGetUniformLocation(prog, "rightHeadlight_intensity");
	rightHeadlight_exponent = glGetUniformLocation(prog, "rightHeadlight_exponent");
	rightHeadlight_cutoff = glGetUniformLocation(prog, "rightHeadlight_cutoff");
	rightHeadlight_color = glGetUniformLocation(prog, "rightHeadlight_color");

	policeLight1_position = glGetUniformLocation(prog, "policeLight1_position");
	policeLight1_direction = glGetUniformLocation(prog, "policeLight1_direction");
	policeLight1_intensity = glGetUniformLocation(prog, "policeLight1_intensity");
	policeLight1_exponent = glGetUniformLocation(prog, "policeLight1_exponent");
	policeLight1_cutoff = glGetUniformLocation(prog, "policeLight1_cutoff");
	policeLight1_color = glGetUniformLocation(prog, "policeLight1_color");


	//Uniforms - Kd, Ks, Ka, Shininess
	Kd = glGetUniformLocation(prog, "Kd");
	Ks = glGetUniformLocation(prog, "Ks");
	Ka = glGetUniformLocation(prog, "Ka");

	


	glBindVertexArray( vao[0] );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	vNormal = glGetAttribLocation(prog, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void init() {
	/*select clearing (background) color*/
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//populate our arrays
	generateStage();

	generateCube();
	generateCar();
	//set up transformation defaults
	//start with no translation or rotation
	tx = ty = tz = rx = ry = rz = 0;

	// Load shaders and use the resulting shader program
	GLuint program = InitShader( "vshader-transform.glsl", "fshader-transform.glsl" );
	glUseProgram( program );
	setupShader(program);

	// Create a vertex array object
	glGenVertexArrays( NUMBER_OF_VAO_OBJECTS, vao );

	// CUBE
	// Create and initialize any buffer objects
	glBindVertexArray( vao[CUBE] );
	glGenBuffers( SHADER_ATTRIBS_PER_OBJECT, &vbo[CUBE_VERTS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CUBE_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CUBE_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//vertex normals
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CUBE_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);


	/*********************************************************
	* POLICE LIGHT 1
	*
	*********************************************************/
	// Create and initialize any buffer objects
	generatePoliceLight(vec4(0.0f,0.0f,1.0f,1.0));
	glBindVertexArray( vao[POLICE_LIGHT] );
	glGenBuffers( SHADER_ATTRIBS_PER_OBJECT, &vbo[POLICE_LIGHT_VERTS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[POLICE_LIGHT_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(policeLightVerts), policeLightVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[POLICE_LIGHT_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(policeLightColors), policeLightColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//normal for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[POLICE_LIGHT_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(policeLightNormals), policeLightNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*********************************************************
	* POLICE LIGHT 2
	*
	*********************************************************/
	// Create and initialize any buffer objects
	generatePoliceLight(vec4(1.0f,0.0f,0.0f,1.0));
	glBindVertexArray( vao[POLICE_LIGHT2] );
	glGenBuffers( SHADER_ATTRIBS_PER_OBJECT, &vbo[POLICE_LIGHT_VERTS2] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[POLICE_LIGHT_VERTS2] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(policeLightVerts), policeLightVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[POLICE_LIGHT_COLORS2] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(policeLightColors), policeLightColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//normal for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[POLICE_LIGHT_NORMALS2] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(policeLightNormals), policeLightNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*********************************************************
	* CAR
	*
	*********************************************************/
	// Create and initialize **CAR** buffer objects
	// Create a vertex array object
	//glGenVertexArrays( 1, &vao[1] );
	glBindVertexArray( vao[CAR] );
	glGenBuffers( SHADER_ATTRIBS_PER_OBJECT, &vbo[CAR_VERTS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CAR_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carVerts), carVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CAR_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carColors), carColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);


	//normal for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CAR_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carNormals), carNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//material attributes for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[CUBE_MATERIAL_AMBIENT] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(carMaterialAmbient), carMaterialAmbient, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vAmbient");
	glEnableVertexAttribArray(vAmbient);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//grab pointers for our modelview and perspecive uniform matrices
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	/*********************************************************
	/** HEAD 
	/*********************************************************/
	// generate vertices for head 
	spherevertcount = generateSphere(0.8, 30, vec4(1.0, 0.5, 0.5, 1.0));

	glBindVertexArray( vao[HEAD] );
	glGenBuffers( SHADER_ATTRIBS_PER_OBJECT, &vbo[HEAD_VERTS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[HEAD_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[HEAD_COLORS] );
	glBufferData( GL_ARRAY_BUFFER,spherevertcount*sizeof(vec4), sphere_colors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//normals for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[HEAD_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER,spherevertcount*sizeof(vec3), sphere_normals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*********************************************************
	/** EYE 
	/*********************************************************/
	// generate vertices for head 
	spherevertcount = generateSphere(0.1, 30, vec4(0.0, 0.0, 1.0, 1.0));

	glBindVertexArray( vao[EYE] );
	glGenBuffers( SHADER_ATTRIBS_PER_OBJECT, &vbo[EYE_VERTS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[EYE_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[EYE_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec3), sphere_colors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/*********************************************************
	/** HEADLIGHT 
	/*********************************************************/

	// generate vertices 
	headlightvertcount = generateHeadlight(0.4, 30, vec4(0.95, 0.95, 0.0, 1.0));

	glBindVertexArray( vao[HEADLIGHT] );
	glGenBuffers( SHADER_ATTRIBS_PER_OBJECT, &vbo[HEADLIGHT_VERTS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[HEADLIGHT_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, headlightvertcount*sizeof(vec4), headlight_verts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[HEADLIGHT_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, headlightvertcount*sizeof(vec4), headlight_colors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//normals for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[HEADLIGHT_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER,headlightvertcount*sizeof(vec3), headlight_normals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);


	/*********************************************************
	/** WHEEL 
	/*********************************************************/
	// generate vertices 
	circlevertcount = generateCircle(1.0, 30, vec4(1.0, 1.0, 1.0, 1.0));

	glBindVertexArray( vao[WHEEL] );
	glGenBuffers( SHADER_ATTRIBS_PER_OBJECT, &vbo[WHEEL_VERTS] );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[WHEEL_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, circlevertcount*sizeof(vec4), circle_verts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[WHEEL_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, circlevertcount*sizeof(vec4), circle_colors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//normals for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[WHEEL_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER,circlevertcount*sizeof(vec3), circle_normals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*********************************************************
	/** TIRE TREAD 
	/*********************************************************/
	// generate vertices 
	tireTreadvertcount = generateTireTread(1.0, 30, vec4(0.0, 0.0, 0.0, 1.0));

	glBindVertexArray( vao[TIRETREAD] );
	glGenBuffers( SHADER_ATTRIBS_PER_OBJECT, &vbo[TIRETREAD_VERTS] );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[TIRETREAD_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, tireTreadvertcount*sizeof(vec4), tireTread_verts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[TIRETREAD_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, tireTreadvertcount*sizeof(vec4), tireTread_colors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//normals for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[TIRETREAD_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER,tireTreadvertcount*sizeof(vec3), tireTread_normals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*********************************************************
	/** WHEEL HUBCAP 
	/*********************************************************/
	// generate vertices
	generateTriangle();

	glBindVertexArray( vao[HUBCAP] );
	glGenBuffers( 2, &vbo[HUBCAP_VERTS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[HUBCAP_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, 3*sizeof(vec4), triangleVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[HUBCAP_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, 3*sizeof(vec4), triangleColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/*********************************************************
	* STAGE
	*
	*********************************************************/
	// Create a vertex array object

	glBindVertexArray( vao[STAGE] );
	glGenBuffers( 3, &vbo[STAGE_VERTS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[STAGE_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageVerts), stageVerts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[STAGE_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageColors), stageColors, GL_STATIC_DRAW );
	vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//normals for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[STAGE_NORMALS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(stageNormals), stageNormals, GL_STATIC_DRAW );
	vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//grab pointers for our modelview and perspecive uniform matrices
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");
	NormalMatrix = glGetUniformLocation(program, "NormalMatrix");

	//Only draw the things in the front layer
	glEnable(GL_DEPTH_TEST);
}

float policeLightRotation = 0.0;
void display(void)
{
	/*clear all pixels*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 cameraMatrix;
	mat4 wholeCarMatrix;
	mat4 headlightMatrix;
	mat4 policeLightMatrix;
	mat4 headMatrix;
	mat4 frontWheelsMatrix;
	mat4 allWheelsMatrix;
	mat4 inner;

	vec4 light_direction_vector; 
	vec4 light_direction_vector_mv;
	vec4 light_position_headlight;

	glVertexAttrib4fv(vAmbientDiffuseColor, vec4(.3, .3, .3, 1));
	glVertexAttrib4fv(vSpecularColor, vec4(1.0f,1.0f,1.0f,1.0f));
	glVertexAttrib1f(vSpecularExponent, 10.0);

	// NormalMatrix
	//glUniform3fv(NormalMatrix, 1, mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

	// glUniform3fv(location, count, value)
	glUniform3fv(Ka, 1, vec3(0.2f,0.0f,0.0f));
	glUniform3fv(Ks, 1, vec3(0.2f,0.2f,0.2f));
	glUniform3fv(Kd, 1, vec3(0.1f,0.1f,0.1f));

	//spotlight
	glUniform3fv(light_intensity, 1, vec3(0.9f,0.9f,0.9f));
	glUniform1f(light_exponent, 30.0f);
	glUniform1f(light_cutoff, 15.0f);

	// glUniform1(loc,val)
	glUniform1f(Shininess, 180.0f);

	glUniform4fv(light_color, 1, vec4(1.0,1.0,1.0,1));
	//spotlight
	glUniform3fv(light_intensity, 1, vec3(0.9f,0.9f,0.9f));
	glUniform1f(light_exponent, 30.0f);
	glUniform1f(light_cutoff, 15.0f);
	glUniform4fv(ambient_light, 1, vec4(0.2, 0.2, 0.2, 5));
	glUniform4fv(rightHeadlight_color, 1, vec4(.4,.4,.4,1));


	switch (current_state)
	{
	case STATE_STATIC_CAMERA:
		// look at car/center of stage
		if (!lookAtCenterOfStage) 
			cameraLookAtPoint = vec4(tx, ty, tz, 1.0);
		else 
			cameraLookAtPoint = vec4(0.0f, 0.0f, 0.0f, 1.0);

		cameraLookAtEye	= vec4(0, 20, cameraPosition_Dolly, 1.0);
		break;
	case STATE_VIEWPOINT_CAMERA:
		cameraLookAtEye = viewPointLookAt_Eye;
		cameraLookAtPoint = viewPointLookAt_At;
		printf("cameraLookAtEye: %f,%f,%f\n", cameraLookAtEye.x, cameraLookAtEye.y, cameraLookAtEye.z);
		printf("cameraLookAtPoint: %f,%f,%f\n", cameraLookAtPoint.x, cameraLookAtPoint.y, cameraLookAtPoint.z);
		break;
	case STATE_CHASE_CAMERA:
		cameraLookAtEye = viewPointLookAt_Eye;
		cameraLookAtPoint = viewPointLookAt_At;
		printf("cameraLookAtEye: %f,%f,%f\n", cameraLookAtEye.x, cameraLookAtEye.y, cameraLookAtEye.z);
		printf("cameraLookAtPoint: %f,%f,%f\n", cameraLookAtPoint.x, cameraLookAtPoint.y, cameraLookAtPoint.z);
		break;
	}
	// LookAt(eye				at					up
	cameraMatrix = LookAt(cameraLookAtEye, cameraLookAtPoint, vec4(0, 1, 0, 0.0));
	cameraMatrix = cameraMatrix * RotateY(cameraRotation);
	wholeCarMatrix = cameraMatrix * Translate(tx, ty, tz);

	mv = cameraMatrix;// * mv;
	mv = mv * RotateX(rx);
	mv = mv * RotateY(ry);
	mv = mv * RotateZ(rz); 


	/* CORNER CUBES */
	// glUniform3fv(location, count, value)
	glUniform3fv(Ka, 1, vec3(1.0,0.2,0.0));
	glUniform3fv(Ks, 1, vec3(1.0f,0.2f,0.0));
	glUniform3fv(Kd, 1, vec3(1.0,0.2,0.0));

	/* cube 1 */
	mat4 cubeView = cameraMatrix;
	cubeView = cubeView * Translate(48, 1, 48);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, cubeView);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glBindVertexArray( vao[0] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube
	/* cube 2 */
	cubeView = cameraMatrix;
	cubeView = cubeView * Translate(-48, 1, -48);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, cubeView);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glBindVertexArray( vao[0] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube
	/* cube 3 */
	cubeView = cameraMatrix;
	cubeView = cubeView * Translate(48, 1, -48);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, cubeView);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glBindVertexArray( vao[0] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube
	/* cube 4 */
	cubeView = cameraMatrix;
	cubeView = cubeView * Translate(-48, 1, 48);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, cubeView);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glBindVertexArray( vao[0] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube


	for(int cubeIndex=0;cubeIndex<3;cubeIndex++)
	{
		cubeView = cameraMatrix;
		cubeView = cubeView * Translate(0, 2, (cubeIndex*15)+10);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, cubeView);
		glUniformMatrix4fv(projection, 1, GL_TRUE, p);
		glBindVertexArray( vao[0] );
		glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube

		cubeView = cameraMatrix;
		cubeView = cubeView * Translate(0, 2, (-(cubeIndex*15))-10);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, cubeView);
		glUniformMatrix4fv(projection, 1, GL_TRUE, p);
		glBindVertexArray( vao[0] );
		glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube
	}

	/* draw car */
	mv = cameraMatrix;
	wholeCarMatrix = mv;
	wholeCarMatrix = wholeCarMatrix * Translate(tx, ty, tz);
	wholeCarMatrix = wholeCarMatrix * RotateX(rx);
	wholeCarMatrix = wholeCarMatrix * RotateY(ry);
	wholeCarMatrix = wholeCarMatrix * RotateZ(rz);
	// move the car up for the wheels
	wholeCarMatrix = wholeCarMatrix * Translate(0, 2.0, 0);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, wholeCarMatrix);
	//glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glUniform3fv(Ka, 1, vec3(0.0,0.0,0.0));
	glUniform3fv(Ks, 1, vec3(0.0,0.0,0.0));
	glUniform3fv(Kd, 1, vec3(0.0,0.0,0.0));
	glBindVertexArray( vao[CAR] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the car

	/* light source */
	glUniform4fv(light_position, 1, mv*wholeCarMatrix);

	/* police light */
	policeLightMatrix = wholeCarMatrix;
	mat4 policeLightRotationMatrix = policeLightMatrix;
	
	if (policeLightRotation < 360) policeLightRotation+=5.0;
	else policeLightRotation = 0;
	policeLightRotationMatrix = policeLightRotationMatrix * RotateY(policeLightRotation);
	policeLightMatrix = policeLightMatrix * Translate(0.6, 1.4, -4.7);
	policeLightRotationMatrix = policeLightRotationMatrix * policeLightMatrix;
	glUniformMatrix4fv(model_view, 1, GL_TRUE, policeLightMatrix);

	// light direction = negative z direction, slight negative y
	light_direction_vector = vec4(0.0, -0.8, -1.0f, 0.0);
	
	
	//light_direction_vector = light_direction_vector * RotateY(1.0);
	// headlightMatrix applied to light_direction_vector
	light_direction_vector_mv = policeLightRotationMatrix * light_direction_vector;
	// set light_position to the origin of the headlightMatrix
	vec4 light_position_point = vec4(0.0,0.0,0.0,1.0);
	// get the light position translated - headlightMatrix coordinates
	light_position_point =	policeLightMatrix * light_position_point;
	
	
	glUniform4fv(policeLight1_position,		1,	light_position_point);
	glUniform3fv(policeLight1_direction,	1,	light_direction_vector_mv);
	glUniform4fv(policeLight1_color,		1,	vec4(1.0,1.0,1.0,1));
	glUniform3fv(policeLight1_intensity,	1,	vec3(1.0,0.0,0.0));
	




	glBindVertexArray( vao[POLICE_LIGHT] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the police light
	/* police light 2*/
	policeLightMatrix = wholeCarMatrix;
	policeLightMatrix = policeLightMatrix * Translate(-0.6, 1.4, -4.7);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, policeLightMatrix);
	glBindVertexArray( vao[POLICE_LIGHT2] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the police light

	/**************************************************************************** HEADLIGHTS */
	glUniform3fv(Ka, 1, vec3(1.0f,1.0f,0.0f));
	glUniform3fv(Ks, 1, vec3(0.0f,0.0f,0.0f));
	glUniform3fv(Kd, 1, vec3(0.1f,0.1f,0.1f));
	/* left headlight */
	headlightMatrix = wholeCarMatrix * Translate(0.7,0.4,0.01);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, headlightMatrix);
	// light direction = negative z direction, slight negative y
	light_direction_vector = vec4(-0.05, -0.19, 0.5f, 0.0);
	// headlightMatrix applied to light_direction_vector
	light_direction_vector_mv = headlightMatrix * light_direction_vector;
	// set light_position to the origin of the headlightMatrix
	light_position_headlight = vec4(0.0,0.0,0.0,1.0);
	// get the light position translated - headlightMatrix coordinates
	light_position_headlight =	headlightMatrix * light_position_headlight;
	/*
	printf("light_position_headlight: %f,%f,%f,%f\n", 
		light_position_headlight.x,
		light_position_headlight.y,
		light_position_headlight.z,
		light_position_headlight.w);
	printf("light_direction_headlight: %f,%f,%f,%f\n", 
		light_direction_vector_mv.x,
		light_direction_vector_mv.y,
		light_direction_vector_mv.z,
		light_direction_vector_mv.w);
		*/
	lightDirections[HEADLIGHT_LEFT] = light_position_headlight;
	glUniform4fv(light_position, 1, light_position_headlight);
	glUniform3fv(light_direction, 1, light_direction_vector_mv);
	glBindVertexArray(vao[HEADLIGHT]);
	glDrawArrays( GL_TRIANGLE_FAN, 0, headlightvertcount );    // draw the headlights
	


	/* right headlight */
	headlightMatrix = wholeCarMatrix * Translate(-0.7,0.4,0.01);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, headlightMatrix);
	// light direction = negative z direction, slight negative y
	light_direction_vector = vec4(0.05, -0.19, 0.5f, 0.0);
	// headlightMatrix applied to light_direction_vector
	light_direction_vector_mv = headlightMatrix * light_direction_vector;
	// set light_position to the origin of the headlightMatrix
	light_position_headlight = vec4(0.0,0.0,0.0,1.0);
	// get the light position translated - headlightMatrix coordinates
	light_position_headlight =	headlightMatrix * light_position_headlight;
	
	glUniform4fv(rightHeadlight_position, 1, light_position_headlight);
	glUniform3fv(rightHeadlight_direction, 1, light_direction_vector_mv);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, headlightMatrix);
	glBindVertexArray( vao[HEADLIGHT] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, headlightvertcount );    // draw the headlights

	/* draw a head */
	headMatrix = wholeCarMatrix * Translate(0, 1.5, -1.0);
	headMatrix = headMatrix * RotateY(rotateHead);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, headMatrix);
	glBindVertexArray( vao[HEAD] );
	glDrawArrays( GL_TRIANGLES, 0, spherevertcount );    // draw the sphere

	/* draw eyes */
	mat4 eyeMat;
	eyeMat = headMatrix * Translate(-.2, .3, .8);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, eyeMat);
	glBindVertexArray( vao[EYE] );
	glDrawArrays( GL_TRIANGLES, 0, spherevertcount );    // draw the sphere

	eyeMat = headMatrix;
	eyeMat = headMatrix * Translate(.2, .3, .8);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, eyeMat);
	glBindVertexArray( vao[EYE] );
	glDrawArrays( GL_TRIANGLES, 0, spherevertcount );    // draw the sphere

	allWheelsMatrix = wholeCarMatrix;

	/**************************************************************************************** draw a wheel - FRONT LEFT */
	mat4 frontLeft = allWheelsMatrix;
	frontLeft = frontLeft * Translate(2.0, -1.0, -0.7);
	frontLeft = frontLeft * RotateY(90+(wheelsTurned*5));
	frontLeft = frontLeft * RotateZ(wheelRotation*20);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, frontLeft);
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );    // draw the circle

	/* inner wheel */
	mat4 frontLeftInner = frontLeft;
	frontLeftInner = frontLeftInner * Translate(0.0, 0.0, -0.9);
	frontLeftInner = frontLeftInner * RotateY(180);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, frontLeftInner);
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );    

	/* HUBCAP */
	frontLeft = frontLeft * Translate(0.0,0.0,0.1);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, frontLeft);
	glBindVertexArray( vao[HUBCAP] );
	glDrawArrays( GL_TRIANGLES, 0, 3 );   

	/* draw tire tread - FRONT LEFT */
	mat4 frontLeftTread = frontLeft;
	frontLeftTread = frontLeftTread * Translate(0.0, 0.0, -1.0);
	glUniform3fv(Ka, 1, vec3(0.0f,0.0f,0.0f));
	glUniform3fv(Ks, 1, vec3(0.0f,0.0f,0.0f));
	glUniform3fv(Kd, 1, vec3(0.1f,0.1f,0.1f));
	glUniformMatrix4fv(model_view, 1, GL_TRUE, frontLeftTread);
	glBindVertexArray( vao[TIRETREAD] );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, tireTreadvertcount ); 

#pragma region draw a wheel - FRONT RIGHT
	//allWheelsMatrix = wholeCarMatrix;
	mat4 frontRight = allWheelsMatrix;
	frontRight = frontRight * Translate(-2.0, -1.0, -0.7);
	frontRight = frontRight * RotateY(270+(wheelsTurned*5));
	frontRight = frontRight * RotateZ(-wheelRotation*20);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, frontRight);
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );    // draw the circle

	/* inner wheel */
	inner = frontRight;
	inner = inner * Translate(0.0, 0.0, -0.9);
	inner = inner * RotateY(180);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, inner);
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );  

	/* HUBCAP */
	frontRight = frontRight * Translate(0.0,0.0,0.1);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, frontRight);
	glBindVertexArray( vao[HUBCAP] );
	glDrawArrays( GL_TRIANGLES, 0, 3 );   

	/* draw tire tread - FRONT RIGHT */
	mat4 frontRightTread = frontRight;
	frontRightTread = frontRightTread * Translate(0.0, 0.0, -1.0);
	glUniform3fv(Ka, 1, vec3(0.0f,0.0f,0.0f));
	glUniform3fv(Ks, 1, vec3(0.0f,0.0f,0.0f));
	glUniform3fv(Kd, 1, vec3(0.1f,0.1f,0.1f));
	glUniformMatrix4fv(model_view, 1, GL_TRUE, frontRightTread);
	glBindVertexArray( vao[TIRETREAD] );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, tireTreadvertcount ); 
#pragma endregion
#pragma region draw a wheel - BACK LEFT

	/**************************************************************************************** draw a wheel - BACK LEFT */
	mat4 backLeft = allWheelsMatrix;
	backLeft = backLeft * Translate(2.05, -1.0, 0.7 - (CAR_LENGTH));
	backLeft = backLeft * RotateX(wheelRotation*20);
	backLeft = backLeft * RotateY(90);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, backLeft);
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );    // draw the circle
	/* inner wheel */
	inner = backLeft;
	inner = inner * Translate(0.0, 0.0, -0.9);
	inner = inner * RotateY(180);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, inner);
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );
	/* HUBCAP */
	backLeft = backLeft * Translate(0.0,0.0,0.1);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, backLeft);
	glBindVertexArray( vao[HUBCAP] );
	glDrawArrays( GL_TRIANGLES, 0, 3 ); 

	/* draw tire tread - BACK LEFT */
	mat4 backLeftTread = backLeft;
	backLeftTread = backLeftTread * Translate(0.0, 0.0, -1.0);
	glUniform3fv(Ka, 1, vec3(0.0f,0.0f,0.0f));
	glUniform3fv(Ks, 1, vec3(0.0f,0.0f,0.0f));
	glUniform3fv(Kd, 1, vec3(0.1f,0.1f,0.1f));
	glUniformMatrix4fv(model_view, 1, GL_TRUE, backLeftTread);
	glBindVertexArray( vao[TIRETREAD] );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, tireTreadvertcount ); 
#pragma endregion
#pragma region draw a wheel - BACK RIGHT
	/**************************************************************************************** draw a wheel - BACK RIGHT */
	mat4 backRight = allWheelsMatrix;
	backRight = backRight * Translate(-2.0, -1.0, 0.7 - (CAR_LENGTH));
	backRight = backRight * RotateX(wheelRotation*20);
	backRight = backRight * RotateY(270);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, backRight);
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );    // draw the circle
	/* inner wheel */
	inner = backRight;
	inner = inner * Translate(0.0, 0.0, -0.9);
	inner = inner * RotateY(180);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, inner);
	glBindVertexArray( vao[WHEEL] );
	glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );
	/* HUBCAP */
	backRight = backRight * Translate(0.0,0.0,0.1);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, backRight);
	glBindVertexArray( vao[HUBCAP] );
	glDrawArrays( GL_TRIANGLES, 0, 3 );   

	/* draw tire tread - BACK RIGHT */
	mat4 backRightTread = backRight;
	backRightTread = backRightTread * Translate(0.0, 0.0, -1.0);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, backRightTread);
	// glUniform3fv(location, count, value)
	glUniform3fv(Ka, 1, vec3(0.0f,0.0f,0.0f));
	glUniform3fv(Ks, 1, vec3(0.0f,0.0f,0.0f));
	glUniform3fv(Kd, 1, vec3(0.1f,0.1f,0.1f));
	glBindVertexArray( vao[TIRETREAD] );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, tireTreadvertcount ); 
#pragma endregion
#pragma region	draw stage
	mv = cameraMatrix;
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	// glUniform3fv(location, count, value)
	glUniform3fv(Ka, 1, vec3(0.2f,0.2f,0.2f));
	glUniform3fv(Ks, 1, vec3(0.2f,0.2f,0.2f));
	glUniform3fv(Kd, 1, vec3(0.1f,0.1f,0.1f));
	glBindVertexArray( vao[STAGE] );
	glDrawArrays( GL_TRIANGLES, 0, 6 );
#pragma endregion

	mv = cameraMatrix;

	/* camera position */
	p = Perspective(fov, (float)ww/(float)wh, 1.0, 200.0);
	

	glFlush();
	MRKCheckError();
	/*start processing buffered OpenGL routines*/
	glutSwapBuffers();
}


void reshape(int width, int height){
	ww = width;
	wh = height;
	//field of view angle, aspect ratio, closest distance from camera to object, largest distanec from camera to object
	p = Perspective(fov, (float)width/(float)height, 1.0, 150.0);

	glViewport( 0, 0, width, height );
}

int main(int argc, char **argv)
{
	/*set up window for display*/
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0); 
	glutInitWindowSize(ww, wh);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("SEIS750 - Assignment 3 - Mark Norgren");  

	glewExperimental = GL_TRUE;

	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutTimerFunc(500,my_timer,60);

	glutMainLoop();
	return 0;
}