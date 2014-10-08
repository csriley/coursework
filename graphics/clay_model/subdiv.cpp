/*
 * subdiv.cpp
 * ----------
 * Contains function callbacks and bookkeeping for window management.
 *
 * Group Members: <FILL IN>
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <vector>
#include <cmath>
#include "common.h"
#include "drawing.h"
#include "data.h"

#define PI 3.14159265
#define SHINE1	128.0
#define SHINE2	70.0
#define SHINE3	10.0

/* GLOBAL VARAIBLES */
/* (storage is actually allocated here) */
int W=400;		/* window width */
int H=400;		/* window height */
int X_OFF = 10;	/* window x offset */
int Y_OFF = 10;	/* window y offset */

int DISPLAY_SIZE = 80;

/* The dimensions of the viewing frustum */
GLfloat fleft   = -40.0;
GLfloat fright  =  40.0;
GLfloat fbottom = -40.0;
GLfloat ftop    =  40.0;
GLfloat zNear   =  40.0;
GLfloat zFar    = -40.0;

/* Current array of control points, and vertex_index is how many points. */
int count = 0;	//number of vertical points
int divi = 10;	//number of horizontal subdivisions
double shininess = SHINE1;
double zoomFactor = 1.0;

/*2D vectors to keep up with points*/
std::vector< std::vector<double> > points_x (divi, std::vector<double> (30, 0)); 
std::vector< std::vector<double> > points_y (divi, std::vector<double> (30, 0)); 
std::vector< std::vector<double> > points_z (divi, std::vector<double> (30, 0)); 

/* Constants for keeping up with mouse location. */
int mouse_mode;
int m_last_x, m_last_y;
#define MOUSE_ROTATE_YX 0
#define MOUSE_ROTATE_YZ 1 
#define MOUSE_ZOOM 		2

/* Flags for key presses. */
bool ROTATED = 0;
bool WIRE = 0; 
bool BEGINNING = 1;
bool POINTS = 0;
bool GOURAD = 0;

/* local function declarations */
void init(void);
void display(void);
void myKeyHandler(unsigned char ch, int x, int y);
void myMouseButton(int button, int state, int x, int y);
void myMouseMotion(int x, int y);
void endSubdiv(int status);
void savePoint(double x, double y);
void makePoints();
void rotate();
void drawWireRotatedPoints();
void drawSolidRotatedPoints();
void drawPoints();
void verticalSubdivision();
void horizontalSubdivision();


int main (int argc, char** argv) {
  glutInit(&argc,argv);
  glutInitWindowSize(W, H);
  glutInitWindowPosition(X_OFF, Y_OFF);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("subdiv");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(myKeyHandler);
  glutMouseFunc(myMouseButton);
  glutMotionFunc(myMouseMotion);
  glutMainLoop();
  return 0;
}

void init() {
	std::vector< std::vector<double> > temp_x (divi, std::vector<double> (30, 0)); 
	std::vector< std::vector<double> > temp_y (divi, std::vector<double> (30, 0)); 
	std::vector< std::vector<double> > temp_z (divi, std::vector<double> (30, 0));
	points_x = temp_x;
	points_y = temp_y;
	points_z = temp_z;
	glClearColor(0.0, 0.0, 0.0, 0.0);  
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(fleft, fright, fbottom, ftop, -zNear, -zFar); 
}  
/*
bool equal(GLfloat* p1, GLfloat* p2) {
	return p1[0]*p1[0] == p2[0]*p2[0] && p1[1]*p1[1] == p2[1]*p2[1] && p1[2]*p1[2] == p2[2]*p2[2];
}

void normal(GLfloat* p1, GLfloat* p2, GLfloat* p3, GLfloat* result) {
 	
	GLfloat u[] = {p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2]};
	GLfloat v[] = {p3[0] - p2[0], p3[1] - p2[1], p3[2] - p2[2]};
	
	GLfloat x = u[1]*v[2] - u[2]*v[1];
	GLfloat y = u[2]*v[0] - u[0]*v[2];
	GLfloat z = u[0]*v[1] - u[1]*v[0];

	GLfloat norm = sqrt(x*x + y*y + z*z);
	printf("NORMAL INSIDE: %f, %f, %f\n", x, y,z);
	result[0] = x/norm;
	result[1] = y/norm;
	result[2] = z/norm;
	result[3] = 0;

	if(equal(p1, p2) || equal(p2, p3) || equal(p1, p3)) {
		result[3] = 1;
	}
} */

/*Sets up grayscale light and material properties.*/
void lighting() {
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

	//Lighting
    GLfloat position[] = {5.0 , 5.0, -1.0, 1.0};
    GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat diffuse[] = {0.8, 0.8, 0.8, 1.0};
    GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
    
    //Set lighting parameters
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    //Material
    GLfloat m_color[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat m_ambient[] = {0.0, 0.0, 0.0, 0.0};
    GLfloat m_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat m_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat m_emission[] = {0.2, 0.2, 0.2, 1.0};

    //Set material parameters
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, m_emission);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    glShadeModel(GL_SMOOTH); 
}

/* Display function, depending on flags. */
void display() {
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if(BEGINNING) {
		makePoints();
	}

	if(ROTATED & !POINTS) {
		lighting();
		if(WIRE) {
			drawWireRotatedPoints();
		}
		else {
			drawSolidRotatedPoints();
		}
	}

	if(POINTS) {
		drawPoints();
	}

    glFlush();  /* Flush all executed OpenGL ops finish */

    /*
     * Since we are using double buffers, we need to call the swap
     * function every time we are done drawing.
     */
    glutSwapBuffers();
}

void myKeyHandler(unsigned char ch, int x, int y) {
	switch(ch) {
		case 'q':
			endSubdiv(0);
			break;
		case 'w':
			BEGINNING = !BEGINNING;
			ROTATED = !ROTATED;
			rotate();
			break;
		case 'e':
			WIRE = !WIRE;
			if(WIRE) {
				printf("Displaying wire frame.\n");
			} 
			else {
				printf("Displaying solid frame.\n");
			}
			break;
		case 'r':
			POINTS = !POINTS;
			if(POINTS) {
				printf("Displaying points only.\n");
			}
			else {
				printf("No longer in points only mode.\n");
			}
			break;
		case 'a':
			verticalSubdivision();
			break;
		case 'b':
			horizontalSubdivision();
			break;
		case 'd':
			GOURAD = !GOURAD;
			if(GOURAD) {
				printf("Displaying Gourad Shading.\n");
			}
			else {
				printf("Displaying Phong Shading.\n");
			}
			break;
		case 's':
			if(shininess == SHINE1) {
				shininess = SHINE2;
			}
			else if(shininess == SHINE2) {
				shininess = SHINE3;
			}
			else if(shininess == SHINE3) {
				shininess = SHINE1;
			}
			printf("Shininess value is %f. \n", shininess);
			break;
		case 'z':
			BEGINNING = 1;
			ROTATED = 0;
			WIRE = 0;
			count = 0;
			divi = 3;
			init();
			printf("Re-entering 2D mode.\n");
			break;
		default:
			/* Unrecognized keypress */
			break;
	}

	glutPostRedisplay();

	return;
}

void myMouseButton(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		m_last_x = x;
		m_last_y = y;
		if (button == GLUT_LEFT_BUTTON) {
			// Add a point, if there is room
			if(!ROTATED && count < 30) {
				savePoint(x, y);
				count += 1;
				printf("x: %3d, y: %3d, i: %d\n", x, y, count);
			}
			else if (!ROTATED) {
				printf("ERROR: Maximum number of points exceeded. Right-click to remove points.\n");
			}
			else {
				mouse_mode = MOUSE_ROTATE_YX;
			}
		}

		if (button == GLUT_MIDDLE_BUTTON) {
			if(!ROTATED) {
				count -= 1;
			}
			else {
				mouse_mode = MOUSE_ROTATE_YZ;
			}
		} 
		else if (button == GLUT_RIGHT_BUTTON && ROTATED) {
			mouse_mode = MOUSE_ZOOM;
		}

		glutPostRedisplay();
	}
}

void myMouseMotion(int x, int y) {
	double d_x, d_y;	/* The change in x and y since the last callback */

	d_x = x - m_last_x;
	d_y = y - m_last_y;

	m_last_x = x;
	m_last_y = y;

	if (mouse_mode == MOUSE_ROTATE_YX) {
		/* scaling factors */
		d_x /= 2.0;
		d_y /= 2.0;

		glRotatef(d_x, 0.0, 1.0, 0.0);	/* y-axis rotation */
		glRotatef(-d_y, 1.0, 0.0, 0.0);	/* x-axis rotation */

	} else if (mouse_mode == MOUSE_ROTATE_YZ) {
		/* scaling factors */
		d_x /= 2.0;
		d_y /= 2.0;

		glRotatef(d_x, 0.0, 1.0, 0.0);	/* y-axis rotation */
		glRotatef(-d_y, 0.0, 0.0, 1.0);	/* z-axis rotation */

	} else if (mouse_mode == MOUSE_ZOOM) {
		d_y /= 100.0;

		zoomFactor += d_y;

		if (zoomFactor <= 0.0) {
			/* The zoom factor should be positive */
			zoomFactor = 0.001;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(fleft*zoomFactor, fright*zoomFactor, fbottom*zoomFactor, ftop*zoomFactor, -zNear, -zFar);  
	}

	/* Redraw the screen */
	glutPostRedisplay();
}

/* Logs the current point clicked in the x_ray and y_ray. */
void savePoint(double x, double y) {
	x = (x - W/2)/W * DISPLAY_SIZE;
	y = (H/2 - y)/H * DISPLAY_SIZE;

	if(x <= 1 && x >= -1) {
		x = 0;
	}

	points_x[0][count] = x;
	points_y[0][count] = y;
	points_z[0][count] = 0;
}

/* Displays the points that the user clicks and places the lines between them. */
void makePoints() {
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2f(0.0, -1 * DISPLAY_SIZE/2);
	glVertex2f(0.0, DISPLAY_SIZE/2);
	glEnd();

	for(int i = 0; i < count; i++) {
		glColor3f(1.0, 0.0, 0.0);
		glPointSize(5);
		glBegin(GL_POINTS);
		glVertex3f(points_x[0][i], points_y[0][i], points_z[0][i]);
		glEnd();
	}

	//Draw lines between points
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < count; i++) {
		glVertex3f(points_x[0][i], points_y[0][i], points_z[0][i]);
	}
	glEnd();
}

/* Displays the current points saved for points only mode. */
void drawPoints() {
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(5);
	glBegin(GL_POINTS);
	for(int i = 0; i < points_x.size(); i++) {
		for(int j = 0; j< points_x[0].size(); j++) {
			glVertex3f(points_x[i][j], points_y[i][j], points_z[i][j]);
		}
	}
	glEnd();
	
}

/* Rotates the 2D points into 3D with the given number of subdivisions. (default is 3)*/
void rotate() {
	double rotation_radius0 = 2*PI/divi;
	double rotation_radius = 0;
	for(int i = 1; i < divi; i++) {
		rotation_radius += rotation_radius0;
		for(int j = 0; j < count; j++) {
			points_x[i][j] = points_x[0][j] * cos(rotation_radius);
			points_y[i][j] = points_y[0][j];
			points_z[i][j] = points_x[0][j] * sin(rotation_radius);
		}
	}
}

/* Uses lines to draw between points for wire frame mode. */
void drawWireRotatedPoints() {
	glColor3f(0.0, 1.0, 0.0);
	for(int j = 0; j < count; j++) {
		glBegin(GL_LINE_LOOP);
		for(int i = 0; i < points_x.size(); i++) {
			glVertex3f(points_x[i][j], points_y[i][j], points_z[i][j]);	
		}
		glEnd();
	}

	for(int i = 0; i < points_x.size(); i++) {
		glBegin(GL_LINE_STRIP);
		for(int j = 0; j < count; j++) {
			glVertex3f(points_x[i][j], points_y[i][j], points_z[i][j]);
			//printf("ORIGINAL:: x: %f, y: %f, z: %f \n", points_x[i][j], points_y[i][j], points_z[i][j]);
		}
		glEnd();
	}
}

/* Uses quadraliterals (Gourad) or Triangles (Phong) to display the solid surface. */
void drawSolidRotatedPoints() {
	glColor3f(0.0, 1.0, 1.0);

	for(int i = 0; i < points_x.size(); i++) {
		int k = i + 1;
		if(i == points_x.size()-1) {
			k = 0;
		}
		for(int j = 0; j < count-1; j++) {
			GLfloat p1 [] = {points_x[i][j], points_y[i][j], points_z[i][j]};
			GLfloat p2 [] = {points_x[k][j], points_y[k][j], points_z[k][j]};
			GLfloat p3 [] = {points_x[k][j+1], points_y[k][j+1], points_z[k][j+1]};
			GLfloat p4 [] = {points_x[i][j+1], points_y[i][j+1], points_z[i][j+1]};
			GLfloat result[] = {0.0f, 0.0f, 0.0f, 0.0f};

			/*For Phong Shading, triangles are needed. Since the points are stored 
			as quadrilaterals, the 1, 2, 4 and 2, 3, 4 vertices can be divided into 
			two triangles. 
			*/
			if(!GOURAD) {
				glBegin(GL_TRIANGLES);

				//1,2,4
				GLfloat half1[] = {0,0,0};
				for(int l = 0; l < 3; l++) {
					half1[l] = (p1[l] + p2[l])/2.0;
				}	

				GLfloat half2[] = {0,0,0};
				for(int l = 0; l < 3; l++) {
					half2[l] = (p2[l] + p4[l])/2.0;
				}

				GLfloat half3[] = {0,0,0};
				for(int l = 0; l < 3; l++) {
					half3[l] = (p4[l] + p1[l])/2.0;
				}
				
				glVertex3fv(p1);
				glVertex3fv(half1);
				glVertex3fv(half3);
				
				glVertex3fv(half1);
				glVertex3fv(p2);
				glVertex3fv(half2);

				
				glVertex3fv(half2);
				glVertex3fv(p4);
				glVertex3fv(half3);

				
				glVertex3fv(half1);
				glVertex3fv(half2);
				glVertex3fv(half3);
				
				//2,3,4

				GLfloat half12[] = {0,0,0};
				for(int l = 0; l < 3; l++) {
					half12[l] = (p2[l] + p3[l])/2.0;
				}

				GLfloat half22[] = {0,0,0};
				for(int l = 0; l < 3; l++) {
					half22[l] = (p3[l] + p4[l])/2.0;
				}

				GLfloat half32[] = {0,0,0};
				for(int l = 0; l < 3; l++) {
					half32[l] = (p4[l] + p2[l])/2.0;
				}
				
				glVertex3fv(p2);
				glVertex3fv(half12);
				glVertex3fv(half32);

				glVertex3fv(p3);
				glVertex3fv(half22);
				glVertex3fv(half12);

				glVertex3fv(half22);
				glVertex3fv(p4);
				glVertex3fv(half32);

				glVertex3fv(half12);
				glVertex3fv(half22);
				glVertex3fv(half32);

				glEnd();
			}
			else {
				//Gouraud Shading with Quadrilaterals
				glBegin(GL_QUADS);
	
				glVertex3fv(p1);
		
				glVertex3fv(p2);

				glVertex3fv(p3);

				glVertex3fv(p4);

				glEnd();
			}
		}
	}
}

/* Applies subdivision along the y-axis. */
void verticalSubdivision() {

	count = count * 2 - 1;
	std::vector <double> temp_x (count, 0);
	std::vector <double> temp_y (count, 0);
	std::vector <double> temp_z (count, 0);
	
	for(int i = 0; i < points_x.size(); i ++) {
		temp_x[0] = points_x[i][0];
		temp_x[1] = 1/8.0 * (4 * points_x[i][0] + 4 * points_x[i][1]);

		temp_y[0] = points_y[i][0];
		temp_y[1] = 1/8.0 * (4 * points_y[i][0] + 4 * points_y[i][1]);

		temp_z[0] = points_z[i][0];
		temp_z[1] = 1/8.0 * (4 * points_z[i][0] + 4 * points_z[i][1]);
		for(int j = 2; j < count-1; j+=2) {
			temp_x[j] = 1/8.0 * (points_x[i][j/2 - 1] + 6 * points_x[i][j/2] + points_x[i][j/2+1]);
			temp_x[j+1] = 1/8.0 * (4 * points_x[i][j/2] + 4 * points_x[i][j/2+1]); 

			temp_y[j] = 1/8.0 * (points_y[i][j/2 - 1] + 6 * points_y[i][j/2] + points_y[i][j/2+1]);
			temp_y[j+1] = 1/8.0 * (4 * points_y[i][j/2] + 4 * points_y[i][j/2+1]); 

			temp_z[j] = 1/8.0 * (points_z[i][j/2 - 1] + 6 * points_z[i][j/2] + points_z[i][j/2+1]);
			temp_z[j+1] = 1/8.0 * (4 * points_z[i][j/2] + 4 * points_z[i][j/2+1]); 	
		}
		temp_x[count - 1] = points_x[i][count/2];
		temp_y[count - 1] = points_y[i][count/2];
		temp_z[count - 1] = points_z[i][count/2];

		points_x[i] = std::vector <double> (temp_x);
		points_y[i] = std::vector <double> (temp_y);
		points_z[i] = std::vector <double> (temp_z);
	}
}

/* Applies subdivision along the x-axis. */
void horizontalSubdivision() {
	divi = divi * 2;

	std::vector < std::vector <double> > temp_x (divi, std::vector <double> (count, 0));
	std::vector < std::vector <double> > temp_y (divi, std::vector <double> (count, 0));
	std::vector < std::vector <double> > temp_z (divi, std::vector <double> (count, 0)); 

	for(int j = 0; j < count; j++) {
		temp_x[0][j] = 1/8.0 * (points_x[divi/2 - 1][j] + 6 * points_x[0][j] + points_x[1][j]);
		temp_x[1][j] = 1/8.0 * (4 * points_x[0][j] + 4 * points_x[1][j]);

		temp_y[0][j] = 1/8.0 * (points_y[divi/2 - 1][j] + 6 * points_y[0][j] + points_y[1][j]);
		temp_y[1][j] = 1/8.0 * (4 * points_y[0][j] + 4 * points_y[1][j]);

		temp_z[0][j] = 1/8.0 * (points_z[divi/2 - 1][j] + 6 * points_z[0][j] + points_z[1][j]);
		temp_z[1][j] = 1/8.0 * (4 * points_z[0][j] + 4 * points_z[1][j]);

		for(int i = 2; i < divi-1; i+=2) {
			int l = i/2 + 1;
			int k = i/2;
			if(i == divi - 2) {
				l = 0;
			}
			if(i == divi - 1) {
				k = 0; 
				l = 1;
			}
			temp_x[i][j] = 1/8.0 * (points_x[i/2 - 1][j] + 6 * points_x[k][j] + points_x[l][j]);
			temp_x[i+1][j] = 1/8.0 * (4 * points_x[k][j] + 4 * points_x[l][j]); 

			temp_y[i][j] = 1/8.0 * (points_y[i/2 - 1][j] + 6 * points_y[k][j] + points_y[l][j]);
			temp_y[i+1][j] = 1/8.0 * (4 * points_y[k][j] + 4 * points_y[l][j]); 

			temp_z[i][j] = 1/8.0 * (points_z[i/2 - 1][j] + 6 * points_z[k][j] + points_z[l][j]);
			temp_z[i+1][j] = 1/8.0 * (4 * points_z[k][j] + 4 * points_z[l][j]); 
		}
	}

	points_x = temp_x;
	points_y = temp_y;
	points_z = temp_z;
}

void endSubdiv(int status) {
  printf("\nQuitting subdivision program.\n\n");
  fflush(stdout);

  exit(status);
}


/* end of subdiv.cpp */
