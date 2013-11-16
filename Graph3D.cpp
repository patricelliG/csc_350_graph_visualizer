#include "Graph3D.h"

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

    //Read in the data
    string line;
    if (input.is_open())
    {
        while (getline (input,line))
        {
            cout << line << endl;
        }
    }
    else
    {
        cout << "Input file error: Unable to open the specified file." << endl;
    }
}

