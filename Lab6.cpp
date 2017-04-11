/*
 * Authors: Jacob Leonard, Tyler Compton, Cecilia La Place
 * Lab 6: Textures
 */
//Create room (box)
//Put camera inside
//Add textures (whatever file type) inside walls, different for wall ceiling an floor
//Walls - do not do |\| instead do squares |\|\|\| use floor function change coordinates
// creating a triangulated plane

#include <vector>
#include "imathvec.h"
#include "gl/glut.h"
#include<iostream>
#include<fstream>
#include<string>

#define PI 3.1415926
using namespace std;
using namespace Imath;

int width = 1200;
int height = 600;
float ratio = 1.0;

// controling parameters
int mouse_button;
int mouse_x = 0;
int mouse_y = 0;
float scale = 0.3;
float x_angle = 0.0;
float y_angle = 0.0;

float numPlanes = 4;
float PLANE_SIZE = 10;

// mesh
typedef Vec3<float> Vec3f;
typedef Vec2<float> Vec2f;

// global moving
float angle = 0;
Vec3f moving_position = Vec3f(0, 0, 0);
Vec3f center_position = Vec3f(0, 0, 0);

// Update
/*struct Mesh {
	// vertex
	vector<Vec3f> dot_vertex;
	vector<Vec3f> dot_normalPerFace;
	vector<Vec3f> dot_normalPerVertex;
	vector<Vec2f> dot_texture;
	// faces
	vector<int> face_index_vertex;
	vector<int> face_index_normalPerFace;
	vector<int> face_index_normalPerVertex;
	vector<int> face_index_texture;
};

Mesh* mesh1;
*/
GLuint displayPlane;

// normal per face
/*void calculateNormalPerFace(Mesh* m) {
	Vec3<float> v1, v2, v3, v4, v5;
	for (int i = 0; i < m->face_index_vertex.size(); i += 3) {
		v1 = m->dot_vertex[m->face_index_vertex[i]];
		v2 = m->dot_vertex[m->face_index_vertex[i + 1]];
		v3 = m->dot_vertex[m->face_index_vertex[i + 2]];
		v4 = (v2 - v1);
		v5 = (v3 - v1);
		v4 = v4.cross(v5);
		v4.normalize();
		m->dot_normalPerFace.push_back(v4);
		int pos = m->dot_normalPerFace.size() - 1;
		// same normal for all vertex in this face
		m->face_index_normalPerFace.push_back(pos);
		m->face_index_normalPerFace.push_back(pos);
		m->face_index_normalPerFace.push_back(pos);
	}
}*/

// calculate normal per vertex
/*
void calculateNormalPerVertex(Mesh* m) {
	m->dot_normalPerVertex.clear();
	m->face_index_normalPerVertex.clear();
	Vec3<float> suma; suma.x = 0; suma.y = 0; suma.z = 0;
	//initialize
	for (unsigned int val = 0; val < m->dot_vertex.size(); val++) {
		m->dot_normalPerVertex.push_back(suma);
	}
	// calculate sum for vertex
	for (long pos = 0; pos < m->face_index_vertex.size(); pos++) {
		m->dot_normalPerVertex[m->face_index_vertex[pos]] +=
			m->dot_normalPerFace[m->face_index_normalPerFace[pos]];
	}
	// normalize for vertex 
	for (unsigned int val = 0; val < m->dot_normalPerVertex.size(); val++) {
		m->dot_normalPerVertex[val] = m->dot_normalPerVertex[val].normalize();
	}
	//normalVertexIndex is the same that vertexIndex
	for (unsigned int pos = 0; pos < m->face_index_vertex.size(); pos++) {
		m->face_index_normalPerVertex.push_back(m->face_index_vertex[pos]);
	}
}
*/
// creating a triangulated plane
/*Mesh* createPlane(int arena_width, int arena_depth, int arena_cell) {
	Mesh *me = new Mesh;
	int n = arena_width / arena_cell;
	int m = arena_depth / arena_cell;
	// vertices
	for (int i = 0; i<n; i++) {
		for (int j = 0; j < m; j++) {
			me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, 0.0, j*arena_cell));
		}
	}
	// faces
	for (int i = 0; i<(n*m) - m; i++) {
		if ((i + 1) % n == 0) continue;
		me->face_index_vertex.push_back(i); me->face_index_vertex.push_back(i + 1);
		me->face_index_vertex.push_back(i + n);
		me->face_index_vertex.push_back(i + 1); me->face_index_vertex.push_back(i + n + 1);
		me->face_index_vertex.push_back(i + n);
	}
	return me;
}*/

#include <windows.h>
#include <wingdi.h>

GLuint textureArray[3];

// Load a DIB/BMP file from disk.
GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info) {
	FILE *fp;      // open file pointer
	GLubyte * bits; // bitmap pixel bits
	int bitsize;   // Size of bitmap
	int infosize;  // Size of header information
	BITMAPFILEHEADER header; // File header
							 // try opening the file; use "rb" mode to read this *binary* file.
	if ((fp = fopen(filename, "rb")) == NULL)
		return (NULL);
	// read the file header and any following bitmap information.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		// Couldn't read the file header - return NULL.
		fclose(fp);
		return (NULL);
	}
	// Check for BM reversed.
	if (header.bfType != 'MB') {
		// not a bitmap file - return NULL.
		fclose(fp);
		return (NULL);
	}
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
		// couldn't allocate memory for bitmap info - return NULL.
		fclose(fp);
		return (NULL);
	}
	if (fread(*info, 1, infosize, fp) < infosize) {
		// Couldn't read the bitmap header - return NULL.
		free(*info);
		fclose(fp);
		return (NULL);
	}
	// Now that we have all the header info read in, allocate memory for the bitmap and read *it* in.
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth*(*info)->bmiHeader.biBitCount + 7) / 8 * abs((*info)->bmiHeader.biHeight);
	if ((bits = (GLubyte *)malloc(bitsize)) == NULL) {
		// Couldn't allocate memory - return NULL!
		free(*info);
		fclose(fp);
		return (NULL);
	}
	if (fread(bits, 1, bitsize, fp) < bitsize) {
		// couldn't read bitmap - free memory and return NULL!
		free(*info);
		free(bits);
		fclose(fp);
		return (NULL);
	}
	// OK, everything went fine - return the allocated bitmap.
	fclose(fp);
	return (bits);
}


void bmpTexture(UINT textureArray[], LPSTR file, int n) {
	BITMAPINFO *bitmapInfo; // Bitmap information
	GLubyte    *bitmapBits; // Bitmap data
	if (!file) {
		cout << "texture file not found!" << endl;
		return;
	}
	bitmapBits = LoadDIBitmap(file, &bitmapInfo);
	glGenTextures(1, &textureArray[n]);
	glBindTexture(GL_TEXTURE_2D, textureArray[n]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // must set to 1 for compact data
										   // glTexImage2D Whith size and minification
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bitmapInfo->bmiHeader.biWidth, bitmapInfo->bmiHeader.biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmapBits);
}

GLuint createPlane() {
	GLuint listID = glGenLists(1);

	glNewList(listID, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, textureArray[0]);
	
	glBegin(GL_TRIANGLES);

	glTexCoord2d(0, 0);
	glVertex3d(0, 0, 0);
	glTexCoord2d(1, 0);
	glVertex3d(10, 0, 0);
	glTexCoord2d(0, 1);
	glVertex3d(0, 0, 10);

	glTexCoord2d(1, 1);
	glVertex3d(10, 0, 10);
	glTexCoord2d(1, 0);
	glVertex3d(10, 0, 0);
	glTexCoord2d(0, 1);
	glVertex3d(0, 0, 10);

	glEnd();

	glEndList();

	return listID;

}

void drawMultiPlane(int width, int height) {
	glPushMatrix();

	for (int x = 0; x<width; x++) {
		for (int y = 0; y<height; y++) {
			glCallList(displayPlane);
			glTranslated(PLANE_SIZE, 0, 0);
		}
		glTranslated(-PLANE_SIZE*width, 0, PLANE_SIZE);
	}

	glPopMatrix();
}

void drawHouse() {
	float ROOM_SIZE = PLANE_SIZE*numPlanes;
	glTranslated(-ROOM_SIZE / 2, 0, -ROOM_SIZE / 2);

	// draw floor
	drawMultiPlane(10, 10);

	// draw ceiling
	glPushMatrix();
	glTranslated(0, ROOM_SIZE, 0);
	drawMultiPlane(10, 10);
	glPopMatrix();

	// Draw back wall
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	drawMultiPlane(10, 10);
	glPopMatrix();

	// Draw forward wall
	glPushMatrix();
	glTranslated(0, 0, ROOM_SIZE);
	glRotated(90, 1, 0, 0);
	drawMultiPlane(10, 10);
	glPopMatrix();

	// Draw left wall
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	drawMultiPlane(10, 10);
	glPopMatrix();

	// Draw right wall
	glPushMatrix();
	glTranslated(ROOM_SIZE, 0, 0);
	glRotated(90, 0, 0, 1);
	drawMultiPlane(10, 10);
	glPopMatrix();

}

// draw
/*GLuint meshToDisplayList(Mesh* m, int id) {
	GLuint listID = glGenLists(id);
	glNewList(listID, GL_COMPILE);
	glBegin(GL_TRIANGLES);

	for (unsigned int i = 0; i < m->face_index_vertex.size(); i++) {
		// PER VERTEX NORMALS
		if ((!m->dot_normalPerVertex.empty() && !m->face_index_normalPerVertex.empty())) {
			glNormal3fv(&m->dot_normalPerVertex[m->face_index_normalPerVertex[i]].x);
		}
		if (!m->dot_texture.empty() && !m->face_index_texture.empty()) {
			glTexCoord2fv(&m->dot_texture[m->face_index_texture[i]].x);
		}
		// color
		Vec3f offset = (m->dot_vertex[m->face_index_vertex[i]]);
		//
		glColor3f(fabs(sin(offset.x)), fabs(cos(offset.y)), fabs(offset.z));
		glVertex3fv(&m->dot_vertex[m->face_index_vertex[i]].x);
	}
	glEnd();
	glEndList();
	return listID;
}*/

// init
void init() {
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	ratio = (double)width / (double)height;
	// mesh
	//mesh1 = createPlane(1000, 1000, 40);
	//calculateNormalPerFace(mesh1);
	//calculateNormalPerVertex(mesh1);
	glEnable(GL_TEXTURE_2D);
	bmpTexture(textureArray, "brick.bmp", 0);
	bmpTexture(textureArray, "cobblestone.bmp", 1);
	bmpTexture(textureArray, "stars.bmp", 2);
	displayPlane = createPlane();
}

// reshape
void reshape(int w, int h) {
	width = w;
	height = h;
	if (h == 0) h = 1;
	ratio = 1.0f * w / h;
}

// display
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	// projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(45, ratio, 1, 1000);
	// view
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// lookAt
	gluLookAt(0.0f, 0.0f, 0.0,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f);
	// camera
	glScalef(scale, scale, scale);
	glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);


	//plane
	glPushMatrix();
	//glTranslatef(-500, 0, -500);
	//glRotatef(5.0, 1.0, 0.0, 0.0);
	glColor3f(1.0, 0.0, 1.0);
	//glCallList(displayPlane);
	drawHouse();
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glutSwapBuffers();
}

// mouse
void mouse(int button, int state, int x, int y) {
	mouse_x = x;
	mouse_y = y;
	mouse_button = button;
}

// motion
void motion(int x, int y) {
	if (mouse_button == GLUT_LEFT_BUTTON) {
		y_angle += (float(x - mouse_x) / width) *360.0;
		x_angle += (float(y - mouse_y) / height)*360.0;
	}
	if (mouse_button == GLUT_RIGHT_BUTTON) {
		scale += (y - mouse_y) / 100.0;
		if (scale < 0.1) scale = 0.1;
		if (scale > 7)	scale = 7;
	}
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}


// main
void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("Mesh Data Structure");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	init();

	glutMainLoop();
}