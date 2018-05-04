#include "Monte-Carlo.h"
#include <iostream>

int* top = NULL;
int** board = NULL;
int M, N;
int noX, noY;
clock_t start;
Tree tree;

void resetBoard() {
	for (int i = 0; i < M; ++i) {
		delete[] board[i];
	}
	delete[] board;
	board = NULL;
	delete[] top;
	top = NULL;
	cout << "reset" << endl;
}

void updateBoard(int MM, int NN, const int* newTop, int lastX, int lastY, int noXX, int noYY, int** newBoard, bool initialize) {
	if (initialize) {
		for (int i = 0; i < M; ++i) {
			delete[] board[i];
		}
		delete[] board;
		board = NULL;
		delete[] top;
		top = NULL;
		tree.reset();
		cout << "reset" << endl;
	}
	srand((unsigned)time(NULL));
	start = clock();
	M = MM;
	N = NN;
	noX = noXX;
	noY = noYY;
	if (top == NULL) top = new int[N];
	for (int i = 0; i < N; ++i) 
		top[i] = newTop[i];
	if (board == NULL) {
		board = new int*[M];
		for (int i = 0; i < M; ++i) {
			board[i] = new int[N];
			for (int k = 0; k < N; ++k)
				board[i][k] = newBoard[i][k];
		}
	}
	else if(lastX != -1) board[lastX][lastY] = 1;

	//¾À´í
	int flag = 0;
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			if (board[i][j] != newBoard[i][j]) {
				cout << "Error!" << endl;
				flag = 1;
				break;
			}
		}
		if (flag == 1) break;
	}
	if (flag == 1) {
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j) {
				cout << board[i][j] << ' ';
				board[i][j] = newBoard[i][j];
			}
			cout << endl;
		}
		tree.reset();
		cout << endl;
		for (int i = 0; i < M; ++i) {
			for (int k = 0; k < N; ++k) {
				cout<<board[i][k]<<' ';
			}
			cout << endl;
		}
	}
}

Node::Node():
	x(-1),y(-1),id(0),winTimes(0),times(0),parent(NULL),childrenColumnNext(0),isEnd(false){}

Node::Node(int x, int y, int id, Node* parent) :
	x(x), y(y), id(id), winTimes(0), times(0), parent(parent), childrenColumnNext(0), isEnd(false) {}

Node::~Node() {
	for (vector<Node*>::iterator iter = children.begin(); iter != children.end(); ++iter)
		delete *iter;
}

bool Node::updateNextColumn() {
	while (childrenColumnNext != N) {
		if (top[childrenColumnNext] == 0) ++childrenColumnNext;
		else return true;
	}
	return false;
}

int Node::getEnd(int* top, int** board) {
	if (id == 1 && userWin(x, y, M, N, board)) return 1;
	if (id == 2 && machineWin(x, y, M, N, board)) return 2;
	if (isTie(N, top)) return 3;
	return 0;
}

Tree::Tree() {
	root = new Node();
}

void Tree::reset() {
	delete root;
	root = new Node();
}

void Tree::moveRoot(int lastX, int lastY) {
	Node* newRoot = NULL;
	for (vector<Node*>::iterator iter = root->children.begin(); iter != root->children.end();) {
		if ((*iter)->x == lastX && (*iter)->y == lastY) {
			newRoot = *iter;
			newRoot->parent = NULL;
			root->children.erase(iter);
			break;
		}
		++iter;
	}
	delete root;
	if (newRoot == NULL) {
		if (lastX == -1) {
			newRoot = new Node();
			newRoot->id = 1;
		}
		else newRoot = new Node(lastX, lastY, 1, NULL);
	}
	root = newRoot;
}

void Tree::moveForward(int x, int y, int id, int* top, int** board) {
	if (noX == x - 1 && noY == y) top[y] = x - 1;
	else top[y] = x;
	board[x][y] = id;
}

Node* Tree::expand(Node* node) {
	Node* newNode = new Node(top[node->childrenColumnNext] - 1, node->childrenColumnNext, 3 - node->id, node);
	moveForward(newNode->x, newNode->y, newNode->id);
	newNode->isEnd = newNode->getEnd();
	node->childrenColumnNext += 1;
	node->children.push_back(newNode);
	return newNode;
}

Node* Tree::bestChild(Node* node) {
	double max = 0;
	Node* childNow = NULL;
	for (vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); ++iter) {
		double ucbScore = (double)(*iter)->winTimes / (double)(*iter)->times + UCBC * sqrt(2.0*log(node->times) / (double)(*iter)->times);
		if (ucbScore > max) {
			max = ucbScore;
			childNow = (*iter);
		}
	}
	moveForward(childNow->x, childNow->y, childNow->id);
	return childNow;
}

Node* Tree::treePolicy(Node* node) {
	while (!node->isEnd) {
		if (node->updateNextColumn()) return expand(node);
		else node = bestChild(node);
	}
	return node;
}

int Tree::defaultPolicy(Node* node) {
	int* topNow = new int[N];
	for (int i = 0; i < N; ++i) 
		topNow[i] = top[i];
	int** boardNow = new int*[M];
	for (int i = 0; i < M; ++i) {
		boardNow[i] = new int[N];
		for (int k = 0; k < N; ++k)
			boardNow[i][k] = board[i][k];
	}
	Node* simulationNode = node;
	while (!simulationNode->isEnd) {
		int columnNext = rand() % N;
		while (topNow[columnNext] == 0)
			columnNext = rand() % N;
		Node* newNode = new Node(topNow[columnNext]-1, columnNext, 3 - simulationNode->id, simulationNode);
		moveForward(newNode->x, newNode->y, newNode->id, topNow, boardNow);
		newNode->isEnd = newNode->getEnd(topNow, boardNow);
		if (simulationNode != node) delete simulationNode;
		simulationNode = newNode;
	}
	delete[] topNow;
	for (int i = 0; i < M; ++i) delete[] boardNow[i];
	delete[] boardNow;
	int isEnd = simulationNode->isEnd;
	if (simulationNode != node) delete simulationNode;
	return isEnd;
}

void Tree::backup(Node* node, int delta) {
	while (node->parent != NULL) {
		node->times += 1;
		if (node->id == delta) node->winTimes += 1;
		board[node->x][node->y] = 0;
		if(node->y == noY && top[node->y] == noX) top[node->y] += 2;
		else top[node->y] += 1;
		node = node->parent;
	}
	node->times += 1;
	if (node->id == delta) node->winTimes += 1;
}

Node* Tree::UCTSearch(int lastX, int lastY) {
	moveRoot(lastX, lastY);
	while ((double)(clock() - start) <= 2500) {
		Node* leafNode = treePolicy(root);
		int delta = defaultPolicy(leafNode);
		backup(leafNode, delta);
	}
	Node* decision = bestChild(root);
	moveRoot(decision->x, decision->y);
	return root;
}