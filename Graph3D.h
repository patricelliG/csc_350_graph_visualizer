#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Node3D {
public:
    float getX(void);
    float getY(void);
    float getZ(void);
    void setX(float);
    void setY(float);
    void setZ(float);
private:
    float x;
    float y;
    float z;
};

class Edge3D {
public:
    int getSource(void);
    int getSink(void);
    void setSource(int);
    void setSink(int);
private:
    int source;
    int sink;
};

class Graph3D {
public:
    void readInGraph(string);
    int getNumNodes(void);
    int getNumEdges(void);
    void setNumNodes(int);
    void setNumEdges(int);
    Node3D getNodeAt(int);
    Edge3D getEdgeAt(int);
    void computeNodeLocations(void);
private:
    vector<Edge3D> edges;
    vector<Node3D> nodes;
    int numNodes;
    int numEdges;
};
