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
	STATE_END };
	enum state current_state = STATE_STATIC_CAMERA;


	GLfloat rx, ry, rz;

	//modelview and perspective matrices
	mat4 mv, p;

	//used for zooming
	GLfloat cameraPosition_Dolly = 90.0f;
	//used for dollying
	GLfloat cameraZoom_FOV = 90.0f;
	// camera rotation
	GLfloat cameraRotation = 5.0f;

	// camera lookat point
	bool lookAtCenterOfStage = true;
	vec4 cameraLookAtPoint	= (0.0f, 0.0f, 0.0f, 1.0);
	vec4 cameraLookAtEye	= vec4(0, 20, cameraPosition_Dolly, 1.0);
	// camera zoom
	float fov = 45.0;


	//and we'll need pointers to our shader variables
	GLuint model_view;
	GLuint projection;
	GLuint vPosition;
	GLuint vColor;

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
		CUBE_COLORS,
		CAR_VERTS,
		CAR_COLORS,
		WHEEL_VERTS,
		WHEEL_COLORS,
		TIRETREAD_VERTS,
		TIRETREAD_COLORS,
		HUBCAP_VERTS,
		HUBCAP_COLORS,
		HEAD_VERTS,
		HEAD_COLORS,
		EYE_VERTS,
		EYE_COLORS,
		STAGE_VERTS,
		STAGE_COLORS,
		NUMBER_OF_VBO_OBJECTS
	};


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
				if (fov<170) fov++;
				break;
			}
		case 'a': // ZOOM IN
			{
				if (fov>1) fov--;
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
				rotateHead += 5;
				if (rotateHead > 360)
					rotateHead -= 360;
				break;
			}
		case 'x':
			{
				rotateHead -= 5;
				if (rotateHead < 360)
					rotateHead+= 360;
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

	void generateStage() {
		for(int i=0; i<6; i++){
			stageColors[i] = vec4(0.5, 0.5, 0.5, 1.0); //front
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
	void generateCar() {
		for(int i=0; i<6; i++){
			carColors[i] = vec4(1.0, 1.0, 0.0, 1.0); //front
		}
		carVerts[0] = vec4(CAR_WIDTH/2,		-(CAR_HEIGHT/2),	0.0f, 1.0);
		carVerts[1] = vec4(CAR_WIDTH/2,		CAR_HEIGHT/2,		0.0f, 1.0);
		carVerts[2] = vec4(-(CAR_WIDTH/2),	CAR_HEIGHT/2,		0.0f, 1.0);
		carVerts[3] = vec4(-(CAR_WIDTH/2),	CAR_HEIGHT/2,		0.0f, 1.0);
		carVerts[4] = vec4(-(CAR_WIDTH/2),	-(CAR_HEIGHT/2),	0.0f, 1.0);
		carVerts[5] = vec4(CAR_WIDTH/2,		-(CAR_HEIGHT/2),	0.0f, 1.0);

		for(int i=6; i<12; i++){
			carColors[i] = vec4(0.0, 0.0, 0.0, 1.0); //back, black
		}
		//				X						Y					Z
		carVerts[6] = vec4(-(CAR_WIDTH/2),	-(CAR_HEIGHT/2),	-(CAR_LENGTH), 1.0);
		carVerts[7] = vec4(-(CAR_WIDTH/2),	CAR_HEIGHT/2,		-(CAR_LENGTH), 1.0);
		carVerts[8] = vec4(CAR_WIDTH/2,		CAR_HEIGHT/2,		-(CAR_LENGTH), 1.0);
		carVerts[9] = vec4(CAR_WIDTH/2,		CAR_HEIGHT/2,		-(CAR_LENGTH), 1.0);
		carVerts[10] = vec4(CAR_WIDTH/2,	-(CAR_HEIGHT/2),	-(CAR_LENGTH), 1.0);
		carVerts[11] = vec4(-(CAR_WIDTH/2), -(CAR_HEIGHT/2),	-(CAR_LENGTH), 1.0);

		for(int i=12; i<18; i++){
			carColors[i] = vec4(0.0, 1.0, 0.0, 1.0); //right
		}
		carVerts[12] = vec4(1.0f, 1.0f,		0.0f,			1.0);
		carVerts[13] = vec4(1.0f, -1.0f,	0.0f,			1.0);
		carVerts[14] = vec4(1.0f, -1.0f,	-(CAR_LENGTH),	1.0);
		carVerts[15] = vec4(1.0f, -1.0f,	-(CAR_LENGTH),	1.0);
		carVerts[16] = vec4(1.0f, 1.0f,		-(CAR_LENGTH),	1.0);
		carVerts[17] = vec4(1.0f, 1.0f,		0.0f,			1.0);

		for(int i=18; i<24; i++){
			carColors[i] = vec4(1.0, 0.0, 0.0, 1.0); //left
		}
		carVerts[18] = vec4(-1.0f, 1.0f,	-(CAR_LENGTH),		1.0);
		carVerts[19] = vec4(-1.0f, -1.0f,	-(CAR_LENGTH),		1.0);
		carVerts[20] = vec4(-1.0f, -1.0f,	0.0f,				1.0);
		carVerts[21] = vec4(-1.0f, -1.0f,	0.0f,				1.0);
		carVerts[22] = vec4(-1.0f, 1.0f,	0.0f,				1.0);
		carVerts[23] = vec4(-1.0f, 1.0f,	-(CAR_LENGTH),		1.0);

		for(int i=24; i<30; i++){
			carColors[i] = vec4(0.0, 0.0, 1.0, 1.0); //top
		}
		carVerts[24] = vec4(1.0f, 1.0f, 0.0f, 1.0);
		carVerts[25] = vec4(1.0f, 1.0f, -(CAR_LENGTH), 1.0);
		carVerts[26] = vec4(-1.0f, 1.0f, -(CAR_LENGTH), 1.0);
		carVerts[27] = vec4(-1.0f, 1.0f, -(CAR_LENGTH), 1.0);
		carVerts[28] = vec4(-1.0f, 1.0f, 0.0f, 1.0);
		carVerts[29] = vec4(1.0f, 1.0f, 0.0f, 1.0);

		for(int i=30; i<36; i++){
			carColors[i] = vec4(0.0, 1.0, 0.0, 1.0); //bottom
		}
		carVerts[30] = vec4(1.0f, -1.0f, -5.0f, 1.0);
		carVerts[31] = vec4(1.0f, -1.0f, 0.0f, 1.0);
		carVerts[32] = vec4(-1.0f, -1.0f, 0.0f, 1.0);
		carVerts[33] = vec4(-1.0f, -1.0f, 0.0f, 1.0);
		carVerts[34] = vec4(-1.0f, -1.0f, -5.0f, 1.0);
		carVerts[35] = vec4(1.0f, -1.0f, -5.0f, 1.0);
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

	/** CUBE OBJECT **/
	vec4 cubeVerts[36];
	vec4 cubeColors[36];
	void generateCube(){
		for(int i=0; i<6; i++){
			cubeColors[i] = vec4(0.0, 1.0, 1.0, 1.0); //front
		}
		cubeVerts[0] = vec4(1.0f, -1.0f, 1.0f, 1.0);
		cubeVerts[1] = vec4(1.0f, 1.0f, 1.0f, 1.0);
		cubeVerts[2] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
		cubeVerts[3] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
		cubeVerts[4] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
		cubeVerts[5] = vec4(1.0f, -1.0f, 1.0f, 1.0);


		for(int i=6; i<12; i++){
			cubeColors[i] = vec4(1.0, 0.0, 1.0, 1.0); //back
		}
		cubeVerts[6] = vec4(-1.0f, -1.0f, -1.0f, 1.0);
		cubeVerts[7] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
		cubeVerts[8] = vec4(1.0f, 1.0f, -1.0f, 1.0);
		cubeVerts[9] = vec4(1.0f, 1.0f, -1.0f, 1.0);
		cubeVerts[10] = vec4(1.0f, -1.0f, -1.0f, 1.0);
		cubeVerts[11] = vec4(-1.0f, -1.0f, -1.0f, 1.0);

		for(int i=12; i<18; i++){
			cubeColors[i] = vec4(1.0, 1.0, 0.0, 1.0); //left
		}
		cubeVerts[12] = vec4(1.0f, 1.0f, 1.0f, 1.0);
		cubeVerts[13] = vec4(1.0f, -1.0f, 1.0f, 1.0);
		cubeVerts[14] = vec4(1.0f, -1.0f, -1.0f, 1.0);
		cubeVerts[15] = vec4(1.0f, -1.0f, -1.0f, 1.0);
		cubeVerts[16] = vec4(1.0f, 1.0f, -1.0f, 1.0);
		cubeVerts[17] = vec4(1.0f, 1.0f, 1.0f, 1.0);

		for(int i=18; i<24; i++){
			cubeColors[i] = vec4(1.0, 0.0, 0.0, 1.0); //right
		}
		cubeVerts[18] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
		cubeVerts[19] = vec4(-1.0f, -1.0f, -1.0f, 1.0);
		cubeVerts[20] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
		cubeVerts[21] = vec4(-1.0f, -1.0f, 1.0f, 1.0);
		cubeVerts[22] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
		cubeVerts[23] = vec4(-1.0f, 1.0f, -1.0f, 1.0);

		for(int i=24; i<30; i++){
			cubeColors[i] = vec4(0.0, 0.0, 1.0, 1.0); //top
		}
		cubeVerts[24] = vec4(1.0f, 1.0f, 1.0f, 1.0);
		cubeVerts[25] = vec4(1.0f, 1.0f, -1.0f, 1.0);
		cubeVerts[26] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
		cubeVerts[27] = vec4(-1.0f, 1.0f, -1.0f, 1.0);
		cubeVerts[28] = vec4(-1.0f, 1.0f, 1.0f, 1.0);
		cubeVerts[29] = vec4(1.0f, 1.0f, 1.0f, 1.0);

		for(int i=30; i<36; i++){
			cubeColors[i] = vec4(0.0, 1.0, 0.0, 1.0); //bottom
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
	int generateSphere(float radius, int subdiv, vec4 color){
		float step = (360.0/subdiv)*(M_PI/180.0);

		int totalverts = ceil(subdiv/2.0)*subdiv * 6;

		if(sphere_verts){
			delete[] sphere_verts;
		}
		sphere_verts = new vec4[totalverts];
		sphere_colors = new vec4[totalverts];
		for(int i=0; i<totalverts; i++){
			sphere_colors[i] = color; //white
		}

		int k = 0;
		for(float i = -M_PI/2; i<=M_PI/2; i+=step){
			for(float j = -M_PI; j<=M_PI; j+=step){
				//triangle 1
				sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
				k++;

				sphere_verts[k]=   vec4(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step), 1.0);
				k++;

				sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
				k++;

				//triangle 2
				sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
				k++;

				sphere_verts[k]=   vec4(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i), 1.0);
				k++;

				sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
				k++;
			}
		}
		return totalverts;
	}

	int circlevertcount;
	vec4* circle_verts;
	vec4* circle_colors;
	int generateCircle(float radius, int subdiv, vec4 color){
		float step = (360.0/subdiv)*(M_PI/180.0);
		printf("step: %f\n", step);

		int totalverts = 360+2;//ceil(subdiv/2.0)*subdiv ;
		printf("totalVerts: %d\n", totalverts);
		if(circle_verts){
			delete[] circle_verts;
		}

		/* COLOR */
		circle_verts = new vec4[totalverts];
		circle_colors = new vec4[totalverts];
		for(int i=0; i<totalverts; i++){
			circle_colors[i] = color; //white
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

#if DEBUG
		for (int i=0;i<=totalverts;i++)
		{
			printf("index,%d,x,%f,y,%f,z,%f\n", i, circle_verts[i][0], circle_verts[i][1], circle_verts[i][2]); 
		}
#endif

		return totalverts;
	}


	int tireTreadvertcount;
	vec4* tireTread_verts;
	vec4* tireTread_colors;
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
			if (i==0) firstOne = vec4(x, y, z, 1.0);
			z = 1.0f;
			vert_count++;
			tireTread_verts[vert_count] = vec4(x, y, z, 1.0);
			if (i==0) firstOneOtherHalf = vec4(x, y, z, 1.0);
			vert_count++;
		}

		/* finish up triangle strips with firsts */
		tireTread_verts[vert_count] = firstOne;
		vert_count++;
		tireTread_verts[vert_count] = firstOneOtherHalf;
		vert_count++;
		tireTread_verts[vert_count] = firstOne;

#if DEBUG
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
			else {
				// not outside of stage bounds
				//if (current_state == STATE_VIEWPOINT_CAMERA)
				//{
				//	// update viewPointLookAtPoint
				//	//viewPointLookAtPoint = vec4((tx+5)*cos(carHeading), (tz+5)*cos(carHeading), 0.0f, 1.0);
				//	//cameraLookAtPoint = viewPointLookAtDirection;

				//	viewPointLookAt_Eye = vec4(tx,ty+5,tz, 1.0);
				//	viewPointLookAt_At = vec4(tx + (VIEWPOINT_LOOK_AT_DISTANCE*sin(carHeading)), ty, tz + (VIEWPOINT_LOOK_AT_DISTANCE*cos(carHeading)), 1.0);
				//}
				//else cameraLookAtPoint = vec4(0.0f, 0.0f, 0.0f, 1.0);
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
			eyeX = tx - (2 * sin(carHeading));
			eyeZ = tz - (2 * cos(carHeading));
			viewPointLookAt_Eye = vec4(eyeX,ty+5,eyeZ, 1.0);
			atX = tx + (20 * sin(carHeading));
			atZ = tz + (20 * cos(carHeading));
			viewPointLookAt_At = vec4(atX, ty, atZ, 1.0);
			printf("cameraLookAtEye: %f,%f,%f\n", cameraLookAtEye.x, cameraLookAtEye.y, cameraLookAtEye.z);
			printf("cameraLookAtPoint: %f,%f,%f\n", cameraLookAtPoint.x, cameraLookAtPoint.y, cameraLookAtPoint.z);
			break;
		case STATE_CHASE_CAMERA:
			eyeX = tx - (20 * sin(carHeading));
			eyeZ = tz - (20 * cos(carHeading));
			viewPointLookAt_Eye = vec4(eyeX,ty+15,eyeZ, 1.0);
			atX = tx + (10 * sin(carHeading));
			atZ = tz + (10 * cos(carHeading));
			viewPointLookAt_At = vec4(atX, ty, atZ, 1.0);
			printf("viewPointLookAt_Eye: %f,%f,%f\n", viewPointLookAt_Eye.x, viewPointLookAt_Eye.y, viewPointLookAt_Eye.z);
			printf("viewPointLookAt_At: %f,%f,%f\n", viewPointLookAt_At.x, viewPointLookAt_At.y, viewPointLookAt_At.z);
			break;
		}
		glutPostRedisplay();
		glutTimerFunc(1000/v, my_timer, v);
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

		// Create a vertex array object
		glGenVertexArrays( NUMBER_OF_VAO_OBJECTS, vao );

		// CUBE
		// Create and initialize any buffer objects
		glBindVertexArray( vao[CUBE] );
		glGenBuffers( 2, &vbo[CUBE_VERTS] );
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

		/*********************************************************
		* CAR
		*
		*********************************************************/
		// Create and initialize **CAR** buffer objects
		// Create a vertex array object
		//glGenVertexArrays( 1, &vao[1] );
		glBindVertexArray( vao[CAR] );
		glGenBuffers( 2, &vbo[CAR_VERTS] );
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

		//grab pointers for our modelview and perspecive uniform matrices
		model_view = glGetUniformLocation(program, "model_view");
		projection = glGetUniformLocation(program, "projection");

		/*********************************************************
		/** HEAD 
		/*********************************************************/
		// generate vertices for head 
		spherevertcount = generateSphere(0.8, 30, vec4(1.0, 0.5, 0.5, 1.0));

		glBindVertexArray( vao[HEAD] );
		glGenBuffers( 2, &vbo[HEAD_VERTS] );
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

		/*********************************************************
		/** EYE 
		/*********************************************************/
		// generate vertices for head 
		spherevertcount = generateSphere(0.1, 30, vec4(0.0, 0.0, 1.0, 1.0));

		glBindVertexArray( vao[EYE] );
		glGenBuffers( 2, &vbo[EYE_VERTS] );
		glBindBuffer( GL_ARRAY_BUFFER, vbo[EYE_VERTS] );
		glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);
		vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

		//and now our colors for each vertex
		glBindBuffer( GL_ARRAY_BUFFER, vbo[EYE_COLORS] );
		glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec4), sphere_colors, GL_STATIC_DRAW );
		vColor = glGetAttribLocation(program, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);

		/*********************************************************
		/** WHEEL 
		/*********************************************************/
		// generate vertices 
		circlevertcount = generateCircle(1.0, 30, vec4(1.0, 1.0, 1.0, 1.0));

		glBindVertexArray( vao[WHEEL] );
		glGenBuffers( 2, &vbo[WHEEL_VERTS] );
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

		/*********************************************************
		/** TIRE TREAD 
		/*********************************************************/
		// generate vertices 
		tireTreadvertcount = generateTireTread(1.0, 30, vec4(0.0, 0.0, 0.0, 1.0));

		glBindVertexArray( vao[TIRETREAD] );
		glGenBuffers( 2, &vbo[TIRETREAD_VERTS] );
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
		glGenBuffers( 2, &vbo[STAGE_VERTS] );
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

		//grab pointers for our modelview and perspecive uniform matrices
		model_view = glGetUniformLocation(program, "model_view");
		projection = glGetUniformLocation(program, "projection");

		//Only draw the things in the front layer
		glEnable(GL_DEPTH_TEST);
	}

	void display(void)
	{
		/*clear all pixels*/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mat4 cameraMatrix;
		mat4 wholeCarMatrix;
		mat4 headMatrix;
		mat4 frontWheelsMatrix;
		mat4 allWheelsMatrix;
		mat4 inner;


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



		cameraMatrix = LookAt(cameraLookAtEye, cameraLookAtPoint, vec4(0, 1, 0, 0.0));
		cameraMatrix = cameraMatrix * RotateY(cameraRotation);
		//mv = LookAt(vec4(0, 0, cameraPosition_Dolly, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));
		wholeCarMatrix = cameraMatrix  * RotateX(rx);
		wholeCarMatrix = cameraMatrix  * RotateY(ry);
		wholeCarMatrix = cameraMatrix  * RotateZ(rz);
		wholeCarMatrix = cameraMatrix * Translate(tx, ty, tz);

		mv = cameraMatrix * mv;
		mv = mv * Translate(0,0,0);
		mv = mv * Translate(tx, ty, tz);

		mv = mv * RotateX(rx);
		mv = mv * RotateY(ry);
		mv = mv * RotateZ(rz); 


		/* CORNER CUBES */
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
		glUniformMatrix4fv(projection, 1, GL_TRUE, p);

		glBindVertexArray( vao[CAR] );
		glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube

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
		glUniformMatrix4fv(model_view, 1, GL_TRUE, frontLeftTread);
		glBindVertexArray( vao[TIRETREAD] );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, tireTreadvertcount ); 

		/**************************************************************************************** draw a wheel - FRONT RIGHT */
		//allWheelsMatrix = wholeCarMatrix;
		mat4 frontRight = allWheelsMatrix;
		frontRight = frontRight * Translate(-2.0, -1.0, -0.7);
		frontRight = frontRight * RotateY(90+(wheelsTurned*5));
		frontRight = frontRight * RotateZ(wheelRotation*20);

		glUniformMatrix4fv(model_view, 1, GL_TRUE, frontRight);
		glBindVertexArray( vao[WHEEL] );
		glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );    // draw the circle

		/* inner wheel */
		inner = frontRight;
		inner = inner * Translate(0.0, 0.0, 0.9);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, inner);
		glBindVertexArray( vao[WHEEL] );
		glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );  
		/* HUBCAP */
		frontRight = frontRight * Translate(0.0,0.0,-0.1);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, frontRight);
		glBindVertexArray( vao[HUBCAP] );
		glDrawArrays( GL_TRIANGLES, 0, 3 );   

		/* draw tire tread - FRONT RIGHT */
		mat4 frontRightTread = frontRight;
		frontRightTread = frontRightTread * Translate(0.0, 0.0, 0.0);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, frontRightTread);
		glBindVertexArray( vao[TIRETREAD] );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, tireTreadvertcount ); 

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
		glUniformMatrix4fv(model_view, 1, GL_TRUE, backLeftTread);
		glBindVertexArray( vao[TIRETREAD] );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, tireTreadvertcount ); 

		/**************************************************************************************** draw a wheel - BACK RIGHT */
		mat4 backRight = allWheelsMatrix;
		backRight = backRight * Translate(-2.0, -1.0, 0.7 - (CAR_LENGTH));
		backRight = backRight * RotateX(wheelRotation*20);
		backRight = backRight * RotateY(90);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, backRight);
		glBindVertexArray( vao[WHEEL] );
		glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );    // draw the circle
		/* inner wheel */
		inner = backRight;
		inner = inner * Translate(0.0, 0.0, 0.9);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, inner);
		glBindVertexArray( vao[WHEEL] );
		glDrawArrays( GL_TRIANGLE_FAN, 0, circlevertcount );
		/* HUBCAP */
		backRight = backRight * Translate(0.0,0.0,-0.1);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, backRight);
		glBindVertexArray( vao[HUBCAP] );
		glDrawArrays( GL_TRIANGLES, 0, 3 );   

		/* draw tire tread - BACK RIGHT */
		mat4 backRightTread = backRight;
		backRightTread = backRightTread * Translate(0.0, 0.0, 0.0);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, backRightTread);
		glBindVertexArray( vao[TIRETREAD] );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, tireTreadvertcount ); 

		/* draw stage */
		mv = cameraMatrix;
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
		glBindVertexArray( vao[STAGE] );
		glDrawArrays( GL_TRIANGLES, 0, 6 );    // draw the sphere

		mv = cameraMatrix;

		/* camera position */
		//cameraRotation = 
		mv = LookAt(vec4(0, 25, cameraPosition_Dolly, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));
		p = Perspective(fov, (float)ww/(float)wh, 1.0, 200.0);
		glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
		glUniformMatrix4fv(projection, 1, GL_TRUE, p);

		glFlush();
		MRKCheckError();
		/*start processing buffered OpenGL routines*/
		glutSwapBuffers();
	}


	void reshape(int width, int height){
		ww= width;
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
		glutCreateWindow("SEIS750 - Assignment 2 - Mark Norgren");  

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