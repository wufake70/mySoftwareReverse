#ifndef TREE_CPP
#define TREE_CPP

class Monster
{
public:
	int ID;
	int Level;
	char Name[20];
public:
	Monster()
	{
		ID = 0;
		Level = 0;
		memcpy(&Name, "null", 5);
	}
	Monster(int ID, int Level, char* Name)
	{
		this->ID = ID;
		this->Level = Level;
		memcpy(&this->Name, Name, strlen(Name) + 1);
	}
};



template<class T>	
class TreeNode{	
public:	
	T element;//当前节点存储的数据	
	TreeNode<T>* pLeft;//指向左子节点的指针	
	TreeNode<T>* pRight;//指向右子节点的指针	
	
	TreeNode(T& ele){	
	//初始化Node节点	
	memset(&element,0,sizeof(TreeNode));
	//为元素赋值	
	memcpy(&element,&ele,sizeof(T));
	pLeft = pRight = nullptr;
	}
};

// 搜索二叉树
template<class T>	
class BSortTree{	
public:	
	BSortTree();//构造函数	
	~BSortTree();//析构函数	
public:	
	void clear(TreeNode<T>* pNode);
	void InOrderTraverse(TreeNode<T>* pNode);//中序遍历	
	void PreOrderTraverse(TreeNode<T>* pNode);//前序遍历	
	void PostOrderTraverse(TreeNode<T>* pNode);//后序遍历	
	TreeNode<T>* GetRoot();//返回根节点	
	int GetDepth(TreeNode<T>* pNode);//返回某个节点的高度/深度	
private:	
	void Init();
private:	
	TreeNode<T>* m_pRoot;//根结点指针	
	int size;//树中元素总个数	
};
	
template<class T> 	
BSortTree<T>::BSortTree()	
{	
	Init();
}

template<class T> 	
BSortTree<T>::~BSortTree(){	
	clear(m_pRoot);
}

template<class T>
void BSortTree<T>::clear(TreeNode<T>* pNode)
{
	if (pNode == nullptr)
	{
		return;
	}
	else {

		if (pNode->pLeft)
		{
			clear(pNode->pLeft);
		}
		if (pNode->pRight)
		{
			clear(pNode->pRight);
		}
		delete pNode;
		pNode = nullptr;
		size--;
	}
}
template<class T> 	
void BSortTree<T>::Init()	
{	
	
	Monster m1(1,1,(char*)"刺猬");
	Monster m2(2,2, (char*)"野狼");
	Monster m3(3, 3, (char*)"野猪");
	Monster m4(4, 4, (char*)"士兵");
	Monster m5(5, 5, (char*)"火龙");
	Monster m6(6, 6, (char*)"独角兽");
	Monster m7(7, 7, (char*)"江湖大盗");
	
	
	TreeNode<Monster>* n1 = new TreeNode<Monster>(m1);
	TreeNode<Monster>* n2 = new TreeNode<Monster>(m2);
	TreeNode<Monster>* n3 = new TreeNode<Monster>(m3);
	TreeNode<Monster>* n4 = new TreeNode<Monster>(m4);
	TreeNode<Monster>* n5 = new TreeNode<Monster>(m5);
	TreeNode<Monster>* n6 = new TreeNode<Monster>(m6);
	TreeNode<Monster>* n7 = new TreeNode<Monster>(m7);
	
	m_pRoot = n5;
	n5->pLeft = n4;
	n5->pRight = n6;
	n4->pLeft = n1;
	n1->pRight = n2;
	n6->pLeft = n3;
	n3->pRight = n7;
	size = 7;
	/*	
	
	
	*/	
}
template<class T> 	
TreeNode<T>* BSortTree<T>::GetRoot()	
{	
	return m_pRoot;
}
template<class T>	
int BSortTree<T>::GetDepth(TreeNode<T>* pNode)	
{	
    if(pNode==NULL) 	
    {	
	return 0;  	
    }
    else  	
    {  	
        int m = GetDepth(pNode->pLeft);  	
        int n = GetDepth(pNode->pRight);  	
        return (m > n) ? (m+1) : (n+1);   	
    }  	
}

template<class T> 
void BSortTree<T>::InOrderTraverse(TreeNode<T>* pNode)	
{	
	
	//递归 中序遍历所有怪物,列出怪的名字,左 中 右
	Monster m;
	if (pNode == nullptr)
	{
		return ;
	}
	else {
		if (pNode->pLeft)
		{
			InOrderTraverse(pNode->pLeft);
		}
		m = pNode->element;
		cout << m.ID << endl;
		if (pNode->pRight)
		{
			InOrderTraverse(pNode->pRight);
		}
	}
	
}


//先序 5 4 1 2 6 3 7

//中序 1 2 4 5 3 7 6

//后序 2 1 4 7 3 6 5
	
template<class T> 	
void BSortTree<T>::PreOrderTraverse(TreeNode<T>* pNode)	
{	
	//前序遍历所有怪物,列出怪的名字	
	Monster m;
	if (pNode == nullptr)
	{
		return;
	}
	else {
		m = pNode->element;
		cout << m.ID << endl;
		if (pNode->pLeft)
		{
			PreOrderTraverse(pNode->pLeft);
		}
		if (pNode->pRight)
		{
			PreOrderTraverse(pNode->pRight);
		}
	}
}
	
template<class T> 	
void BSortTree<T>::PostOrderTraverse(TreeNode<T>* pNode)	
{	
	
	//后序遍历所有怪物,列出怪的名字	
	Monster m;
	if (pNode == nullptr)
	{
		return;
	}
	else {
		
		if (pNode->pLeft)
		{
			PostOrderTraverse(pNode->pLeft);
		}
		if (pNode->pRight)
		{
			PostOrderTraverse(pNode->pRight);
		}
		m = pNode->element;
		cout << m.ID << endl;
	}
}

#endif // TREE_CPP