#include "Graph3D.h"
#include <stdlib.h>

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
