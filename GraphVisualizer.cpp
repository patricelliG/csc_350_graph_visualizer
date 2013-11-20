/////////////////////////////////          
// GraphVisualizer.cpp
//
// This program draws a 3D graph about the origin. 
//
// Modified from box.cpp by Sumanta Guha.
/////////////////////////////////

#include <iostream>
#include "Graph3D.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;

#define NODE_RADIUS .4
#define NODE_SLICES 25
#define NODE_STACKS 25

Graph3D g1;
float zoom = -15.0;
float rotX = 0.0;
float rotY = 0.0;

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0, 0.3, 0.8);
    glLoadIdentity(); 
    gluLookAt(0.0, 0.0, zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(rotX, 1.0, 0.0, 0.0);
    glRotatef(rotY, 0.0, 1.0, 0.0);
 
    glMatrixMode(GL_MODELVIEW);
    //Draw the nodes
    for (int i = 0; i < g1.getNumNodes(); i++)
    {
        glPushMatrix();
        //Move it into position
        glTranslatef(g1.getNodeAt(i).getX(), g1.getNodeAt(i).getY(), g1.getNodeAt(i).getZ());
        //Render the node 
        glutSolidSphere(NODE_RADIUS, NODE_SLICES, NODE_STACKS);
        glPopMatrix();
    }

    //Draw the edges
    Node3D source, sink;
    glLineWidth(1.0);
        glColor3f(1.0, 1.0, 1.0);
        for (int i = 0; i < g1.getNumEdges(); i++)
        {
            //Retrieve the source and sink nodes
            source = g1.getNodeAt(g1.getEdgeAt(i).getSource());
            sink = g1.getNodeAt(g1.getEdgeAt(i).getSink());
            glBegin(GL_LINE_STRIP);
                //Draw the line from source to sink
                glVertex3f(source.getX(), source.getY(), source.getZ());
                glVertex3f(sink.getX(), sink.getY(), sink.getZ());
            glEnd();
        }

    glFlush();
    glutSwapBuffers();
}

// Initialization routine.
void setup(void) 
{
    glClearColor(0.0, 0.0, 0.0, 0.0);  
    glEnable(GL_DEPTH_TEST);
}

//Initialize the graph
void initGraph()
{
    // Get the file name
    string fileName = "";
    cout << "Enter the file to read from:" << endl;
    cin >> fileName;
    g1.readInGraph(fileName);
    g1.computeNodeLocations(); 
}
    
// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
 
    glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch(key) 
    {
        case 97:
            rotY += 1;
            glutPostRedisplay();
            break;
        case 100:
            rotY -= 1;
            glutPostRedisplay();
            break;
        case 101:
            //Zoom out
            zoom += .25;
            glutPostRedisplay();
            break;
        case 113:
            //Zoom in
            zoom -= .25;
            glutPostRedisplay();
            break;
        case 115:
            rotX += 1;
            glutPostRedisplay();
            break;
        case 119:
            rotX -= 1;
            glutPostRedisplay();
            break;
        case 27:
            exit(0);
            break;
        default:
            break;
    }
}

void printInteraction(void)
{
    cout << "Interaction:" << endl;
    cout << "Press Q or E to zoom in / out." << endl;
    cout << "Press A or D to rotate the graph about the Y axix." << endl;
    cout << "Press W or S to rotate the graph about the X axix." << endl;
}

// Main routine.
int main(int argc, char **argv) 
{
    initGraph();
    printInteraction();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(100, 100); 
    glutCreateWindow("box.cpp");
    setup(); 
    glutDisplayFunc(drawScene); 
    glutReshapeFunc(resize);  
    glutKeyboardFunc(keyInput);
    glutMainLoop(); 
 
    return 0;  
}


