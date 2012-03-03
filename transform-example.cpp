/*
example implementation of the transformation in-class exercise
**/

#include <GL/Angel.h>
#include <GL/CheckError.h>
#include <math.h>
#pragma comment(lib, "glew32.lib")

//store window width and height
int ww=1200, wh=800;

#define M_PI 3.14159265358979323846

//these are the extra variables needed for the exercise
GLfloat tx, ty, tz;
GLfloat rx, ry, rz;

//our modelview and perspective matrices
mat4 mv, p;

//used for zooming
GLfloat cameraPosition_Dolly = 45.0f;
//used for dollying
GLfloat cameraZoom_FOV = 90.0f;
// camera rotation
GLfloat cameraRotation = 5.0f;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint vColor;

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
	HEAD_VERTS,
	HEAD_COLORS,
	EYE_VERTS,
	EYE_COLORS,
	STAGE_VERTS,
	STAGE_COLORS,
	NUMBER_OF_VBO_OBJECTS
};

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
		carColors[i] = vec4(1.0, 1.0, 1.0, 1.0); //front
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
	carVerts[17] = vec4(1.0f, 1.0f,		-(CAR_LENGTH),	1.0);

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

void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	if(key == 'a')
		tz += 0.1;
	else if(key == 'z')
		tz -= 0.1;
	else if(key == 's')
	{
		printf("s pressed, cameraPosition_Dolly: %f\n", cameraPosition_Dolly);
		if (cameraPosition_Dolly < 200) 
		{
			printf("cameraPosition_Dolly < -25\n");
			cameraPosition_Dolly++;
		}
	}
	else if (key == 'x')
	{
		printf("s pressed, cameraPosition_Dolly: %f\n", cameraPosition_Dolly);
		if (cameraPosition_Dolly > 5) {
			printf("cameraPosition_Dolly > -400\n");
			cameraPosition_Dolly--;
		}
	}
	else if (key == 'f')
	{
		if (cameraRotation < 360) cameraRotation++;
		else cameraRotation = 0;
	}
	else if (key == 'v')
	{
		if (cameraRotation > 0) cameraRotation--;
		else cameraRotation = 360;
	}
	else if(key == 'i'){
		rx += 5;
		if(rx > 360)
			rx -= 360;
	}
	else if(key == 'j'){
		rx -= 5;
		if(rx < 0)
			rx += 360;
	}
	else if(key == 'o'){
		ry += 5;
		if(ry > 360)
			ry -= 360;
	}
	else if(key == 'k'){
		ry -= 5;
		if(ry < 0)
			ry += 360;
	}else if(key == 'p'){
		rz += 5;
		if(rz > 360)
			rz -= 360;
	}else if(key == 'l'){
		rz -= 5;
		if(rz < 0)
			rz += 360;
	}
	//don't forget to request a new frame since parameters have changed
	glutPostRedisplay();

}

void special(int key, int x, int y){
	if(key == GLUT_KEY_UP)
		ty += 0.1;
	else if(key == GLUT_KEY_DOWN)
		ty -= 0.1;
	else if(key == GLUT_KEY_LEFT)
		tx -= 0.1;
	else if(key == GLUT_KEY_RIGHT)
		tx += 0.1;

	//don't forget to request a new frame since parameters have changed
	glutPostRedisplay();
}


/*****************************************************/
/* HEAD OBJECT */
/*****************************************************/
#define M_PI 3.14159265358979323846

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

	int totalverts = ceil(subdiv/2.0)*subdiv * 6;

	if(circle_verts){
		delete[] circle_verts;
	}
	circle_verts = new vec4[totalverts];
	circle_colors = new vec4[totalverts];
	for(int i=0; i<totalverts; i++){
		circle_colors[i] = color; //white
	}

	int i;
	int sections = subdiv; //number of triangles to use to estimate a circle
	GLfloat twoPi = 2.0f * M_PI;

	
	
	for(i = 0; i <= sections;i++) 
	{	
		// make $section number of circles
		circle_verts[i]=   vec4(radius * cos(i *  twoPi / sections), radius* sin(i * twoPi / sections), 0.0f);
	}
	return totalverts;
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
	spherevertcount = generateSphere(0.1, 30, vec4(1.0, 1.0, 1.0, 1.0));

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
	// generate vertices for head 
	circlevertcount = generateCircle(0.1, 30, vec4(0.0, 0.0, 1.0, 1.0));

	glBindVertexArray( vao[EYE] );
	glGenBuffers( 2, &vbo[EYE_VERTS] );
	glBindBuffer( GL_ARRAY_BUFFER, vbo[EYE_VERTS] );
	glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);
	vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//and now our colors for each vertex
	glBindBuffer( GL_ARRAY_BUFFER, vbo[EYE_COLORS] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(sphere_colors), sphere_colors, GL_STATIC_DRAW );
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

	cameraMatrix = LookAt(vec4(0, 20, cameraPosition_Dolly, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));
	cameraMatrix = cameraMatrix * RotateY(cameraRotation);

	mv = LookAt(vec4(0, 0, cameraPosition_Dolly, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));
	wholeCarMatrix = cameraMatrix  * RotateX(rx);
	wholeCarMatrix = cameraMatrix  * RotateY(ry);
	wholeCarMatrix = cameraMatrix  * RotateZ(rz);
	wholeCarMatrix = cameraMatrix * Translate(tx, ty, tz);

	mv = cameraMatrix * mv;
	mv = mv * Translate(-2,0,0);
	mv = mv * Translate(tx, ty, tz);

	mv = mv * RotateX(rx);
	mv = mv * RotateY(ry);
	mv = mv * RotateZ(rz);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glBindVertexArray( vao[CUBE] );
	glDrawArrays( GL_TRIANGLES, 0, 36 );    // draw the cube 


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
	wholeCarMatrix = wholeCarMatrix * Translate(0, 1.5, -1.0);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, wholeCarMatrix);
	glBindVertexArray( vao[HEAD] );
	glDrawArrays( GL_TRIANGLES, 0, spherevertcount );    // draw the sphere

	/* draw eyes */
	mat4 eyeMat;
	eyeMat = wholeCarMatrix * Translate(-.2, .3, .8);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, eyeMat);
	glBindVertexArray( vao[EYE] );
	glDrawArrays( GL_TRIANGLES, 0, spherevertcount );    // draw the sphere

	eyeMat = wholeCarMatrix;
	eyeMat = wholeCarMatrix * Translate(.2, .3, .8);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, eyeMat);
	glBindVertexArray( vao[EYE] );
	glDrawArrays( GL_TRIANGLES, 0, spherevertcount );    // draw the sphere

	/* draw stage */
	mv = cameraMatrix;
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glBindVertexArray( vao[STAGE] );
	glDrawArrays( GL_TRIANGLES, 0, 6 );    // draw the sphere

	mv = cameraMatrix;

	/* camera position */
	//cameraRotation = 
	mv = LookAt(vec4(0, 25, cameraPosition_Dolly, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0));
	p = Perspective(90.0, (float)ww/(float)wh, 1.0, 200.0);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	glFlush();
	CheckError();
	/*start processing buffered OpenGL routines*/
	glutSwapBuffers();
}


void reshape(int width, int height){
	ww= width;
	wh = height;
	//field of view angle, aspect ratio, closest distance from camera to object, largest distanec from camera to object
	p = Perspective(90.0, (float)width/(float)height, 1.0, 150.0);

	glViewport( 0, 0, width, height );
}

int main(int argc, char **argv)
{
	/*set up window for display*/
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0); 
	glutInitWindowSize(ww, wh);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Transformations Exercise");  

	glewExperimental = GL_TRUE;

	glewInit();
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	//glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}