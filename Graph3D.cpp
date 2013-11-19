#include "Graph3D.h"
#include <stdlib.h>
#include <math.h> 
#define PI 3.1415926

float Node3D::getX()
{
    return x;
}

float Node3D::getY()
{
    return y;
}

float Node3D::getZ()
{
    return z;
}

void Node3D::setX(float x)
{
    this->x = x; 
}

void Node3D::setY(float y)
{
    this->y = y; 
}

void Node3D::setZ(float z)
{
    this->z = z; 
}

int Edge3D::getSource()
{
    return source;
}

int Edge3D::getSink()
{
    return sink;
}

void Edge3D::setSource(int source)
{
    this->source = source;
}

void Edge3D::setSink(int sink)
{
    this->sink = sink;
}

void Graph3D::readInGraph(string file)
{
    //Open the file
    ifstream input (file.c_str());

    //Read in the data and make the graph
    string line;
    if (input.is_open())
    {
        //Get the number of nodes
        getline(input, line);
        numNodes = atoi(line.c_str()); 
        

        //Get the number of edges
        getline(input, line);
        numEdges = atoi(line.c_str()); 

        //Initialize the nodes vector
        for (int i = 0; i < numNodes; i++)
        {
            Node3D newNode;
            nodes.push_back(newNode);
        }

        //The rest of the file are edges
        //Store the edges in the edges vector
        string edgeSource, edgeSink;
        while (getline(input,line))
        {
          //Parse the source
          int index = 0;
          edgeSource = "";
          while (line[index] != ' ')
          {
              edgeSource += line[index];
              index++;
          }
          //Parse the sink
          edgeSink = line.substr(index, line.length());

          //Store the edge in the edges vector
          Edge3D newEdge;
          newEdge.setSource(atoi(edgeSource.c_str()));
          newEdge.setSink(atoi(edgeSink.c_str()));
          edges.push_back(newEdge);
        }
    }
    else
    {
        cout << "Input file error: Unable to open the specified file." << endl;
    }
}

int Graph3D::getNumNodes()
{
    return numNodes;
}

int Graph3D::getNumEdges()
{
    return numEdges;
}

void Graph3D::setNumNodes(int numNodes)
{
    this->numNodes = numNodes;
}

void Graph3D::setNumEdges(int numEdges)
{
    this->numEdges = numEdges;
}

Node3D Graph3D::getNodeAt(int index)
{
    return nodes[index];
}

Edge3D Graph3D::getEdgeAt(int index)
{
    return edges[index];
}

//This function computes the location of each node
//The location scales according to how many nodes are in the graph
void Graph3D::computeNodeLocations()
{
  int n;		/* number of points */
  int k;                /* index */
  float phi1, phi, theta, h, x, y, z;
  float R = 10.0;	/* default radius */
  int r;		/* true radius */
  int r1, r2, r3;	/* ellipsoid axis lengths */
  
  r = R;
  r1 = r2 = r3 = r;
  
  n = numNodes;
 
  phi1 = 0.0;

  //First node
  //printf ( "%6f  %6f  %6f\n", 0.0, 0.0, -1.0 * r3 );
  nodes[0].setX(0.0);
  nodes[0].setY(0.0);
  nodes[0].setZ(-1.0 * r3);

  for ( k = 2; k <= n - 1; k ++ ) {
    /* Generate a random point on a sphere of radius 1. */
    h = -1 + 2 * ( k - 1 ) / ( double )( n - 1 );
    theta = acos ( h );

    if ( theta < 0 || theta > M_PI )
      printf( "Error\n" ),
      exit (1);

    phi = phi1 + 3.6 / ( sqrt ( ( double )n * ( 1 - h * h ) ) ); 
    phi = fmod ( phi, 2 * M_PI );
    phi1 = phi;

    x = cos ( phi ) * sin ( theta );
    y = sin ( phi ) * sin ( theta );
    /* z = cos ( theta ); But z==h, so: */
    z = h;

//    cout << "x= " << r1 * x << " y= " << r2*y << " z= " << r3 * z << endl;
    nodes[k].setX(r1 * x);
    nodes[k].setY(r2 * y);
    nodes[k].setZ(r3 * z);
//    printf ( "%6f  %6f  %6f\n", r1 * x, r2 * y, r3 * z );
  }

    //Last Node
    nodes[n].setX(0.0);
    nodes[n].setY(0.0);
    nodes[n].setZ(1.0 * r3);
    //printf ( "%6f  %6f  %6f\n", 0.0, 0.0, 1.0 * r3 );  

}
