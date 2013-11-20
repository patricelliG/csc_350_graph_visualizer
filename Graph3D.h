#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Node3D {
public:
    // Return x coordinate
    float getX(void);
    // Return y coordinate
    float getY(void);
    // Return z coordinate
    float getZ(void);
    // Set the x coordinate to float
    void setX(float);
    // Set the y coordinate to float
    void setY(float);
    // Set the z coordinate to float
    void setZ(float);
private:
    float x; // This node's x coordinate
    float y; // This node's y coordinate
    float z; // This node's z coordinate
};

class Edge3D {
public:
    // Return this edges source node
    int getSource(void);
    // Return this edges sink node
    int getSink(void);
    // Set this edges source 
    void setSource(int);
    // Set this edges sink
    void setSink(int);
private:
    int source;
    int sink;
};

class Graph3D {
public:
    // Populates the graph via a text file input
    void readInGraph(string);
    // Return the number of nodes in the graph
    int getNumNodes(void);
    // Return the number of edges in the graph
    int getNumEdges(void);
    // Set the number of nodes in this graph
    void setNumNodes(int);
    // Set the number of edges in this graph
    void setNumEdges(int);
    // Return the node at index param
    Node3D getNodeAt(int);
    // Return the edges at index param
    Edge3D getEdgeAt(int);
    // Calculate the positions of the nodes evenly dispursed on a sphere
    void computeNodeLocations(void);
private:
    vector<Node3D> nodes; // Container to hold this graph's nodes
    vector<Edge3D> edges; // Container to hold this graph's edges
    int numNodes; // The number of nodes in this graph
    int numEdges; // The number of edges in this graph
};
