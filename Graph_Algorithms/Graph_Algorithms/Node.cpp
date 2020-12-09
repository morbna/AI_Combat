#include "Node.h"

Node::Node(int row, int col, Node* parent)
{
	this->row = row;
	this->col = col;
	this->parent = parent;
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

void Node::setRow(int r)
{
	row = r;
}

void Node::setCol(int c)
{
	col = c;
}

void Node::setParent(Node* p)
{
	parent = p;
}
