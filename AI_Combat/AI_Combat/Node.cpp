#include "Node.h"

Node::Node(int r, int c, Node* pr, double g, double h)
{
    row = r;
    col = c;
    parent = pr;
    this->g = g;
    this->h = h;
    f = g + h;
}

int Node::getRow()
{
    return row;
}

int Node::getCol()
{
    return col;
}

Node* Node::getParent()
{
    return parent;
}

double Node::getG()
{
    return g;
}

double Node::getF()
{
    return f;
}

bool Node::operator==(const Node& other)
{
    return row == other.row && col == other.col;
}
