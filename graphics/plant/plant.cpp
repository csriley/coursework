/*
 * plant.cpp
 * ---------
 * Contains function callbacks and bookkeeping used by glut.  Starter code
 * for Project 2.
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
#include <iostream>

#include "common.h"
#include "drawplant.h"

//#include "matrix.cpp"

/* GLOBAL VARAIBLES */
/* (storage is actually allocated here) */
int W=800;		/* window width */
int H=600;		/* window height */
int X_OFF = 10;	/* window x offset */
int Y_OFF = 10;	/* window y offset */

/* local function declarations */
void display(void);
void init(void);

/* Constants for specifying the 3 coordinate axes */
#define X_AXIS			0
#define Y_AXIS			1
#define Z_AXIS			2

double viewing_size = 15.0;
double distance = 2;
bool PERSPECTIVE = FALSE;
bool WIND = FALSE;
bool JUSTLEAF = FALSE;
int cur_depth = 3;

extern double dim;


void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(PERSPECTIVE) {
        glFrustum(-1 * viewing_size, viewing_size, -1 * viewing_size, viewing_size, distance, viewing_size + distance);
        glTranslatef(0.0f, 0.0f, -1 * distance/2);
    }
    else {
        glOrtho(-1 * viewing_size, viewing_size, -1 * viewing_size, viewing_size, -1 * viewing_size, viewing_size);
    }
 
}

int endPlant(int status) {
    printf("\nQuitting.\n");
    fflush(stdout);
    exit(status);
}

/* The last position of the mouse since the last callback */
int m_last_x, m_last_y;


void myMouseButton(int button, int state, int x, int y) {
    init();
	if (state == GLUT_DOWN) {
		m_last_x = x;
		m_last_y = y;
	}
}

double deg_total = 0;

void rotateCamera(double deg) {
    deg_total += deg;
    deg_total = (int)deg_total % 360;
    double rad = (deg_total)* PI / 180;
    gluLookAt(distance*cos(rad), 0, distance*sin(rad), 0,0,0,0,1,0);
}

void myMouseMotion(int x, int y) {
    init();
	double d_x, d_y;	/* The change in x and y since the last callback */
    
	d_x = x - m_last_x;
	d_y = y - m_last_y;
    
	m_last_x = x;
	m_last_y = y;
    
    d_x /= 2.0;
    
    rotateCamera(d_x);	/* y-axis rotation */
    
	/* Redraw the screen */
	glutPostRedisplay();
}

void zoomCheck() {
    double length = dim*pow(2, (double)cur_depth);
    if(length > viewing_size/2) {
        viewing_size *= 2.0;
    }
    
    if(length < viewing_size/4) {
        viewing_size /= 2.0;
    }
    
}

/* Handle user input */
void myKeyHandler(unsigned char ch, int x, int y) {
    init();
    
	switch(ch) {
		case '/':
			rotateCamera(5);
			break;
            
		case '?':
			rotateCamera(-5);
			break;
            
        case 'a':
            cur_depth += 1;
            fprintf(stderr, "Current L-System depth is %2d. \n", cur_depth);
            zoomCheck();
            break;
            
        case 's':
            cur_depth -= 1;
            if(cur_depth < 0 )
                cur_depth = 0;
            fprintf(stderr, "Current L-System depth is %2d. \n", cur_depth);
            zoomCheck();
            break;
        
        case 'p':
            PERSPECTIVE = !PERSPECTIVE;
            if(PERSPECTIVE) {
                fprintf(stderr, "Using perspective projection mode.\n");
            }
            else {
                fprintf(stderr, "Using orthographic projection mode.\n");
            }
            rotateCamera(0);
            break;
            
        case 'b':
            dim -= 0.01;
            if(dim <= 0) {
                dim = 0.1;
            }
            fprintf(stderr, "Zoom dimension is %.2f. \n", dim);
            break;
        
        case 'B':
            dim += 0.1;
            fprintf(stderr, "Zoom dimension is %.2f. \n", dim);
            break;
            
        case 'w':
            WIND = !WIND;
            if(WIND) {
                for(int i = 0; i < 50; i++) {
                    usleep(100000);
                    display();
                    init();
                }
            }
            WIND = !WIND;
            break;
            
        case 'L':
            JUSTLEAF = !JUSTLEAF;
            fprintf(stderr, "Displaying 1 leaf.\n");
            break;
            
        case 'g':
            viewing_size *= 6.0;
            for(int t = 0; t < 5; t++){
                for(int j = 0; j<40; j++) {
                    usleep(100000);
                    dim += 0.005;
                    display();
                    init();
                }
                dim -= 0.25;
                cur_depth++;
            }
            viewing_size /= 6.0;
            break;
            
        case 'q':
			/* Quit with exit code 0 */
			endPlant(0);
			break;
            
	}
    
	/*
	 * If control reaches here, the key press was recognized.  Refresh
	 * the screen, since most key presses change the display in some way.
	 */
    
    display();
	return;
}

void display() {
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_MAP1_VERTEX_3);
    
	/* See drawplant.c for the definition of this routine */
	drawPlant(cur_depth);


    glFlush();  /* Flush all executed OpenGL ops finish */

    /*
     * Since we are using double buffers, we need to call the swap
     * function every time we are done drawing.
     */
    glutSwapBuffers();
}


int main (int argc, char** argv) {
    glutInit(&argc,argv);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(X_OFF, Y_OFF);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("plant");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyHandler);
    
    glutMouseFunc(myMouseButton);
	glutMotionFunc(myMouseMotion);
    glutMainLoop();
    return 0;
}
/* end of plant.c */
