#ifndef  __NODE_H__
#define  __NODE_H__

class Node
{
private:
	int row, col;
	Node* parent;
	double g, h, f;
public:
	Node(int r, int c, Node* pr, double g, double h);
	int getRow();
	int getCol();
	Node* getParent();
	double getG();
	double getF();
	bool operator==(const Node& other);
};

class CompareNodes
{
public:
	bool operator()(Node* pn1, Node* pn2)
	{
		return pn1->getF() > pn2->getF();
	}
};

#endif /* __NODE_H__ */
