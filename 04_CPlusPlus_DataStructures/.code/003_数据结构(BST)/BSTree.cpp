

#define SUCCESS   1 // 执行成功
#define ERROR	 -1 // 执行失败	         	
	
template<class T>	
class TreeNode{	
public:	
	T element;				//当前节点存储的数据	
	TreeNode<T>* pLeft;		//指向左子节点的指针	
	TreeNode<T>* pRight;	//指向右子节点的指针	
	TreeNode<T>* pParent;	//指向父结点的指针	
	
	TreeNode(T& ele){
		//初始化Node节点	
		memset(&element,0,sizeof(TreeNode));
		//为元素赋值	
		memcpy(&element,&ele,sizeof(T));
		pLeft = pRight = pParent = nullptr;
	}

	//重载== 比较两结点是否相等
	bool operator==(TreeNode<T>* node){ 
		return node->element == element?true:false;
	}
};
	



template<class T>	
class BSortTree{	
public:	
	BSortTree();//构造函数
	~BSortTree();//析构函数	
public:
	bool IsEmpty();//判断树是否为空
	DWORD Insert(T element);//新增节点
	void Delete(T element);//删除节点
	TreeNode<T>* Search(T element);//查找节点
	void InOrderTraverse(TreeNode<T>* pNode);//中序遍历
	void PreOrderTraverse(TreeNode<T>* pNode);//前序遍历
	void PostOrderTraverse(TreeNode<T>* pNode);//后序遍历
private:	
	TreeNode<T>* SearchNode(TreeNode<T>* pNode,T element);//获取以pNode为根的最小节点
	DWORD InsertNode(T element, TreeNode<T>* pNode);//新增节点
	TreeNode<T>* DeleteNode(TreeNode<T>* pNode);//删除节点
	void Clear(TreeNode<T>* pNode);//清空所有节点
	
	
	//获取以pNode为根的最大节点
	TreeNode<T>* GetMaxNode(TreeNode<T>* pNode)
	{
		if (pNode->pLeft == nullptr && pNode->pRight == nullptr) return pNode;
		else return GetMinNode(pNode->pRight);
	}
	
	//获取以pNode为根的最小节点
	TreeNode<T>* GetMinNode(TreeNode<T>* pNode)
	{
		if (pNode->pLeft == nullptr && pNode->pRight == nullptr) return pNode;
		else return GetMinNode(pNode->pLeft);
	}

private:	
	TreeNode<T>* m_pRoot;//根结点指针
	int size;//树中元素总个数
};
	
template<class T> 	
BSortTree<T>::BSortTree()	
{	
	m_pRoot = nullptr;
	size = 0;
}
template<class T> 	
BSortTree<T>::~BSortTree(){	
	Clear(m_pRoot);
}

template<class T> 	
DWORD BSortTree<T>::Insert(T element)	
{	
	//如果根节点为空
	if ( !m_pRoot )
	{
		m_pRoot = new TreeNode<T>(element);
		size++;
		return SUCCESS;
	}
	//如果根节点不为空
	return InsertNode(element, m_pRoot);
}
template<class T> 	
DWORD BSortTree<T>::InsertNode(T element, TreeNode<T>* pNode)	
{	
	//将元素封装到节点中
	TreeNode<T>* pNewNode = new TreeNode<T>(element);
	//如果element == 当前节点 直接返回
	if(element == pNode->element)
	{
		return SUCCESS;
	}
	//如果pNode的左子节点为nullptr 并且element < 当前节点
	if(pNode->pLeft == nullptr && element < pNode->element)
	{
		pNode->pLeft = pNewNode;
		pNewNode->pParent = pNode;
		size++;
		return SUCCESS;
	}
	//如果pNode的右子节点为nullptr 并且element > 当前节点
	if(pNode->pRight == nullptr && element > pNode->element){
		pNode->pRight = pNewNode;
		pNewNode->pParent = pNode;
		size++;
		return SUCCESS;
	}
	//如果element<当前节点 且当前节点的左子树不为空
	if(element < pNode->element)
	{
		InsertNode(element,pNode->pLeft);
	}
	else
	{
		InsertNode(element,pNode->pRight);
	}
	return SUCCESS;
}
	
template<class T> 	
void BSortTree<T>::Clear(TreeNode<T>* pNode)	
{	
	if(pNode!=nullptr)
	{
		Clear(pNode->pLeft);
		Clear(pNode->pRight);
		delete pNode;
		pNode=nullptr;
	}
}
	
template<class T> 	
bool BSortTree<T>::IsEmpty()	
{	
	return size==0?true:false;
}
	
template<class T> 	
TreeNode<T>* BSortTree<T>::Search(T element)	
{	
	return SearchNode(m_pRoot, element);
}
template<class T> 	
TreeNode<T>* BSortTree<T>::SearchNode(TreeNode<T>* pNode,T element)	
{	
	if(pNode == nullptr)	//如果节点为nullptr	
	{
		return nullptr;
	}
	else if(element == pNode->element)	//如果相等	
	{
		return pNode;
	}//如果比节点的元素小 向左找	
	else if(element < pNode->element)
	{
		return SearchNode(pNode->pLeft,element);
	}
	else	//如果比节点的元素大 向右找	
	{
		return SearchNode(pNode->pRight,element);
	}
}


template<class T> 	
void BSortTree<T>::Delete(T element)
{	
	TreeNode<T>* pnode = SearchNode(m_pRoot, element);
	if (!pnode) {
		return;
	}
	DeleteNode(pnode);

}
template<class T> 	
TreeNode<T>* BSortTree<T>::DeleteNode(TreeNode<T>* pNode)	
{	
	//情况一：叶/*子节点

	//	1、删除该节点

	//	2、将父节点(左或者右)指针置NULL

	//	情况2：只有一个子树

	//	1、删除该节点

	//	2、将父节点(左或者右)指针指向子树

	//	情况3：左右子树都有

	//	1、用右子树最小的节点取代源节点

	//	2、再递归删除最小节点*/
	
	// 还有根节点根节点

	if (pNode->pLeft==nullptr&&pNode->pRight==nullptr)
	{
		if (pNode == m_pRoot)
		{
			delete pNode;
		}
		else {
			(pNode->pParent->pLeft == pNode) ? pNode->pParent->pLeft = nullptr : pNode->pParent->pRight = nullptr;
			delete pNode;
		}
	}
	else if (pNode->pLeft != nullptr && pNode->pRight == nullptr) {
		if (pNode == m_pRoot)
		{
			m_pRoot = pNode->pLeft;
			delete pNode;
		}
		else {
			(pNode->pParent->pLeft == pNode) ? pNode->pParent->pLeft = pNode->pLeft : pNode->pParent->pRight = pNode->pLeft;
			delete pNode;
		}
	}
	else if (pNode->pLeft == nullptr && pNode->pRight != nullptr) {
		if (pNode == m_pRoot)
		{
			m_pRoot = pNode->pRight;
			delete pNode;
		}
		else {
			(pNode->pParent->pLeft == pNode) ? pNode->pParent->pLeft = pNode->pRight : pNode->pParent->pRight = pNode->pRight;
			delete pNode;
		}
	}
	else {
		TreeNode<T>* pnode = GetMinNode(pNode->pRight);
		if (pNode == m_pRoot)
		{
			m_pRoot = pNode->pRight;
			pnode->pLeft = pNode->pLeft;
			delete pNode;
		}
		else {
			(pNode->pParent->pLeft == pNode) ? pNode->pParent->pLeft = pNode->pRight : pNode->pParent->pRight = pNode->pRight;
			pnode->pLeft = pNode->pLeft;
			delete pNode;
		}
	}

	size--;
	return nullptr;
}

