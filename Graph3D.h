#include <string>
#include <iostream>
#include <fstream>
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
private:
    Edge3D edges [];
    Node3D nodes [];
};
