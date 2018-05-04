#pragma once

#include <vector>
#include <cmath>
#include <ctime>
#include "Judge.h"
#include <random>

using namespace std;

const double UCBC = 0.5;//信心上限算法中使用的比例系数
extern int* top;//top数组
extern int** board;//board数组
extern int M, N;//棋盘行数M，列数N
extern int noX, noY;//不可放子的位置
extern clock_t start;//这一回合的开始时间

void updateBoard(int MM, int NN, const int* newTop, int lastX, int lastY, int noXX, int noYY,int** newBoard, bool initialize);//每回合开始时执行该函数更新局面

struct Node {
	int x, y, id;//该节点新增落子的位置及其编号（1或2，1代表自己，2代表对方）
	int winTimes, times;//当id=1时为用户获胜次数和被模拟的次数，id=2时为电脑获胜次数
	vector<Node*> children;//子节点
	Node* parent;//父节点
	int childrenColumnNext;//下一个尚未被模拟过的列序号，若全都被模拟过至少一次则返回-1
	int isEnd;//是否该节点时游戏已结束，0为未结束，1为胜利，2为失败，3为平局

	Node();
	Node(int x, int y, int id, Node* parent);//构造函数
	~Node();//析构函数
	bool updateNextColumn();//将childrenColumnNext更新为下一个未被扩展过的可以落子点的列序号。若没有可以扩展的子节点则更新为-1并返回false，否则返回true
	int getEnd(int* top = top, int** board = board);//获取游戏是否结束的信息
};

class Tree {
	Node* root;

public:
	Tree();//默认构造函数
	void reset();
	void moveRoot(int lastX, int lastY);//执行该函数以移动根节点
	void moveForward(int x, int y, int id, int* top = top, int** board = board);//（在扩展中使用）将top数组和board数组以被扩展的节点或子节点更新
	Node* expand(Node* node);//扩展
	Node* bestChild(Node* node);//最优子节点
	Node* treePolicy(Node* node);//寻找最优的需要模拟的节点
	int defaultPolicy(Node* node);//进行模拟并返回收益值
	void backup(Node* node, int delta);//回溯，更新节点
	Node* UCTSearch(int lastX, int lastY);
};

extern Tree tree;