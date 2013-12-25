////////////////////////////////////////////////////////////////////////////////////        
//
// Gary Patricelli 
//
// CSC 350 - Final Project 
//
// GraphVisualizer.cpp
//
// This program reads in a file to create a graph. The graph is modeled in three
// dimensions and the user is able to interact with the graph. Instructions on how
// to do so are listed below.
//
// How to use the program:
//      Keyboard controls:
//          Zooming:
//              'Q' / 'E' = zoom in / sioom out
//          Rotation on X:
//              'W' / 'S' = rotate the graph about the 'x' axis
//          Rotation on Y:
//              'A' / 'D' = rotate the graph about the 'y' axis
//      Mouse Controls:
//          Left click on a node to select it. The graph will enter selection mode
//          in oder to ease the user in exploring the graph in byte sized pieces.
//          In oder to exit selection mode, click anywhere outside of the currently
//          selected node cluster. Note, you may wish to chain clicks together to
//          explore the graph a node at a time.
//
//
// Modified from box.cpp, viewports.cpp, fonts.cpp, sphereInBox2.cpp and ballAndTorusPicking.cpp by Sumanta Guha.
//
////////////////////////////////////////////////////////////////////////////////////        

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
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
#define BITMAP_FONT GLUT_BITMAP_9_BY_15

static int isSelecting = 0; // Are we in selection mode? 1 = yes, 0 = no 
static unsigned int closestName = -1; // Name of closest hit.
static int hits; // Number of entries in hit buffer.
static unsigned int buffer[1024]; // Hit buffer.
static Graph3D g1; // The Graph to be visualized
static float zoom = 0.0; // The user's zoom level
static float zOffset = 0.0; // The graph's position on the 'z' axis 
static float zoomAmount = 0.0; // The amount of change in zoom on a key press 
static float rotX = 0.0; // The graph's rotation in X
static float rotY = 0.0; // The graph's rotation in Y
static int selectedNode = -1; // The currently selected node. -1 denotes no node is selected
static float nodeRadius = 1.0;
static float far = 0.0; // The depth of the frustum, this needs to scale with the graph
static string fileName = ""; // The name of the input file


// Routine to draw a bitmap character string.
void printString(string str)
{  
    char c;
    int cVal;
    for (int i = 0; i < str.size(); i++)
    {
        // Get first character
        c = str[i];
        // Get this char's int
        cVal = (int)(c); 
        // Print it 
        glutBitmapCharacter(BITMAP_FONT, cVal);
    }
}

// Method to convert int to string
string intToString(int num)
{
    stringstream ss;
    ss << num;
    return ss.str();
}

void drawGraph()
{

    // Draw the information pane
    glDisable(GL_LIGHTING);
    glViewport(0, 0, WINDOW_WIDTH / 2.0, WINDOW_HEIGHT);

    // Display the information text, must take into account the position of the camera
    // Display the file name
    string file = "FILE_NAME: "; 
    file.append(fileName);
    glRasterPos3f(WINDOW_WIDTH / 120, WINDOW_HEIGHT / 60 , zoom + 15.0);
    printString(file);

    // Display the number of nodes
    string nodes = "NODE COUNT: "; 
    nodes.append(intToString(g1.getNumNodes()));
    glRasterPos3f(WINDOW_WIDTH / 120, WINDOW_HEIGHT / 65 , zoom + 15.0);
    printString(nodes);

    // Display the number of edges
    string edges = "EDGE COUNT: ";
    edges.append(intToString(g1.getNumEdges()));
    glRasterPos3f(WINDOW_WIDTH / 120, WINDOW_HEIGHT / 70 , zoom + 15.0);
    printString(edges);

    // Display the selected node ID
    string node = "SELECTED NODE: ";
    if (selectedNode != -1)
        node.append(intToString(selectedNode));
    else
        node.append("NONE");
    glRasterPos3f(WINDOW_WIDTH / 120, WINDOW_HEIGHT / 75 , zoom + 15.0);
    printString(node);
    

    // Draw the graph display pane
    glViewport(WINDOW_WIDTH / 2.0, 0, WINDOW_WIDTH / 2.0, WINDOW_HEIGHT);
    glEnable(GL_LIGHTING);

    // If a node is selected, find out which edges and nodes are effected
    if (selectedNode != -1)
        g1.modify(selectedNode); //Modify the graph to reflect the selection
    else
        g1.reset(); //Not selecting, reset the graph to normal

    glLoadIdentity(); 
    // Set up the camera
    gluLookAt(0.0, 0.0, zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glPushMatrix();

    //Move the graph back to the origin
    glTranslatef(0.0, 0.0, -zOffset);
    // Rotate the graph on the X and Y axis as per the user's request
    glRotatef(rotX, 1.0, 0.0, 0.0);
    glRotatef(rotY, 0.0, 1.0, 0.0);
 
    // Material property vectors for each type of node.
    float matAmbAndDifSelected[] = {0.0, 0.5, 0.5, 1.0}; // Selected node
    float matAmbAndDifRelated[] = {0.0, 0.0, 0.5, 1.0}; // Node with connecting edge to selected node
    float matAmbAndDifNormal[] = {0.0, 0.0, 0.9, 1.0}; // Normal node 
    float matAmbAndDifDark[] = {0.0, 0.0, 0.0, 1.0}; // Dark node
    float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
    float matShine[] = { 50.0 };
    
    //Draw the nodes
    for (int i = 0; i < g1.getNumNodes(); i++)
    {
        // Put a copy of the matrix on the stack
        glPushMatrix();
        // Move this node into position
        glTranslatef(g1.getNodeAt(i).getX(), g1.getNodeAt(i).getY(), g1.getNodeAt(i).getZ());

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
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDifSelected);
                // Cull the back faces of the sphere.
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                // Render the node
                glutSolidSphere(nodeRadius, NODE_SLICES, NODE_STACKS);
                glDisable(GL_CULL_FACE);
            }
            else if (g1.getNodeAt(i).getDrawState() == 2)
            { 
                // This node is connected to the selected node
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDifRelated);
                // Cull the back faces of the sphere.
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                // Render the node
                glutSolidSphere(nodeRadius, NODE_SLICES, NODE_STACKS);
                glDisable(GL_CULL_FACE);
            }
            else 
            {
                // This node is not directly connected to the selected node
                // Material properties of the sphere (only the front is ever seen).
                // Don't render anything
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDifDark);
            }
        }
        else
        {
            // No node is selected, use default colors
            // Material properties of the sphere (only the front is ever seen).
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDifNormal);
            // Cull the back faces of the sphere.
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            // Render the node
            glutSolidSphere(nodeRadius, NODE_SLICES, NODE_STACKS);
            glDisable(GL_CULL_FACE);
        }
        
        // Restore the previous matrix
        glPopMatrix();
    }

    //Draw the edges
    glDisable(GL_LIGHTING);
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
                glBegin(GL_LINES);
                    //Draw the line from source to sink
                    glVertex3f(source.getX(), source.getY(), source.getZ());
                    glVertex3f(sink.getX(), sink.getY(), sink.getZ());
                glEnd();
            }
        }
    // Move the graph back to its level along the 'z' axis
    glTranslatef(0.0,0.0, zOffset);
    glEnable(GL_LIGHTING);
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
    // Enable anti aliasing
    glEnable(GL_LINE_SMOOTH); 
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); 

    //Set fill color
    glClearColor(0.0, 0.0, 0.0, 0.0);  

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);

    // Light property vectors.
    float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
    float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
    float lightPos[] = { 0.0, zoom, 0.0, 1.0 };
    float globAmb[] = { 0.1, 0.1, 0.1, 1.0 };
 
    // Light properties.
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
 
    glEnable(GL_LIGHT0); // Enable particular light source.
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Enable two-sided lighting.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.
 
    // Enable two vertex arrays: position and normal.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
 
}

//Initialize the graph 
void initGraph()
{
    // Get the file name
    cout << "Enter the file to read from:" << endl;
    cin >> fileName;
    // Create the graph from the file
    g1.readInGraph(fileName);
    // Compute the node locations
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
            zOffset += zoomAmount;
            glutPostRedisplay();
            break;
        case 113:
            //Zoom in
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

   // Determine hits by calling drawGraph() so that names are assigned.
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
    cout << "\nInteraction:" << endl;
    cout << "   Press Q or E to zoom in / out." << endl;
    cout << "   Press A or D to rotate the graph about the Y axis." << endl;
    cout << "   Press W or S to rotate the graph about the X axis." << endl;
    cout << "   Click on a node to enter selection mode." << endl;
    cout << "   Click anywhere besides a node to exit selection mode." << endl;
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
