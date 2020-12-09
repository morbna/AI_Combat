#ifndef  __NODE_H__
#define  __NODE_H__

using namespace std;

class Node
{
private:
	int row, col;
	Node* parent;
public:
	Node(int row, int col, Node* parent);
	int getRow();
	int getCol();
	Node* getParent();

	void setRow(int r);
	void setCol(int c);
	void setParent(Node* p);
};

#endif /* __NODE_H__ */
