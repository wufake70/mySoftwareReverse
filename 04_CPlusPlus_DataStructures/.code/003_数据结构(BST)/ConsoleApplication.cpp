

// ConsoleApplication.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。												
//												

#include <iostream>	
#include <Windows.h>
#include "Vector.h"
#include "SimpleLinkedList.h"
#include "BSTree.cpp"


#define uchar unsigned char												
#define ushort unsigned short												
#define uint unsigned int		

using namespace std;



// 测试代码	
void TestInsert()
{
	//12 8 5 9 17 15 13

	BSortTree<int> tree;

	tree.Insert(12);
	tree.Insert(8);
	tree.Insert(5);
	tree.Insert(9);
	tree.Insert(17);
	tree.Insert(15);
	tree.Insert(13);

	tree.Insert(16);
	tree.Insert(18);
	tree.Delete(17);

	TreeNode<int>* p = tree.Search(16);
	if(p) cout << p->element << endl;
}

class B {
public:
	int x;
	int y;
	B()
	{
		this->x = 9;
		this->y = 9;
	}
	B(int x,int y) {
		this->x = x;
		this->y = y;
	}
	// *** 使用二叉搜索树 存储必须要 运算符重载 *** 
	bool operator==(B bptr){
		return this->y == bptr.y ? true : false;

	}
	bool operator>(B bptr) {
		return this->y > bptr.y ? true : false;

	}
	bool operator<(B bptr) {
		return this->y < bptr.y ? true : false;

	}
};
// 测试 复杂类型
void TestInsert(int a)
{
	BSortTree<B>* tree = new BSortTree<B>;
	B* arr[7] = {
		new B(5,5),
		new B(1,1),
		new B(6,6),
		new B(4,4),
		new B(2,2),
		new B(3,3),
		new B(7,7),
	};
	//if (arr[1][0] < arr[2][0]) cout << "ok" << endl;
	for (int i = 0; i < 7; i++)
	{
		tree->Insert(arr[i][0]);
	}

	B t = tree->Search(arr[6][0])->pParent->pParent->pLeft->pRight->pLeft->element;
	cout << t.y << endl;
}

int main(uint argc, char argv[]...) {
	TestInsert(1);

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单												
// 调试程序: F5 或调试 >“开始调试”菜单												

// 入门使用技巧: 												
//   1. 使用解决方案资源管理器窗口添加/管理文件												
//   2. 使用团队资源管理器窗口连接到源代码管理												
//   3. 使用输出窗口查看生成输出和其他消息												
//   4. 使用错误列表窗口查看错误												
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目												
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件												
