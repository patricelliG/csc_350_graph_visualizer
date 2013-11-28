/////////////////////////////////          
// GraphVisualizer.cpp
//
// This program draws a 3D graph about the origin. 
//
// Modified from box.cpp and ballAndTorusPicking.cpp by Sumanta Guha.
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

//Values used in picking
static int isSelecting = 0; // Are we in selection mode? 1 = yes, 0 = no 
static unsigned int closestName = -1; // Name of closest hit.
static int hits; // Number of entries in hit buffer.
static unsigned int buffer[1024]; // Hit buffer.

Graph3D g1; //The Graph to be visualized
float zoom = -15.0; //The user's zoom level
float rotX = 0.0; //The graph's rotation in X
float rotY = 0.0; //The graph's rotation in Y
int selectedNode = -1; // The currently selected node. -1 denoted no node is selected


    


void drawGraph()
{
    // If a node is selected, find out which edges and nodes are effected
    if (selectedNode != -1)
        g1.modify(selectedNode);    
    else
        g1.reset();

    glLoadIdentity(); 
    gluLookAt(0.0, 0.0, zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(rotX, 1.0, 0.0, 0.0);
    glRotatef(rotY, 0.0, 1.0, 0.0);
 
    glMatrixMode(GL_MODELVIEW);
    //Draw the nodes
    for (int i = 0; i < g1.getNumNodes(); i++)
    {
        if (isSelecting)
            glLoadName(g1.getNodeAt(i).getID());
        glPushMatrix();
        //Move it into position
        glTranslatef(g1.getNodeAt(i).getX(), g1.getNodeAt(i).getY(), g1.getNodeAt(i).getZ());
        //Render the node 
        glutSolidSphere(NODE_RADIUS, NODE_SLICES, NODE_STACKS);
        glPopMatrix();
        //If we are is selection mode, get the ID
    }

    //Draw the edges
    Node3D source, sink;
    glLineWidth(1.0);
        glColor3f(1.0, 1.0, 1.0);
        for (int i = 0; i < g1.getNumEdges(); i++)
        {
            //Check to see if this edge should be drawn
            if (g1.getEdgeAt(i).getDrawStatus())
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
        }
    glutSwapBuffers();
}

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0, 0.3, 0.8);

    //Draw graph is rendering mode
    isSelecting = 0;
    drawGraph();
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

// Process hit buffer to find record with smallest min-z value.
void findClosestHit(int hits, unsigned int buffer[])
{
   unsigned int *ptr, minZ;
   
   minZ= 0xffffffff; // 2^32 - 1
   ptr = buffer;
   closestName = -1;
   for (int i = 0; i < hits; i++)                   
   {
      ptr++;
      if (*ptr < minZ) 
      {
         minZ = *ptr;
         ptr += 2;
         closestName = *ptr;
         ptr++;
      }
      else ptr += 3;
   }
   // Set selected node
   selectedNode = closestName;
}

// The mouse callback routine.
void pickFunction(int button, int state, int x, int y)
{
   int viewport[4]; // Viewport data.

   if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return; // Don't react unless left button is pressed.
   
   glGetIntegerv(GL_VIEWPORT, viewport); // Get viewport data.

   glSelectBuffer(1024, buffer); // Specify buffer to write hit records in selection mode
   (void) glRenderMode(GL_SELECT); // Enter selection mode.

   // Save the viewing volume defined in the resize routine.
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   
   // Define a viewing volume corresponding to selecting in 3 x 3 region around the cursor.
   glLoadIdentity();
   gluPickMatrix((float)x, (float)(viewport[3] - y), 3.0, 3.0, viewport);
   glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0); // Copied from the reshape routine.
   
   glMatrixMode(GL_MODELVIEW); // Return to modelview mode before drawing.
   glLoadIdentity();

   glInitNames(); // Initializes the name stack to empty.
   glPushName(0); // Puts name 0 on top of stack.

   // Determine hits by calling drawBallAndTorus() so that names are assigned.
   isSelecting = 1;
   drawGraph();

   hits = glRenderMode(GL_RENDER); // Return to rendering mode, returning number of hits.

   // Restore viewing volume of the resize routine and return to modelview mode.
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

   // Determine closest of the hit objects (if any).
   findClosestHit(hits, buffer);

   glutPostRedisplay();
}


//Print instructions for the user
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
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100); 
    glutCreateWindow("GraphVisualizer.cpp");
    setup(); 
    glutDisplayFunc(drawScene); 
    glutReshapeFunc(resize);  
    glutKeyboardFunc(keyInput);
    glutMouseFunc(pickFunction); 
    glutMainLoop(); 
 
    return 0;  
}


