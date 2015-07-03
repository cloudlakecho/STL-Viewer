// STL Viewer in GitHub repository
// coding starts May 17, 2015 in Chula Vista, California
// ASCII STL


#include <iostream>
#include <fstream>
#include <cstdio>
#include <math.h>
#include <cmath>
#include <vector>
#include <cstring>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <freeglut.h>

using namespace std;

void ChangeSize(GLsizei, GLsizei);
void RenderScene();
void SetupRC();
void SpinDisplay();
void mouse(int, int, int, int);

static GLfloat spin = 0.0;
static char nameoffile[256];

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	char stringone[256] = "STL Viewer ";
	strcat(stringone,nameoffile);
	
	glutCreateWindow(stringone);
	
	SetupRC();
	
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(mouse);

	glutMainLoop();

	system ("pause");

	
	
	
	
	return 0;
}

void SetupRC()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(0.0, 1.0, 0.0);
	glShadeModel(GL_FLAT);
	GLfloat ambientLight[]={0.3, 0.3, 0.3, 1.0};
	GLfloat diffuseLight[]={0.7, 0.7, 0.7, 1.0};
	GLfloat specular[]={1.0, 1.0, 1.0, 1.0};
	GLfloat specref[]={1.0, 1.0, 1.0, 1.0};
	GLfloat lightPos[]={0.0, 0.0, 75.0, 1.0};
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specref);
	glMateriali(GL_FRONT,GL_SHININESS,128);

}

void ChangeSize(GLsizei w, GLsizei h)
{
	float maxDistance = 100.0;
	GLfloat maxRange = maxDistance;
	if (h==0)
		h=1;

	glViewport (0, 0, w, h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float ratio = w/h;
	if (w<=h)
		glOrtho (-maxRange, maxRange, -maxRange/ratio, maxRange/ratio, -maxRange,
		maxRange);
	else
		glOrtho (-maxRange*ratio, maxRange*ratio, -maxRange, maxRange, -maxRange,
		maxRange);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderScene()
{		
	string givenline;
	vector<string> givenlines;
	string title, eachline, facet, normal, vertex;
	char linefromstandard[256];
	float coordinatex, coordinatey, coordinatez; 
	int totalfacet=0, totalvertex=0;
	int plutonium;
	FILE* givenfile;
	givenfile = fopen("coin_cargo.stl","r"); // The file you want to see. Currently it should be at the same
	// folder with main.cpp. The file location depends on system spec.
	/*givenfile = fopen ("anulus.stl","r");*/
	/*givenfile = fopen ("SmallGear.stl","r");*/
	/*givenfile = fopen ("handle.stl","r");*/
	/*givenfile = fopen ("crank.stl","r");*/
	
	vector <float> normalvector;
	vector <float> trianglecoordinate;
	if (givenfile != NULL)
	{
		/*cout << "There is the file" << endl;*/
		fscanf(givenfile, "%s %s", linefromstandard, nameoffile);
		/*cout << "The file name is " << nameoffile << endl;*/
		
		int m=1;
		while((char) fgetc(givenfile)!='e')
		{
			
			fscanf(givenfile, "%s %s %f %f %f", linefromstandard, linefromstandard, 
				&coordinatex, &coordinatey, &coordinatez);
			totalfacet++;
			/*cout << coordinatex << " " << coordinatey << " " << coordinatez << endl;*/
			normalvector.push_back(coordinatex);
			normalvector.push_back(coordinatey);
			normalvector.push_back(coordinatez);
			fscanf(givenfile, "%s %s", linefromstandard, linefromstandard);
			for (int n=0; n<3; n++)
			{
				fscanf(givenfile, "%s %f %f %f", linefromstandard, 
					&coordinatex, &coordinatey, &coordinatez);
				totalvertex++;
				/*cout << coordinatex << " " << coordinatey << " " << coordinatez << endl;*/
				trianglecoordinate.push_back(coordinatex);
				trianglecoordinate.push_back(coordinatey);
				trianglecoordinate.push_back(coordinatez);
			}
			fscanf(givenfile, "%s", linefromstandard);
			fscanf(givenfile, "%s", linefromstandard);
			if ((char) fgetc(givenfile)=='e')
			{
				break;
				m=0;
			}
		} 
		fclose(givenfile);
	}
	else 
		cout << "I couldn't find the file or file is empty." << endl;

	string filename;
	int totalfacetnumber=0;
	int filenameline = 2;
	int facetline = 2+2+3;
	
	/*cout << "\n";
	cout << totalfacet << " " << totalvertex/3 << " " << totalfacetnumber << endl;*/
	cout << normalvector.size()/3 << " " << trianglecoordinate.size()/(3*3) << endl;

	int protactinium, neptunium;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glPushMatrix();
	float xRot=45.0, yRot=45.0, zRot=45.0;
	glRotatef(xRot, 1.0, 0.0, 0.0);
	glRotatef(spin, 0.0, 1.0, 0.0); // Somehow this spin valiable doesn't work.
	glRotatef(zRot, 0.0, 0.0, 1.0);
	
	int groupsize=9;
	glBegin(GL_TRIANGLES);
	for (protactinium=0; protactinium<totalfacet; protactinium++)
	{
		neptunium = groupsize*protactinium;
		glVertex3f(trianglecoordinate.at(neptunium),trianglecoordinate.at(neptunium+1),
			trianglecoordinate.at(neptunium+2));
		glVertex3f(trianglecoordinate.at(neptunium+3),trianglecoordinate.at(neptunium+4),
			trianglecoordinate.at(neptunium+5));
		glVertex3f(trianglecoordinate.at(neptunium+6),trianglecoordinate.at(neptunium+7),
			trianglecoordinate.at(neptunium+8));
	}
	glEnd();
	
	glPopMatrix();
	glutSwapBuffers();
}

void SpinDisplay()
{
	spin =+ 20.0;
	spin = spin - 360 * (int) spin/360.0;
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
				glutIdleFunc(SpinDisplay);
			else
				break;
		case GLUT_MIDDLE_BUTTON:
			if (state == GLUT_DOWN)
				glutIdleFunc(NULL);
			else
				break;
		default:
			break;
	}
}







