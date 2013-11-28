#include "Graph3D.h"
#include <stdlib.h>
#include <math.h> 

#define PI 3.1415926 // For computing the point locations on the sphere

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

int Node3D::getID()
{
    return ID;
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

void Node3D::setID(int ID)
{
    this->ID = ID;
}

int Edge3D::getSource()
{
    return source;
}

int Edge3D::getSink()
{
    return sink;
}

bool Edge3D::getDrawStatus()
{
    return drawStatus;
}

void Edge3D::setSource(int source)
{
    this->source = source;
}

void Edge3D::setSink(int sink)
{
    this->sink = sink;
}

void Edge3D::setDrawStatus(bool drawStatus)
{
    this->drawStatus = drawStatus;
}

// This method populates its Graph components using
// the data recieved in the text file passed as a paramater
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
            // Set this node's ID
            newNode.setID(i);
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
        // Something went wrong
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

void Graph3D::modify(int selNode)
{
    // Iterate though the edges
    vector<Edge3D>::iterator edgeIterator = edges.begin();
    while (edgeIterator != edges.end())
    {
        if (edgeIterator->getSource() == selNode || edgeIterator->getSink() == selNode)
        {
            // This edge is related, set draw enabled
            edgeIterator->setDrawStatus(true); 
            edgeIterator++;
        }
        else
        {
            // This edge is unrelated to the selected node
            edgeIterator->setDrawStatus(false);
            edgeIterator++;
        }
    }

}
    
void Graph3D::reset()
{
    // Iterate throught the edges and set draw to true for all
    vector<Edge3D>::iterator edgeIterator = edges.begin();
    while (edgeIterator != edges.end())
    {
        edgeIterator->setDrawStatus(true);
        edgeIterator++;
    }
}


// This function computes the location of each node
// The location scales according to how many nodes are in the graph
// The algorithm implemented below was designed by Saff and Kuijlaars
// I have modified this algorithm slightly to fit this programs needs
void Graph3D::computeNodeLocations()
{
    int radiusModifier = 10; // The radius of the sphere
    int N = numNodes;
    float s = 3.6 / sqrt(N);
    float dz = 2.0 / N;
    float llong = 0;
    float z = 1 - dz / 2;
    float r = 0.0;
    for (int k = 0; k < N; k++)
    {
        r = sqrt(1 - z * z);
        // Set node k's coordinates to the calculated points
        nodes[k].setX(cos(llong) * r * radiusModifier);
        nodes[k].setY(sin(llong) * r * radiusModifier);
        nodes[k].setZ(z * radiusModifier);
        z = z - dz;
        llong += s / r;
    }
}
