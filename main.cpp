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

static char nameoffile[256];

/// Looks after camera position
class cCamera
{
public:

    /** Constructor */
    cCamera()
        : myZoom( 1 )
        , mySpin( 0 )
    {

    }

    /** Zoom */
    void Zoom( int dir )
    {
        if( dir > 0 )
            myZoom += 0.1;
        else
            myZoom -= 0.1;
        if( myZoom < 0.1 )
            myZoom = 0.1;
        glutPostRedisplay();
    }

    /** Window size */
    void WindowSize( int w, int h )
    {
        glViewport (0, 0, w, h);
        myRatio = w/h;
        glutPostRedisplay();
    }

    /** Move camera into position */
    void Position()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective (50.0*myZoom, myRatio, 10, 2000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt( 0, -1000, 0,
                   0, 0, 0,
                   0, 0, 1 );
    }

    void setSpin()
    {
        // increment the spin a little on each mouse button click
        mySpin += 20.0;
        if( mySpin > 360 )
            mySpin -= 360;
        glutPostRedisplay();
    }

    float Spin()
    {
        return mySpin;
    }

private:

    float myZoom;       // Ratio of zoom in/out to default;
    float myRatio;           // window aspect ratio;
    float mySpin;

} theCamera;

/// Looks after STL file containing the triangular meshes
class cSTLFile
{
public:

    void Open( const char* fname )
    {
        myFacetCount = 0;
        myVertexCount = 0;
        myF = fopen( fname, "r" );
    }

    FILE * file()
    {
        return myF;
    }

    void Readfacets()
    {
        char linefromstandard[256];
        float coordinatex, coordinatey, coordinatez;

        if ( ! myF )
        {
            cout << "I couldn't find the file or file is empty." << endl;
        }

        fscanf(myF, "%s %s", linefromstandard, nameoffile);

        while((char) fgetc(myF)!='e')
        {
            fscanf(myF, "%s %s %f %f %f", linefromstandard, linefromstandard,
                   &coordinatex, &coordinatey, &coordinatez);
            myFacetCount++;
            /*cout << coordinatex << " " << coordinatey << " " << coordinatez << endl;*/
            normalvector.push_back(coordinatex);
            normalvector.push_back(coordinatey);
            normalvector.push_back(coordinatez);
            fscanf(myF, "%s %s", linefromstandard, linefromstandard);
            for (int n=0; n<3; n++)
            {
                fscanf(myF, "%s %f %f %f", linefromstandard,
                       &coordinatex, &coordinatey, &coordinatez);
                myVertexCount++;
                /*cout << coordinatex << " " << coordinatey << " " << coordinatez << endl;*/
                trianglecoordinate.push_back(coordinatex);
                trianglecoordinate.push_back(coordinatey);
                trianglecoordinate.push_back(coordinatez);
            }
            fscanf(myF, "%s", linefromstandard);
            fscanf(myF, "%s", linefromstandard);

            if ((char) fgetc(myF)=='e')
            {
                break;
            }
        }

        fclose(myF);
    }

    int FacetCount()
    {
        return myFacetCount;
    }

    void DrawFacets()
    {
        int groupsize=9;
        glBegin(GL_TRIANGLES);
        for ( int protactinium=0; protactinium<myFacetCount; protactinium++)
        {
            int neptunium = groupsize*protactinium;
            glVertex3f(trianglecoordinate.at(neptunium),trianglecoordinate.at(neptunium+1),
                       trianglecoordinate.at(neptunium+2));
            glVertex3f(trianglecoordinate.at(neptunium+3),trianglecoordinate.at(neptunium+4),
                       trianglecoordinate.at(neptunium+5));
            glVertex3f(trianglecoordinate.at(neptunium+6),trianglecoordinate.at(neptunium+7),
                       trianglecoordinate.at(neptunium+8));
        }
        glEnd();
    }

private:
    FILE * myF;
    int myFacetCount;
    int myVertexCount;
    vector <float> normalvector;
    vector <float> trianglecoordinate;

} theFile;

// called when mousewheel is turned
void mousewheel(int button, int dir, int x, int y)
{
    theCamera.Zoom( dir );
}
// called when the window changes size
void ChangeSize(GLsizei w, GLsizei h)
{
    theCamera.WindowSize( w, h );
}

// called when mouse button pressed
void mouse(int button, int state, int x, int y)
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
            theCamera.setSpin();
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
    glutMouseWheelFunc( mousewheel );

    glutMainLoop();

    system ("pause");

    return 0;
}

void SetupRC()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glColor3f(0.0, 1.0, 0.0);
    glShadeModel(GL_FLAT);
    GLfloat ambientLight[]= {0.3, 0.3, 0.3, 1.0};
    GLfloat diffuseLight[]= {0.7, 0.7, 0.7, 1.0};
    GLfloat specular[]= {1.0, 1.0, 1.0, 1.0};
    GLfloat specref[]= {1.0, 1.0, 1.0, 1.0};
    GLfloat lightPos[]= {0.0, 0.0, 75.0, 1.0};
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

void RenderScene()
{
    // Move camera to required position
    theCamera.Position();

    string givenline;
    vector<string> givenlines;
    string title, eachline, facet, normal, vertex;
    char linefromstandard[256];
    float coordinatex, coordinatey, coordinatez;
    int plutonium;

    // read mesh from file, if not already done
    if( ! theFile.FacetCount() )
    {

        theFile.Open(  "coin_cargo.stl" );

        theFile.Readfacets();

    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glPushMatrix();
    float xRot=45.0, yRot=45.0, zRot=45.0;
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(theCamera.Spin(), 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);

    theFile.DrawFacets();

    glPopMatrix();

    glutSwapBuffers();
}











