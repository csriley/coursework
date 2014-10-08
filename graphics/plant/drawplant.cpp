/*
 * drawplant.cpp
 * -------------
 * Contains the drawing routine and related helper functions for the
 * L-system fractal plant.  Starter code for Project 2.
 *
 * Group Members: Crystal Riley
 */

#ifdef _WIN32
#include <windows.h>
#endif
#include <cmath>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <list>

#include "common.h"
#include "drawplant.h"
#include "readppm.h"

using namespace std;

extern bool WIND;
extern bool JUSTLEAF;
double dim = 0.5;

/* Takes a 2D matrix in row-major order, and loads the 3D matrix which
 does the same trasformation into the OpenGL MODELVIEW matrix, in
 column-major order. */
void load2DMatrix(
                  GLfloat m00, GLfloat m01, GLfloat m02,
                  GLfloat m10, GLfloat m11, GLfloat m12,
                  GLfloat m20, GLfloat m21, GLfloat m22) {
    
	GLfloat M3D [16];  /* three dimensional matrix doing same transform */
    
	M3D[0] = m00;  M3D[1] = m10; M3D[2] = 0.0; M3D[3] = m20;
	M3D[4] = m01;  M3D[5] = m11; M3D[6] = 0.0; M3D[7] = m21;
	M3D[8] = 0.0;  M3D[9] = 0.0; M3D[10] = 1.0; M3D[11] = 0.0;
	M3D[12] = m02; M3D[13] = m12; M3D[14] = 0.0; M3D[15] = m22;
    
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(M3D);
}

/* Takes a 3D matrix in row-major order, and loads the 3D matrix which
 does the same trasformation into the OpenGL MODELVIEW matrix, in
 column-major order. */
void load3DMatrix(
                  GLfloat m00, GLfloat m01, GLfloat m02, GLfloat m03,
                  GLfloat m10, GLfloat m11, GLfloat m12, GLfloat m13,
                  GLfloat m20, GLfloat m21, GLfloat m22, GLfloat m23,
                  GLfloat m30, GLfloat m31, GLfloat m32, GLfloat m33) {
    
    GLfloat M3D [16];
    
    M3D[0] = m00;  M3D[1] = m10; M3D[2] = m20; M3D[3] = m30;
	M3D[4] = m01;  M3D[5] = m11; M3D[6] = m21; M3D[7] = m31;
	M3D[8] = m02;  M3D[9] = m12; M3D[10] = m22; M3D[11] = m32;
	M3D[12] = m03; M3D[13] = m13; M3D[14] = m23; M3D[15] = m33;
    
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(M3D);
}



class Matrix {
  public:
    GLfloat matrix[4][4];
    
    Matrix() {
        
        matrix[0][0] = 1.0f;
        matrix[0][1] = 0.0f;
        matrix[0][2] = 0.0f;
        matrix[0][3] = 0.0f;
        
        matrix[1][0] = 0.0f;
        matrix[1][1] = 1.0f;
        matrix[1][2] = 0.0f;
        matrix[1][3] = 0.0f;
        
        matrix[2][0] = 0.0f;
        matrix[2][1] = 0.0f;
        matrix[2][2] = 1.0f;
        matrix[2][3] = 0.0f;
        
        matrix[3][0] = 0.0f;
        matrix[3][1] = 0.0f;
        matrix[3][2] = 0.0f;
        matrix[3][3] = 1.0f;
        
     /*
        {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };*/
    }
    
    /* Returns this times mat */
    Matrix multiply(Matrix mat) {
        Matrix ret = Matrix();
        for(int i = 0; i < mat.len(); i++) {
            for(int j = 0; j < mat.len(); j++) {
                GLfloat sum = 0.0f;
                for(int k = 0; k < mat.len(); k++) {
                    sum += matrix[i][k] * mat.matrix[k][j];
                }
                ret.matrix[i][j] = sum;
            }
            
        }
        return ret;
    }
    
    Matrix add(Matrix mat) {
        Matrix ret = Matrix();
        for(int i = 0; i < mat.len(); i++) {
            for(int j = 0; j < mat.len(); j++) {
                ret.matrix[i][j] = matrix[i][j] + mat.matrix[i][j];
            }
        }
        return ret;
    }
    
    Matrix fill(GLfloat scalar) {
        Matrix ret = Matrix();
        for(int i = 0; i < ret.len(); i++) {
            for(int j = 0; j< ret.len(); j++) {
                ret.matrix[i][j] = scalar;
            }
        }
        return ret;
    }
    
    int len() {
        return sizeof(matrix[0])/sizeof(GLfloat);
    }
    
   /* void print() {
        for(int i = 0; i < this->len(); i++) {
            for(int j = 0; j < this->len(); j++) {
                fprintf(stderr, "%.1f ", matrix[i][j]);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
    }*/
    
    GLfloat normalize(GLfloat x, GLfloat y, GLfloat z) {
        GLfloat norm = sqrt(x*x + y*y + z*z);
        return norm;
    }
    
    Matrix rotate(GLfloat degrees, GLfloat x, GLfloat y, GLfloat z) {
        GLfloat norm = normalize(x, y, z);
        x = x/norm;
        y = y/norm;
        z = z/norm;
        degrees = degrees * PI / 180;
        
        Matrix id = Matrix();
        Matrix cross = id.crossVector(x, y, z);
        Matrix tensor = id.tensorVector(x, y, z);
        Matrix cosine = id.s_mult(cos(degrees));
        Matrix sine = cross.s_mult(sin(degrees));
        Matrix minus_cosine = tensor.s_mult((1 - cos(degrees)));
        
        Matrix rot = cosine.add(sine);
        rot = rot.add(minus_cosine);
        
        rot.matrix[3][3] = 1.0f;
        return rot;
        
    }
    
    Matrix s_mult(GLfloat scalar) {
        Matrix ret = Matrix();
        for(int i = 0; i < ret.len(); i++) {
            for(int j = 0; j < ret.len(); j++) {
                ret.matrix[i][j] = matrix[i][j] * scalar;
            }
        }
        
        return ret;
    }
    
    Matrix crossVector(GLfloat x, GLfloat y, GLfloat z) {
        Matrix id = Matrix();
        Matrix cross = id.s_mult(0.0f);
        cross.matrix[0][1] = -z;
        cross.matrix[0][2] = y;
        cross.matrix[1][0] = z;
        cross.matrix[1][2] = -x;
        cross.matrix[2][0] = -y;
        cross.matrix[2][1] = x;
        cross.matrix[3][3] = 1.0f;
        return cross;
    }
    
    Matrix tensorVector(GLfloat x, GLfloat y, GLfloat z) {
        Matrix tensor = Matrix();
        tensor.matrix[0][0] = x*x;
        tensor.matrix[0][1] = x*y;
        tensor.matrix[0][2] = x*z;
        tensor.matrix[1][0] = x*y;
        tensor.matrix[1][1] = y*y;
        tensor.matrix[1][2] = y*z;
        tensor.matrix[2][0] = x*z;
        tensor.matrix[2][1] = y*z;
        tensor.matrix[2][2] = z*z;
        return tensor;
    }
    
    Matrix scale(GLfloat x, GLfloat y, GLfloat z) {
        Matrix id = Matrix();
        id.matrix[0][0] = x;
        id.matrix[1][1] = y;
        id.matrix[2][2] = z;
        return id;
    }
    
    Matrix translate(GLfloat x, GLfloat y, GLfloat z) {
        Matrix trans = Matrix();
        trans.matrix[0][3] = x;
        trans.matrix[1][3] = y;
        trans.matrix[2][3] = z;
        return trans;
    }
    
    Matrix copy() {
        Matrix temp = Matrix();
        for(int i = 0; i<this->len(); i++) {
            for(int j = 0; j <this->len(); j++) {
                temp.matrix[i][j] = this->matrix[i][j];
            }
        }
        return temp;
    }
    
    void load() {
        load3DMatrix(
                     this->matrix[0][0], this->matrix[0][1],this->matrix[0][2],this->matrix[0][3],
                     this->matrix[1][0], this->matrix[1][1],this->matrix[1][2],this->matrix[1][3],
                     this->matrix[2][0], this->matrix[2][1],this->matrix[2][2],this->matrix[2][3],
                     this->matrix[3][0], this->matrix[3][1],this->matrix[3][2],this->matrix[3][3]);
    }
    
    void writeOver(double a00, double a01, double a02, double a03, double a04, double a05, double a06, double a07, double a08, double a09, double a10, double a11, double a12, double a13, double a14, double a15) {
        this->matrix[0][0] = a00;
        this->matrix[0][1] = a01;
        this->matrix[0][2] = a02;
        this->matrix[0][3] = a03;
        
        this->matrix[1][0] = a04;
        this->matrix[1][1] = a05;
        this->matrix[1][2] = a06;
        this->matrix[1][3] = a07;
        
        this->matrix[2][0] = a08;
        this->matrix[2][1] = a09;
        this->matrix[2][2] = a10;
        this->matrix[2][3] = a11;
        
        this->matrix[3][0] = a12;
        this->matrix[3][1] = a13;
        this->matrix[3][2] = a14;
        this->matrix[3][3] = a15;
        
    }
};


list<Matrix> L;

void push(Matrix mat) {
    L.push_back(mat);
}

Matrix pop() {
    Matrix ret = L.back();
    L.pop_back();
    return ret;
}

void drawBerries() {
    for(double i = 0; i < dim/2.0; i+= dim/5.0) {
        for(double j = i; j<dim/2.0; j+= dim/5.0) {
            for(double k =j; k<dim/2.0; k += dim/5.0) {
                
                glColor3f(0.5f * k * 10, 0.0f, 1.0f * j * 100 + 0.7);
                glPushMatrix();
                glTranslatef(i ,j,k);
                glutSolidSphere(dim/5.0, 10,10);
                glPopMatrix();
            }
        }
    }
}


void drawBase() {
    glPushMatrix();
    glTranslatef(0.0f, -dim, 0.0f);
    for(int x = 0; x < 4; x++ ) {
        
        glBegin(GL_TRIANGLES);
        glColor3f(0.0, 0.3, 0.0);
        glVertex3f(-0.25f, 0.0f, 0.0f);
        
        glColor3f(0.0, 0.5, 0.0);
        glVertex3f(0.0f, 1.5f, 0.0f);
        
        glColor3f(0.0, 0.3, 0.0);
        glVertex3f(0.25f, 0.0f, 0.0f);
        glEnd();
        
        glBegin(GL_TRIANGLES);
        
        glColor3f(0.0, 0.3, 0.0);
        glVertex3f(0.0f, 0.0f, 0.0f);
        
        glColor3f(0.0, 0.5, 0.0);
        glVertex3f(0.25f, 1.5f, 0.0f);
        
        glColor3f(0.0, 0.3, 0.0);
        glVertex3f(0.5f, 0.0f, 0.0f);
        glEnd();
        
        glBegin(GL_TRIANGLES);
        glColor3f(0.0, 0.3, 0.0);
        glVertex3f(0.25f, 0.0f, 0.0f);
        
        glColor3f(0.0, 0.5, 0.0);
        glVertex3f(0.5f, 1.5f, 0.0f);
        
        glColor3f(0.0, 0.3, 0.0);
        glVertex3f(0.75f, 0.0f, 0.0f);
        glEnd();
        
        glRotatef(90, 0.0, 1.0f, 0.0f);
    }
    glPopMatrix();
}

void drawLeaf(){
    
    dim *=2;
    if(JUSTLEAF) {
        dim = 5;
    }
    glBegin(GL_POLYGON);
    
    double a_x = dim*2;
    double a_y = dim*2;
    double a_z = dim;
    
    double b_x = dim*2;
    double b_y = 0;
    double b_z = dim*2;
    
    double c_x = 0;
    double c_y = 0;
    double c_z = dim;
    
    double d_x = 0;
    double d_y = 0;
    double d_z = 0;

    
    for(double t = 0; t< 1; t+=0.1) {
        double p_x = (1 - t)*(1-t) * a_x + (1-t)*t*b_x + t*t* c_x;
        double p_y = (1 - t)*(1-t) * a_y + (1-t)*t*b_y + t*t * c_y;
        double p_z = (1 - t)*(1-t) * a_z + (1-t)*t*b_z + t*t * c_z;
        
        glColor3f(0.0, 0.8 *(t + 0.1), 0.0);
        glVertex3f(p_x,p_y,p_z);
    }
    
    for(double t = 0; t< 1; t+=0.1) {
        double p_x = (1 - t)*(1-t) * c_x + (1-t)*t*d_x + t*t* a_x;
        double p_y = (1 - t)*(1-t) * c_y + (1-t)*t*d_y + t*t * a_y;
        double p_z = (1 - t)*(1-t) * c_z + (1-t)*t*d_z + t*t * a_z;
        
        glColor3f(0.0, 0.8 * (1 - t), 0.0);
        glVertex3f(p_x,p_y,p_z);
    }
    
    
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();
    if(JUSTLEAF) {
        dim = 0.5;
    }
    dim/=2.0;
    
}

void drawKnot() {
    
    glColor3f(0.1f, 0.3f, 0.0f);
    glTranslatef(dim * 0.5, 0, dim*0.5);
    glutSolidSphere(dim, 10, 10);
    glTranslatef(-1 * dim*0.5, 0, -1 * dim*0.5);
}

void drawBranch(double factor) {
	/* ADD YOUR CODE to make the 2D branch a 3D extrusion */
    
    double length = dim * pow((double)2, (double)factor);
    
	glBegin(GL_QUAD_STRIP);
    glColor3f(0.0f, 0.2f, 0.0f);
    
	glVertex3f(dim,0.0, 0.0);
	glVertex3f(dim,length,0.0);
    
    glColor3f(0.0f, 0.5f, 0.0f);
    
	glVertex3f(0.0,length,0.0);
	glVertex3f(0.0,0.0,0.0);
    
    glColor3f(0.0f, 0.8f, 0.0f);
    
	glVertex3f(0.0,0.0,dim);
    glVertex3f(0.0,length, dim);
    
    glColor3f(0.0f, 0.5f, 0.0f);
    
    glVertex3f(dim, length, dim);
    glVertex3f(dim, 0.0f, dim);
    
    glColor3f(0.0f, 0.2f, 0.0f);
    
    glVertex3f(dim, 0.0f, 0.0f);
    glVertex3f(dim, length, 0.0f);
    
    glColor3f(0.0f, 0.5f, 0.0f);
    
    glVertex3f(dim, length, 0.0f);
    glVertex3f(0.0,length,0.0);
    glVertex3f(0.0,length, dim);
    glVertex3f(dim, length, dim);
    
    glEnd();
    
    
}

void recurr(int depth, Matrix cur) {
    
    Matrix rot = Matrix();
    Matrix trans = Matrix();
    Matrix scal = Matrix();
    
    if(depth <= 0) {
        cur.load();
        drawLeaf();
        //glTranslatef(0.0f, -dim, 0.0f);
        trans = trans.translate(0.0f, -dim, 0.0f);
        cur = cur.multiply(trans);
        cur.load();
        drawBerries();
        return;
    }
    
    
    double x_Tilt = 3.0;
    double y_Tilt = 3.0;
    if(WIND) {
        y_Tilt = (double)(rand() % 30)/10.0;
        x_Tilt = (double)(rand() % 30)/10.0;
    }
    
    double length = dim*pow(2, (double)depth);
    
    //glScalef(0.9, 0.9, 0.9)
    scal = scal.scale(0.9, 0.9, 0.9);
    cur = cur.multiply(scal);
    cur.load();
    
    drawKnot();
    
    drawBranch(depth); //F
    
    //glTranslatef(0.0f, length, 0.0f);
    trans = trans.translate(0.0f, length, 0.0f);
    cur = cur.multiply(trans);
    
    push(cur);
    push(cur);
    
    //glRotatef(330, 0.0f, 0.0f, -5.0f);
    rot = rot.rotate(330, 0.0f, 0.0f, -1.0f);
    cur = cur.multiply(rot);
    
    recurr(depth - 1, cur);
    cur = pop();
    cur.load();

    //glRotatef(30, x_Tilt, 0.0f, 0.0f);
    rot = rot.rotate(330, x_Tilt, 0.0f, 0.0f);
    cur = cur.multiply(rot);
    
    recurr(depth - 1, cur);
    cur = pop();
    cur.load();
    
    //glRotatef(30, 0.0f, 0.0f, 1.0f);
    rot = rot.rotate(30, 0.0f, 0.0f, 1.0f);
    cur = cur.multiply(rot);
    cur.load();
    
    drawBranch(depth);
    
    //glTranslatef(0.0f, length/2, 0.0f);
    trans = trans.translate(0.0f, length/2, 0.0f);
    cur = cur.multiply(trans);
    
    push(cur);
    
    //glRotatef(30, x_Tilt, y_Tilt, 1.0f);
    rot = rot.rotate(330, x_Tilt, y_Tilt, 1.0f);
    cur = cur.multiply(rot);
    cur.load();
    
    drawBranch(depth);
    
    //glTranslatef(0.0f, length/2, 0.0f);
    trans = trans.translate(0.0f, length/2, 0.0f);
    cur = cur.multiply(trans);
    
    recurr(depth - 1, cur);
    cur = pop();
    cur.load();
    
    //glRotatef(330, 0.0f, 0.0f, 1.0f);
    rot = rot.rotate(330, 0.0f, 0.0f, 1.0f);
    cur = cur.multiply(rot);
    
    recurr(depth - 1, cur);
    
}

/* Here for testing purposes only */

void recurr2(int depth) {
    //(X → F-[[X]+X]+F[+FX]-X)
    
    if(depth <= 0) {
        drawLeaf();
        glTranslatef(0.0f, -dim, 0.0f);
        drawBerries();
        return;
    }
    
    double x_Tilt = 3.0;
    double y_Tilt = 3.0;
    if(WIND) {
        y_Tilt = (double)(rand() % 30)/10.0;
        x_Tilt = (double)(rand() % 30)/10.0;
    }
    
    double length = dim*pow(2, (double)depth);
    
    glScalef(0.9, 0.9, 0.9);
    
    drawKnot();
    
    drawBranch(depth); //F
    glTranslatef(0.0f, length, 0.0f);
    
    glPushMatrix();
    glPushMatrix();
    
    glRotatef(330, 0.0f, 0.0f, -5.0f);
    recurr2(depth - 1);//X
    glPopMatrix();
    
    glRotatef(30, x_Tilt, 0.0f, 0.0f);
    recurr2(depth - 1); //X]
    glPopMatrix();
    
    glRotatef(30, 0.0f, 0.0f, 1.0f);
    drawBranch(depth); //F
    glTranslatef(0.0f, length/2, 0.0f);
    
    glPushMatrix();
    glRotatef(30, x_Tilt, y_Tilt, 1.0f);
    drawBranch(depth); //F
    glTranslatef(0.0f, length/2, 0.0f);
    
    recurr2(depth - 1); //X]
    glPopMatrix();
    
    glRotatef(330, 0.0f, 0.0f, 1.0f);
    recurr2(depth - 1); //X
    
    
}

/*
 * Draws the plant.
 *
 * ADD YOUR CODE and modify the function to take an L-system depth and
 * any other necessary arguments.
 */
void drawPlant(int depth) {
    
    if(JUSTLEAF) {
        drawLeaf();
    }
    else {
        Matrix temp = Matrix();
        temp.load();
        drawBase();
        recurr(depth, temp);
    }
}

/* end of drawplant.c */
