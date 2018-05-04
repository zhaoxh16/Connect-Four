#pragma once

#include <vector>
#include <cmath>
#include <ctime>
#include "Judge.h"
#include <random>

using namespace std;

const double UCBC = 0.5;//���������㷨��ʹ�õı���ϵ��
extern int* top;//top����
extern int** board;//board����
extern int M, N;//��������M������N
extern int noX, noY;//���ɷ��ӵ�λ��
extern clock_t start;//��һ�غϵĿ�ʼʱ��

void updateBoard(int MM, int NN, const int* newTop, int lastX, int lastY, int noXX, int noYY,int** newBoard, bool initialize);//ÿ�غϿ�ʼʱִ�иú������¾���

struct Node {
	int x, y, id;//�ýڵ��������ӵ�λ�ü����ţ�1��2��1�����Լ���2����Է���
	int winTimes, times;//��id=1ʱΪ�û���ʤ�����ͱ�ģ��Ĵ�����id=2ʱΪ���Ի�ʤ����
	vector<Node*> children;//�ӽڵ�
	Node* parent;//���ڵ�
	int childrenColumnNext;//��һ����δ��ģ���������ţ���ȫ����ģ�������һ���򷵻�-1
	int isEnd;//�Ƿ�ýڵ�ʱ��Ϸ�ѽ�����0Ϊδ������1Ϊʤ����2Ϊʧ�ܣ�3Ϊƽ��

	Node();
	Node(int x, int y, int id, Node* parent);//���캯��
	~Node();//��������
	bool updateNextColumn();//��childrenColumnNext����Ϊ��һ��δ����չ���Ŀ������ӵ������š���û�п�����չ���ӽڵ������Ϊ-1������false�����򷵻�true
	int getEnd(int* top = top, int** board = board);//��ȡ��Ϸ�Ƿ��������Ϣ
};

class Tree {
	Node* root;

public:
	Tree();//Ĭ�Ϲ��캯��
	void reset();
	void moveRoot(int lastX, int lastY);//ִ�иú������ƶ����ڵ�
	void moveForward(int x, int y, int id, int* top = top, int** board = board);//������չ��ʹ�ã���top�����board�����Ա���չ�Ľڵ���ӽڵ����
	Node* expand(Node* node);//��չ
	Node* bestChild(Node* node);//�����ӽڵ�
	Node* treePolicy(Node* node);//Ѱ�����ŵ���Ҫģ��Ľڵ�
	int defaultPolicy(Node* node);//����ģ�Ⲣ��������ֵ
	void backup(Node* node, int delta);//���ݣ����½ڵ�
	Node* UCTSearch(int lastX, int lastY);
};

extern Tree tree;