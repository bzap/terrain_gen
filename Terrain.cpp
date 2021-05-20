// Linas Aleknevicius | Nov 5, 2020

// updated Dec 3, 2020

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector> 
using namespace std;

int terrain; 
int overview;
float scale;
int size = 300; 
vector<vector<float> > newMap(size, vector<float> (size, 0));
vector<vector<vector<float> > > normals(size, vector<vector<float> >(size, vector<float>(3)));

float high = 0;
float low = 0;
float greyVal;
bool algo = false;
int wire = 0;
int shape = 0;
int xangle = 0;
int yangle = 0;
int currLight = 0;

GLfloat ambient0[4] = {0.576, 0.439, 0.859, 1};
GLfloat diffuse0[4] = {0.2, 0.2, 1, 1}; 
GLfloat specular0[4] = {0.600, 0.196, 0.800, 1};
GLfloat pos0[4] = {(float) size, (float) size, (float) size, 1};

GLfloat ambient1[4] = {0.576, 0.439, 0.859, 1};
GLfloat diffuse1[4] = {0.2, 0.2, 1, 1}; 
GLfloat specular1[4] = {0.600, 0.196, 0.800, 1};
GLfloat pos1[4] = {0, (float) size, 0, 1};

// generate a fault heightmap
void makeFaults() {
	// initialize to default values 
	for (int x = 0; x < size; x++) {
		for (int z = 0; z < size; z++) {
			newMap[x][z] = 0;
		}
	}
	low = 0;
	high = 0;
	float v, a, b, d, c;
	// heavily based off the lighthouse3d fault algorithm 
	// http://www.lighthouse3d.com/opengl/terrain/index.php?fault
	for (int i = 0; i < 100; i++) {
		v = rand();
		a = sin(v);
		b = cos(v);
		d = sqrtf((size * size) * 2);
		c = (rand() / (float) RAND_MAX) * d - d / 2;
		for (int x = 0; x < size; x++) {
			for (int z = 0; z < size; z++) {
		        if (a * x + b * z - c > 0) {                  
                    newMap[x][z] += 1;
                }
                else {  
                	newMap[x][z] -= 1;  
                }	
				if(newMap[x][z] > high) {
					high = newMap[x][z]; 
				}
				if (newMap[x][z] < low) {
					low = newMap[x][z]; 
				}   	
			}
		}
	}
}

// generate a circle heightmap 
void makeCircles() {
	// initialize to default values 
	for (int x = 0; x < size; x++) {
		for (int z = 0; z < size; z++) {
			newMap[x][z] = 0;
		}
	}
	low = 0; 
	high = 0;
	// heavily based off the lighthouse3d circles algorithm 
	// http://www.lighthouse3d.com/opengl/terrain/index.php?circles
	for (int i = 0; i < size / 4; i++) {
		int xPoint = rand() % size + 1;
		int zPoint = rand() % size + 1;
		int rad = rand() % size + 15;
		int height = rand() % 10;
        for (int x = 0; x < size; x++) {
            for (int z = 0; z < size; z++) {
				float pd = ((sqrtf(powf((x - xPoint), 2) + powf((z - zPoint), 2))) * 2) / rad;
                if (fabs(pd) <= 1.0) {
					newMap[x][z] += height / 2 + cosf(pd * 3.14) * height / 2;
                }
				if (newMap[x][z] > high) {
					high = newMap[x][z]; 
				}
				if (newMap[x][z] < low) {
					low = newMap[x][z]; 
				}
            }	        
        }
    }
}

// render the quad representation
void renderQuads(){
	for (int x = 0; x < size - 1; x++) {
		for (int z = 0; z < size - 1; z++) {
			glBegin(GL_QUADS);
				greyVal = (newMap[x][z] - low) / (high - low);
				glNormal3f(normals[x][z][0], normals[x][z][1], normals[x][z][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x, newMap[x][z], z);
				greyVal = (newMap[x][z + 1] - low) / (high - low);
				glNormal3f(normals[x][z + 1][0], normals[x][z + 1][1], normals[x][z + 1][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x, newMap[x][z + 1], z + 1);
				greyVal = (newMap[x + 1][z + 1] - low) / (high - low);
				glNormal3f(normals[x + 1][z + 1][0], normals[x + 1][z + 1][1], normals[x + 1][z + 1][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x + 1, newMap[x + 1][z + 1], z + 1);
				greyVal = (newMap[x + 1][z] - low) / (high - low);
				glNormal3f(normals[x + 1][z][0], normals[x + 1][z][1], normals[x + 1][z][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x + 1, newMap[x + 1][z], z);
			glEnd();
		}
	}
}

// render the wireframe for quads
void renderQuadWires(){	
	for (int x = 0; x < size - 1; x++){
		for (int z = 0; z < size - 1; z++){
			glBegin(GL_LINE_LOOP);
			glColor3f(0.0, 1.0, 0.0);
			glNormal3f(normals[x][z][0], normals[x][z][1], normals[x][z][2]);
			glVertex3f(x, newMap[x][z], z);
			glNormal3f(normals[x][z + 1][0], normals[x][z + 1][1], normals[x][z + 1][2]);
			glVertex3f(x, newMap[x][z + 1], z + 1);
			glNormal3f(normals[x + 1][z + 1][0], normals[x + 1][z + 1][1], normals[x + 1][z + 1][2]);
			glVertex3f(x + 1, newMap[x + 1][z + 1], z + 1);
			glNormal3f(normals[x + 1][z][0], normals[x + 1][z][1], normals[x + 1][z][2]);
			glVertex3f(x + 1, newMap[x + 1][z], z);
			glEnd();
		}
	}
}

// render the triangle representation
void renderTriangles(){
	for (int x = 0; x < size - 1; x++){
		for (int z = 0; z < size - 1; z++){
			glBegin(GL_TRIANGLES);
				greyVal = (newMap[x][z] - low) / (high - low);
				glNormal3f(normals[x][z][0], normals[x][z][1], normals[x][z][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x, newMap[x][z], z);
				greyVal = (newMap[x][z + 1] - low) / (high - low);
				glNormal3f(normals[x][z + 1][0], normals[x][z + 1][1], normals[x][z + 1][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x, newMap[x][z + 1], z + 1);
				greyVal = (newMap[x + 1][z + 1] - low) / (high - low);
				glNormal3f(normals[x + 1][z + 1][0], normals[x + 1][z + 1][1], normals[x + 1][z + 1][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x + 1, newMap[x + 1][z + 1], z + 1); 
				greyVal = (newMap[x][z] - low) / (high - low);
				glNormal3f(normals[x][z][0], normals[x][z][1], normals[x][z][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x, newMap[x][z], z); 
				greyVal = (newMap[x + 1][z + 1] - low) / (high - low);
				glNormal3f(normals[x + 1][z + 1][0], normals[x + 1][z + 1][1], normals[x + 1][z + 1][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x + 1, newMap[x + 1][z + 1], z + 1);
				greyVal = (newMap[x + 1][z] - low) / (high - low);
				glNormal3f(normals[x + 1][z][0], normals[x + 1][z][1], normals[x + 1][z][2]);
				glColor3f(greyVal, greyVal, greyVal);
				glVertex3f(x + 1, newMap[x + 1][z], z);
			glEnd();
		}
	}
}

// render the wireframe for triangles 
void renderTrianglesWires(){
	for (int x = 0; x < size - 1; x++){
		for (int z = 0; z < size - 1; z++){
			glBegin(GL_LINE_LOOP);
				glColor3f(0.0, 1.0, 0.0);
				glVertex3f(x, newMap[x][z], z);
				glVertex3f(x, newMap[x][z + 1], z + 1);
				glVertex3f(x + 1, newMap[x + 1][z + 1], z + 1); 
				glVertex3f(x, newMap[x][z], z); 
				glVertex3f(x + 1, newMap[x + 1][z + 1], z + 1);
				glVertex3f(x + 1, newMap[x + 1][z], z);
			glEnd();
		}
	}
}

// calculate the normals 
// heavily based off of http://www.lighthouse3d.com/opengl/terrain/index.php?normals
void getNormals() { 
	for (int x = 0; x < size - 1; x++) {
		for (int z = 0; z < size - 1; z++) {
			normals[x][z][0] = 0;
			normals[x][z][1] = 0;
			normals[x][z][2] = 0;
		}
	}

	for (int x = 0; x < size - 1; x++) {
		for (int z = 0; z < size - 1; z++) {
			float v1[3]; 
			float v2[3]; 
			float v3[3]; 
			v1[0] = x; 
			v2[0] = x; 
			v3[0] = x + 1;
			v1[1] = newMap[x][z]; 
			v2[1] = newMap[x][z + 1]; 
			v3[1] = newMap[x + 1][z + 1];
			v1[2] = z; 
			v2[2] = z + 1; 
			v3[2] = z;

			float t1[3];
			float t2[3];
			t1[0] = v2[0] - v1[0];
			t1[1] = v2[1] - v1[1];
			t1[2] = v2[2] - v1[2];
			t2[0] = v3[0] - v1[0];
			t2[1] = v3[1] - v1[1];
			t2[2] = v3[2] - v1[2];

			float vx;
			float vy; 
			float vz; 
			vx = t1[1] * t2[2] - t1[2] * t2[1];
			vy = t1[2] * t2[0] - t1[0] * t2[2]; 
			vz = t1[0] * t2[1] - t1[1] * t2[0];

			float unit = sqrtf(vx * vx + vy * vy + vz * vz);
			float norm[3];
			norm[0] = vx / unit; 
			norm[1] = vy / unit; 
			norm[2] = vz / unit; 

			normals[x][z][0] = norm[0]; 
			normals[x][z][1] = norm[1]; 
			normals[x][z][2] = norm[2]; 
		}
	}
} 

// control what is displayed on the screen
void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glPushMatrix();
	gluLookAt(600, 100, 600, size, 0, size, 0, 1, 0);
	glRotatef(xangle, 1, 0, 1);
	glRotatef(yangle, 0, 1, 1);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0); 
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1); 
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);	

	if (size == 300){ 
		glScalef(1.5, 1.5, 1.5);
	}
	else if (size == 250){
		glScalef(1.9, 1.9, 1.9);
	}
	else if (size == 200){
		glScalef(2.3, 2.3, 2.3);
	}
	else if (size == 150){
		glScalef(2.9, 2.9, 2.9);
	}
	else if (size == 100){
		glScalef(3.9, 3.9, 3.9);
	}
	else if (size == 50){
		glScalef(7, 7, 7);
	}

	if (shape == 0 && wire == 0){ 
		renderQuads(); 
	}
	else if (shape == 1 && wire == 1){ 
		renderTrianglesWires();
	}
	else if (shape == 0 && wire == 1){
		renderQuadWires();
	}
	else if (shape == 1 && wire == 2){ 
		renderTriangles();
		renderTrianglesWires();
	}
	else if (shape == 0 && wire == 2){
		renderQuads();
		renderQuadWires();
	}	
	else if (shape == 1 && wire == 0){
		renderTriangles();
	}	
	else if (shape == 1 && wire == 1){
		renderTrianglesWires();
	}		
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}

//reset the function 
void reset(){
	xangle = 0;
	yangle = 0;
	if (!algo){ 
		makeCircles();
	}
	else {
		makeFaults();
	}	
	getNormals();
	glutSetWindow(overview);
	glutPostRedisplay();
	glutSetWindow(terrain);
}

// to render the points on the second screen 
void renderPoints(){
	float greyVal;
	for (int x = 0; x < size; x++){
		for (int z = 0; z < size; z++){
				glBegin(GL_POINTS);
					greyVal = (newMap[x][z] - low) / (high - low);
					glColor3f(greyVal, greyVal, greyVal);
					glVertex2i(x, z);
				glEnd();
		}
	}
}

void displayOverview(){
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	renderPoints();
	glutSwapBuffers();
}

void getSize(){
	int input;
	std::cout << "\nPlease select the n size of the terrain: \n";
	std::cout << "1. 50x50 \n";
	std::cout << "2. 100x100 \n";
	std::cout << "3. 150x150 \n";
	std::cout << "4. 200x200 \n";
	std::cout << "5. 250x250 \n";
	std::cout << "6. 300x300 \n";
	cin >> input;
	if (input == 6){
		size = 300;
	}
	else if (input == 5){ 
		size = 250;
	}
	else if (input == 4){ 
		size = 200;
	}
	else if (input == 3){ 
		size = 150;
	}
	else if (input == 2){ 
		size = 100;
	}	
	else if (input == 1){ 
		size = 50;
	}
}  

// handle input 
void keyboard(unsigned char key, int x, int y){
	if (key == 's' || key == 'S'){
		shape += 1;
		if (shape == 2){
			shape = 0;
		}
	}
	else if (key == 'r' || key == 'R'){
		reset();
	}
	else if (key == 'w' || key == 'W'){
		wire += 1;
		if (wire == 3){
			wire = 0;
		}	
	}
	else if (key == 'q' || key == 'Q'){
		exit(0);
	}
	else if (key == 'f' || key == 'F'){
		algo = !algo;
	
		reset();
	}
	else if (key == 'c' || key == 'C'){
		if (currLight == 0){ 
			currLight = 1; 
			std::cout << "\nSelected light: light1\n" << flush;
		}
		else if (currLight == 1){
			currLight = 0;
			std::cout << "\nSelected light: light0\n" << flush;
		}
	}
	else if (key == 'j' || key == 'J'){
		if (currLight == 0){
			pos0[0] -= 50;
		}
		else if (currLight == 1){
			pos1[0] -= 50;
		}
		
	}
	else if (key == 'l' || key == 'L'){
		if (currLight == 0){
			pos0[0] += 50;
		}
		else if (currLight == 1){
			pos1[0] += 50;
		}
	}
	else if (key == 'i' || key == 'I'){
		if (currLight == 0){
			pos0[1] += 50;
		}
		else if (currLight == 1){
			pos1[1] += 50;
		}
	}
	else if (key == 'k' || key == 'K'){
		if (currLight == 0){
			pos0[1] -= 50;
		}
		else if (currLight == 1){
			pos1[1] -= 50;
		}
	}
}

// handle arrow keys 
void special(int key, int x, int y) {
	if (key == GLUT_KEY_RIGHT) {
		xangle += 1;
	}
	else if (key == GLUT_KEY_LEFT){ 
		xangle -= 1; 
	}
	else if (key == GLUT_KEY_UP){
		yangle += 1; 
	}
	else if (key == GLUT_KEY_DOWN){ 
		yangle -= 1; 
	}
}

// render everything live 
void anim(int value) { 
	glutPostRedisplay();
	glutTimerFunc(30, anim, 0);
}

// handle the size 
void reshape(int width, int height){
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (width / height), 1, 5000);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
	srand((unsigned int)time(NULL));
	getSize();
	glutInit(&argc, argv);	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1200.0, 1200.0);
	glutInitWindowPosition(0.0, 0.0);
	terrain = glutCreateWindow("3GC3 - Assignment 3");	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutReshapeFunc(reshape);
	glutTimerFunc(30, anim, 0);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);  
	glCullFace(GL_BACK);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos0);
	glLightfv(GL_LIGHT1, GL_POSITION, pos1);
   	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
 
	makeCircles();
	getNormals();
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(size, size);
	overview = glutCreateWindow("Terrain Overview");
	glutDisplayFunc(displayOverview);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, size, 0.0, size);
	glMatrixMode(GL_MODELVIEW);
	
	std::cout << "Terrain Map Generator - Assignment 3\n";
	std::cout << "------------------------------------\n";
	std::cout << "W key: toggles wireframe mode between three options\n";
	std::cout << "Arrow keys: move the camera around the x and y axis\n";
	std::cout << "R key: reset and generate a new terrain\n";
	std::cout << "S key: toggle between quad and triangle rendering\n";
	std::cout << "C key: toggle between the first and second light source\n";
	std::cout << "J key: Move the light source to the left\n";
	std::cout << "L key: Move the light source upwards\n";
	std::cout << "K key: Move the light source to the right\n";
	std::cout << "I key: Move the light source downwards\n";
	std::cout << "F key: toggle fault and circles generation\n" << flush;
	
	glutMainLoop();	
	return(0);			
}



