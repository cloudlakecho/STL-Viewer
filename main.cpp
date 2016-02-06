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

class cMenu
{
public:
    enum class item
    {
        none,
        axis,
        zoom,
        spinx,
        spiny,
        spinz,
        panx,
        pany,
        panz,
        viewx,
        viewy,
        viewz,
        solid
    };

    /// Construct menu
    void Construct();

    /**  User has selected a menu item, do whatever is required

    @param[in] i index to menu item selected

    */
    void Do( item i );

} theMenu;

class cMouseWheel
{
public:

    cMenu::item myMode;

    cMouseWheel()
        : myMode(cMenu::item::zoom )
    {

    }

    void Mode( cMenu::item item )
    {
        myMode = item;
    }

    cMenu::item Mode()
    {
        return myMode;
    }

    /// Called when mousewheel moves
    void Do (int dir );

} theMouseWheel;


/// Looks after camera position
class cCamera
{
public:

    /** Constructor */
    cCamera();

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
        gluPerspective (50.0, myRatio, 10, 200000);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt( 0, -1000*myZoom, 0,
                   myPanX, myPanY, myPanZ,
                   0, 0, 1 );
    }

    void setSpin( int dir )
    {
        // increment the spin a little on each mouse button click

        float inc = dir * 10.0;

        switch( theMouseWheel.Mode() )
        {
        case cMenu::item::spinx:
            mySpinX += inc;
            if( mySpinX > 360 )
                mySpinX -= 360;
            break;
        case cMenu::item::spiny:
            mySpinY += inc;
            if( mySpinY > 360 )
                mySpinY -= 360;
            break;
        case cMenu::item::spinz:
            mySpinZ += inc;
            if( mySpinZ > 360 )
                mySpinZ -= 360;
            break;
        }

        glutPostRedisplay();
    }

    void Spin( float& x, float& y, float& z)
    {
        x = mySpinX;
        y = mySpinY;
        z = mySpinZ;
    }

    void setViewAlongAxis( cMenu::item menuItem )
    {
        switch( menuItem )
        {

        case cMenu::item::viewx:
            mySpinX = 0;
            mySpinY = 0;
            mySpinZ = 90;
            break;
        case cMenu::item::viewy:
            mySpinX = 0;
            mySpinY = 0;
            mySpinZ = 0;
            break;
        case cMenu::item::viewz:
            mySpinX = 90;
            mySpinY = 0;
            mySpinZ = 0;
            break;

        }
    }

    void Pan(  int dir )
    {
        float inc = dir * 10;

        switch( theMouseWheel.Mode() )
        {
        case cMenu::item::panx:
            myPanX += inc;
            break;
        case cMenu::item::pany:
            myPanY += inc;
            break;
        case cMenu::item::panz:
            myPanZ += inc;
            break;
        }

        glutPostRedisplay();
    }

private:

    float myZoom;       // Ratio of zoom in/out to default;
    float myRatio;           // window aspect ratio;
    float mySpinX;
    float mySpinY;
    float mySpinZ;
    float myPanX;
    float myPanY;
    float myPanZ;

} theCamera;

class cFacet
{
public:

    cFacet( const string& n )
        : myName( n )
    {

    }
    void Read( FILE * F );

    void DrawTriangle();
    void DrawWireFrame();

    bool IsBin()
    {
        return ( myName.find("PACK") != -1 );
    }

private:
    string myName;
    vector< float > myV;
};

/// Looks after STL file containing the triangular meshes
class cSTLFile
{
public:

    cSTLFile()
        : mySolidShowCount( -99 )
    {

    }

    /** Read triangular mesh from file */
    void Readfacets();

    int FacetCount()
    {
        return myFacetCount;
    }

    void DrawFacets();

    /** Show more or less solids as mousewheel turns

    @param[in] direction

    */
    void Solid( int dir );

    void Filename( const string& fn )
    {
        myfilename = fn;
    }

    void DrawText();

private:
    string myfilename;
    FILE * myF;
    int myFacetCount;
    int myVertexCount;
    int mySolidCount;       // number of solids in file
    int mySolidShowCount;   // number of solids to show, -99 show them all
    string myLastSolidName;
    vector < cFacet > myFacets;
    vector < cFacet > myBinFacets;

} theFile;

class cAxis
{
public:
    bool myfHide;

    cAxis()
        : myfHide( false )
    {

    }

    // Toggle Axis display
    void Toggle()
    {
        myfHide = ! myfHide;
    }

    // Draw, if not hidden
    void Draw()
    {
        if( myfHide )
            return;
        GLfloat xoff = - 150;
        GLfloat size = 100;
        glBegin(GL_LINES);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f( xoff, 0, 0 );
        glVertex3f( xoff + size, 0, 0);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f( xoff, 0, 0 );
        glVertex3f( xoff, size, 0 );
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f( xoff, 0, 0 );
        glVertex3f( xoff, 0, size );
        glColor3f(0.0, 1.0, 0.0);
        glEnd();
    }
} theAxis;


// called when mousewheel is turned
void mousewheel(int button, int dir, int x, int y)
{
    theMouseWheel.Do( dir );
}
// called when the window change
void ChangeSize(GLsizei w, GLsizei h)
{
    theCamera.WindowSize( w, h );
}

// called when mouse button pressed
void mouse(int button, int state, int x, int y)
{
//    switch (button)
//    {
//    case GLUT_LEFT_BUTTON:
//        if (state == GLUT_DOWN)
//            //theCamera.setSpin();
//        else
//            break;
//    case GLUT_MIDDLE_BUTTON:
//        if (state == GLUT_DOWN)
//            glutIdleFunc(NULL);
//        else
//            break;
//    default:
//        break;
//    }
}
// Called when menu item selected
void menu( int item )
{
    theMenu.Do( (cMenu::item) item );
}

void cMenu::Construct()
{
    glutCreateMenu( menu );
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutAddMenuEntry( "Toggle Axis", (int)item::axis );
    glutAddMenuEntry( "Wheel Zoom", (int)item::zoom );
    glutAddMenuEntry( "Wheel Spin X", (int)item::spinx );
    glutAddMenuEntry( "Wheel Spin Y", (int)item::spiny );
    glutAddMenuEntry( "Wheel Spin Z", (int)item::spinz );
    glutAddMenuEntry( "View down X", (int)item::viewx );
    glutAddMenuEntry( "View down Y", (int)item::viewy );
    glutAddMenuEntry( "View down Z", (int)item::viewz );
    glutAddMenuEntry( "Pan X", (int)item::panx );
    glutAddMenuEntry( "Pan Y", (int)item::pany );
    glutAddMenuEntry( "Pan Z", (int)item::panz );
    glutAddMenuEntry( "Pack", (int)item::solid );
}

void cMenu::Do( item i )
{
    switch( i )
    {
    case item::axis:
        theAxis.Toggle();
        break;
    case item::zoom:
    case item::spinx:
    case item::spiny:
    case item::spinz:
    case item::panx:
    case item::pany:
    case item::panz:
    case item::solid:

        theMouseWheel.Mode( i );
        break;

    case item::viewx:
    case item::viewy:
    case item::viewz:

        theCamera.setViewAlongAxis( i );
        break;

    }
}

void cMouseWheel::Do (int dir )
{
    //cout << (int)myMode << endl;

    switch( myMode )
    {
    case cMenu::item::zoom:
        theCamera.Zoom( dir );
        break;
    case cMenu::item::spinx:
    case cMenu::item::spiny:
    case cMenu::item::spinz:
        theCamera.setSpin( dir );
        break;
    case cMenu::item::panx:
    case cMenu::item::pany:
    case cMenu::item::panz:
        theCamera.Pan( dir );
        break;
    case cMenu::item::solid:
        theFile.Solid( dir );
        break;
    }
}

cCamera::cCamera()
    : myZoom( 1 )
    , mySpinX( 45 )
    , mySpinY( 45 )
    , mySpinZ( 45 )
    , myPanX( 0 )
    , myPanY( 0 )
    , myPanZ( 0 )
{

}

void cSTLFile::Solid( int dir )
{
    if( mySolidShowCount == -99 )
    {
        mySolidShowCount = mySolidCount;
    }
    mySolidShowCount += dir;
    if( mySolidShowCount < 1 )
        mySolidShowCount = 1;
    if( mySolidShowCount > mySolidCount )
        mySolidShowCount = mySolidCount;

    //cout << "show " << mySolidShowCount << endl;

    Readfacets();

    glutPostRedisplay();
}

void cFacet::Read( FILE * F )
{
    char linefromstandard[256];
    float x,y,z;
    fscanf(F, "%s %s %f %f %f", linefromstandard, linefromstandard,
           &x, &y, &z);
    myV.push_back( x );
    myV.push_back( y );
    myV.push_back( z );
    fscanf(F, "%s %s", linefromstandard, linefromstandard);
    for (int n=0; n<3; n++)
    {
        fscanf(F, "%s %f %f %f", linefromstandard,
               &x, &y, &z);
        myV.push_back( x );
        myV.push_back( y );
        myV.push_back( z );
    }
    fscanf(F, "%s", linefromstandard);
    fscanf(F, "%s", linefromstandard);
}

void cSTLFile::Readfacets()
{

    char linefromstandard[256];
    float coordinatex, coordinatey, coordinatez;

    myF = fopen( myfilename.c_str(), "r" );
    if ( ! myF )
    {
        cout << "I couldn't find the file or file is empty." << endl;
    }

    int SolidCount = 0;
    myFacetCount = 0;
    myVertexCount = 0;
    myFacets.clear();
    myBinFacets.clear();

    fscanf(myF, "%s %s", linefromstandard, nameoffile);
    //cout << linefromstandard <<" "<< nameoffile << endl;

    while((char) fgetc(myF)!='e')
    {
        cFacet facet( nameoffile );
        facet.Read( myF );
        if( ! facet.IsBin() )
        {
            myFacetCount++;
            myFacets.push_back( facet );
        }
        else
        {
            myBinFacets.push_back( facet );
        }

        fgetc(myF);
        if ((char) fgetc(myF)=='e')
        {
            // end of solid
            fscanf(myF, "%s %s", linefromstandard, nameoffile);

            // check if we want to see any more
            SolidCount++;
            if( mySolidShowCount == -99 )
            {
                // we want to see every solid
                mySolidCount = SolidCount;
                myLastSolidName = "";
            }
            else
            {
                // limited solids to be shown
                if( SolidCount == mySolidShowCount )
                {
                    cout << SolidCount << " solids, last " << nameoffile << endl;
                    myLastSolidName = nameoffile;
                    break;
                }
            }
            //cout << linefromstandard <<" "<< mySolidCount<<" "<< nameoffile << endl;

            // new solid
            if( fscanf(myF, "%s %s", linefromstandard, nameoffile) != 2 )
                break;
        }
    }

    fclose(myF);
}

void cFacet::DrawTriangle()
{
    glVertex3f( myV[ 3 ], myV[ 4 ], myV[ 5 ] );
    glVertex3f( myV[ 6 ], myV[ 7 ], myV[ 8 ] );
    glVertex3f( myV[ 9 ], myV[ 10 ], myV[ 11 ] );
}

void cFacet::DrawWireFrame()
{
    glVertex3f( myV[ 3 ], myV[ 4 ], myV[ 5 ] );
    glVertex3f( myV[ 6 ], myV[ 7 ], myV[ 8 ] );
    glVertex3f( myV[ 6 ], myV[ 7 ], myV[ 8 ] );
    glVertex3f( myV[ 9 ], myV[ 10 ], myV[ 11 ] );
    glVertex3f( myV[ 9 ], myV[ 10 ], myV[ 11 ] );
    glVertex3f( myV[ 3 ], myV[ 4 ], myV[ 5 ] );

}

void cSTLFile::DrawFacets()
{

    glColor4f(0.0, 1.0, 0.0, 0.5 );
    glBegin(GL_TRIANGLES);
    for( auto& f : myFacets )
        f.DrawTriangle();
    glEnd();

    glColor3f(0.0, 0.0, 1.0);
    glLineWidth( 5 );
    glBegin(GL_LINES);
    for( auto& f : myFacets )
        f.DrawWireFrame();
    glEnd();

    glColor4f(1.0, 1.0, 1.0, 0.2 );
    glBegin(GL_TRIANGLES);
    for( auto& f : myBinFacets )
        f.DrawTriangle();
    glEnd();


    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    for( auto& f : myBinFacets )
        f.DrawWireFrame();
    glEnd();


    DrawText();
}

void cSTLFile::DrawText()
{
    // check if we are showing all boxes
    if( mySolidCount == -99 )
        return;

    // color the letters white, no matter the lighting
    //glDisable(GL_LIGHTING);
    glColor3f( 1., 1., 1. );

    // locate the letters in botton left, no matter spinning, zooming and panning
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 100, 0.0, 100);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(5, 5);

    // draw the letters
    for( auto c : myLastSolidName )
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // restore opengl state
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    //glEnable(GL_LIGHTING);
}
int main(int argc, char* argv[])
{
    theFile.Filename( argv[1] );
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    char stringone[256] = "STL Viewer ";
    strcat(stringone,nameoffile);

    glutCreateWindow(stringone);

    SetupRC();

    // Create menu, popped up when user clicks right mouse button
    theMenu.Construct();

    // register functions to call when something happens
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glColor3f(0.0, 1.0, 0.0);
//    glShadeModel(GL_FLAT);
//    GLfloat ambientLight[]= {0.3, 0.3, 0.3, 1.0};
//    GLfloat diffuseLight[]= {0.7, 0.7, 0.7, 1.0};
//    GLfloat specular[]= {1.0, 1.0, 1.0, 1.0};
//    GLfloat specref[]= {1.0, 1.0, 1.0, 1.0};
//    GLfloat lightPos[]= {0.0, 0.0, 75.0, 1.0};
//    glEnable(GL_LIGHTING);
//    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
//    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
//    glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
//    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
//    glEnable(GL_LIGHT0);
//
//    glEnable(GL_COLOR_MATERIAL);
//    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
//    glMaterialfv(GL_FRONT,GL_SPECULAR,specref);
//    glMateriali(GL_FRONT,GL_SHININESS,128);

}

void RenderScene()
{
    // Move camera to required position
    theCamera.Position();

    // read mesh from file, if not already done
    if( ! theFile.FacetCount() )
    {
        theFile.Readfacets();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glPushMatrix();
    float xRot=45.0, yRot=45.0, zRot=45.0;
    theCamera.Spin( xRot, yRot, zRot );
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);

    theFile.DrawFacets();

    theAxis.Draw();

    glPopMatrix();

    glutSwapBuffers();
}











