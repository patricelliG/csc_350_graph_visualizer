/////////////////////////////////          
// GraphVisualizer.cpp
//
// This program draws a 3D graph about the origin. 
//
// Modified from box.cpp, viewports.cpp and ballAndTorusPicking.cpp by Sumanta Guha.
/////////////////////////////////

#include <iostream>
#include <cmath>
#include "Graph3D.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;

#define NODE_SLICES 25
#define NODE_STACKS 25
#define NODE_RADIUS 2.0
#define EDGE_WIDTH 1.5
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 800

static int isSelecting = 0; // Are we in selection mode? 1 = yes, 0 = no 
static unsigned int closestName = -1; // Name of closest hit.
static int hits; // Number of entries in hit buffer.
static unsigned int buffer[1024]; // Hit buffer.
static Graph3D g1; //The Graph to be visualized
static float zoom = 0.0; //The user's zoom level
static float zOffset = 0.0; //The user's zoom level
static float zoomAmount = 0.0; //The amount of change in zoom on a keypress 
static float rotX = 0.0; //The graph's rotation in X
static float rotY = 0.0; //The graph's rotation in Y
static int selectedNode = -1; // The currently selected node. -1 denotes no node is selected
static float nodeRadius = 1.0;
static float far = 0.0; // The depth of the frustrum, this needs to scale with the graph


// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{  
   char *c;
   for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

void drawGraph()
{
    // Draw the information pane
    glViewport(0, 0, WINDOW_WIDTH / 2.0, WINDOW_HEIGHT);
    //glutSolidSphere(nodeRadius, NODE_SLICES, NODE_STACKS);
    // Display the information text, must take into acount the position of the camera
    glRasterPos3f(WINDOW_WIDTH / 120, WINDOW_HEIGHT / 60 , zoom + 15.0);
    writeBitmapString(GLUT_BITMAP_8_BY_13, "GLUT_BITMAP_8_BY_13");
    glRasterPos3f(WINDOW_WIDTH / 120, WINDOW_HEIGHT / 65 , zoom + 15.0);
    writeBitmapString(GLUT_BITMAP_8_BY_13, "GLUT_BITMAP_8_BY_13");
    glRasterPos3f(WINDOW_WIDTH / 120, WINDOW_HEIGHT / 70 , zoom + 15.0);
    writeBitmapString(GLUT_BITMAP_8_BY_13, "GLUT_BITMAP_8_BY_13");

    // Draw the graph display pane
    glViewport(WINDOW_WIDTH / 2.0, 0, WINDOW_WIDTH / 2.0, WINDOW_HEIGHT);

    // If a node is selected, find out which edges and nodes are effected
    if (selectedNode != -1)
        g1.modify(selectedNode); //Modify the graph to reflect the selection
    else
        g1.reset(); //Not selecting, reset the graph to normal

    glLoadIdentity(); 
    gluLookAt(0.0, 0.0, zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glPushMatrix();

    //Move the graph back to the origin
    glTranslatef(0.0, 0.0, -zOffset);
    // Rotate the graph on the X and Y axis as per the user's request
    glRotatef(rotX, 1.0, 0.0, 0.0);
    glRotatef(rotY, 0.0, 1.0, 0.0);
 
    //glMatrixMode(GL_MODELVIEW);
    
    //Draw the nodes
    for (int i = 0; i < g1.getNumNodes(); i++)
    {
        //If we are in selection mode, get the ID
        if (isSelecting)
            glLoadName(g1.getNodeAt(i).getID());
        // See if a node is selected
        if (selectedNode != -1)
        {
            // We have a node selected
            // See what drawing state the node is in
            // Set the drawing color accordingly
            if (g1.getNodeAt(i).getDrawState() == 1)
            {
                // This is the selected node
                glColor3f(0.0, 1.0, 1.0);
            }
            else if (g1.getNodeAt(i).getDrawState() == 2)
            { 
                // This node is connected to the selected node
                glColor3f(0.0, 0.0, 0.5);
            }
            else 
            {
                // This node is not directly connected to the selected node
                glColor3f(0.1, 0.1, 0.1);
            }
        }
        else
        {
            // No node is selected, use defualt colors
            glColor3f(0.0, 0.0, 1.0);
        }

        // Put a copy of the martrix on the stack
        glPushMatrix();
        // Move this node into position
        glTranslatef(g1.getNodeAt(i).getX(), g1.getNodeAt(i).getY(), g1.getNodeAt(i).getZ());
        // Render the node 
        glutSolidSphere(nodeRadius, NODE_SLICES, NODE_STACKS);
        // Restore the previous matrix
        glPopMatrix();
    }

    //Draw the edges
    Node3D source, sink;
    glLineWidth(EDGE_WIDTH);
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
    // Move the graph back to its level along the z axis
    glTranslatef(0.0,0.0, zOffset);
    glPopMatrix();
    glutSwapBuffers();
}

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    // Create the graph from the file
    g1.readInGraph(fileName);
    // Compute the node lacatios
    g1.computeNodeLocations(); 
    // Compute node radius
    nodeRadius += ((float)(g1.getNumNodes()) / 50.0); 
    // Initialize zoom level on start
    zoom = -g1.getNumNodes() * 3;
    // Compute zoom amount
    zoomAmount = (float)g1.getNumNodes() / 10.0;
    // Compute the required frustum depth
    far = 5 * (float)g1.getNumNodes(); 
}

    
// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, far);
 
    glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch(key) 
    {
        case 97:
            rotY += 2;
            glutPostRedisplay();
            break;
        case 100:
            rotY -= 2;
            glutPostRedisplay();
            break;
        case 101:
            //Zoom out
            //zoom += zoomAmount;
            zOffset += zoomAmount;
            glutPostRedisplay();
            break;
        case 113:
            //Zoom in
            //zoom -= zoomAmount;
            zOffset -= zoomAmount;
            glutPostRedisplay();
            break;
        case 115:
            rotX += 2;
            glutPostRedisplay();
            break;
        case 119:
            rotX -= 2;
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
   glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, far); // Copied from the reshape routine.
   
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
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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


