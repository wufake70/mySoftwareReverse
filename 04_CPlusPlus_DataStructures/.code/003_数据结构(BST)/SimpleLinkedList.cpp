#ifndef SIMPLELINKEDLIST_CPP
#define SIMPLELINKEDLIST_CPP

#include "SimpleLinkedList.h"


//无参构造函数 初始化成员	
template<class T_ELE> 
LinkedList<T_ELE>::LinkedList()	
:m_pList(nullptr),m_dwLength(0)	
{	

}	
//析构函数 清空元素	
template<class T_ELE> 
LinkedList<T_ELE>::~LinkedList()	
{	
	// 1. 判断链表是否为空	
	if (m_dwLength == 0)
	{
		return;
	}
	// 2. 循环删除链表中的节点，从前往后删
	int i = 0;
	PNODE pnode = nullptr;
	for (i = 0; i < m_dwLength; i++)
	{
		pnode = m_pList->pNext;
		delete m_pList;
		m_pList = pnode;
	}
	// 3. 删除最后一个节点并将链表长度置为0	
	m_dwLength -= i;
	return;
}	
//判断链表是否为空	
template<class T_ELE> 
BOOL LinkedList<T_ELE>::empty()	
{	
	return m_dwLength == 0 ? true : false;

}	

//清空链表	
template<class T_ELE> 
void LinkedList<T_ELE>::clear()	
{	
	// 1. 判断链表是否为空	
	if (m_dwLength == 0)
	{
		return ;
	}
	// 2. 循环删除链表中的节点，从前往后删
	int i = 0;
	PNODE pnode = nullptr;
	for (i = 0; i < m_dwLength; i++)
	{
		pnode = m_pList->pNext;
		delete m_pList;
		m_pList = pnode;
	}
	// 3. 删除最后一个节点并将链表长度置为0	
	m_dwLength -= i;
	return ;
	
}	

//根据索引获取元素	
template<class T_ELE> 
DWORD LinkedList<T_ELE>::at(IN DWORD dwIndex,OUT T_ELE& Element)	
{	
	// 1. 判断索引是否有效	
	if (dwIndex<0 || dwIndex>=m_dwLength)
	{
		std::cout << "Error: Index error." << endl;
		return INDEX_IS_ERROR;
	}
	
	// 2. 取得索引指向的节点	
	PNODE pnode = GetIndexCurrentNode(dwIndex);
	
	// 3. 将索引指向节点的值复制到OUT参数	
	memcpy(&Element, &pnode->Data, sizeof T_ELE);
	return SUCCESS;
	
}	

//在链表尾部新增节点	
template<class T_ELE> 
DWORD LinkedList<T_ELE>::append(IN T_ELE Element)	
{	
	// 1. 判断链表是否为空	
	if (m_dwLength == 0)
	{
		m_pList = new NODE;
		memcpy(&m_pList->Data, &Element, sizeof T_ELE);
		m_pList->pNext = nullptr;
		m_dwLength++;
		return SUCCESS;
	}
	// 2. 如果链表中已经有元素	
	PNODE pnode=GetIndexCurrentNode(m_dwLength-1);
	pnode->pNext = new NODE;
	pnode=pnode->pNext;

	memcpy(&pnode->Data, &Element, sizeof T_ELE);
	pnode->pNext = nullptr;
	m_dwLength++;
	return SUCCESS;
}

template<class T_ELE>
DWORD LinkedList<T_ELE>::append(DWORD argc, IN T_ELE ...)
{
	va_list va_argvs;
	va_start(va_argvs, argc);
	T_ELE va_argv;
	PNODE pnode = nullptr;

	// 1. 判断链表是否为空	
	if (m_dwLength == 0)
	{
		m_pList = new NODE;
		pnode = m_pList;
		for (int i = 0; i < argc; i++)
		{
			va_argv = va_arg(va_argvs, T_ELE);
			memcpy(&pnode->Data, &va_argv, sizeof T_ELE);
			if (i == (argc - 1))
			{
				pnode->pNext = nullptr;
				m_dwLength++;
				break;
			}
			pnode->pNext = new NODE;
			pnode = pnode->pNext;
			m_dwLength++;
		}
		return SUCCESS;
	}

	// 2. 如果链表中已经有元素	
	pnode = GetIndexCurrentNode(m_dwLength - 1);
	for (int i = 0; i < argc; i++)
	{
		va_argv = va_arg(va_argvs, T_ELE);

		pnode->pNext = new NODE;
		pnode = pnode->pNext;
		memcpy(&pnode->Data, &va_argv, sizeof T_ELE);
		if (i == (argc - 1))
		{
			pnode->pNext = nullptr;	
			m_dwLength++;
			break;
		}
		m_dwLength++;
	}
	return SUCCESS;

}

//将节点新增到指定索引的位置	0 1 2 3 4	
template<class T_ELE> 
DWORD LinkedList<T_ELE>::insert(IN DWORD dwIndex, IN T_ELE Element)	
{	
	//  1. 判断链表是否为空	
	if (m_dwLength == 0)
	{
		m_pList = new NODE;
		memcpy(&m_pList->Data, &Element, sizeof T_ELE);
		m_pList->pNext = nullptr;
		m_dwLength++;
		return SUCCESS;
	}

	//  2. 判断索引值是否有效	
	if (dwIndex < 0 || dwIndex > m_dwLength)
	{
		std::cout << "Error: Index error." << endl;
		return INDEX_IS_ERROR;
	}
	
	//  3. 如果索引为0	
	//  4. 如果索引为链表尾	
	//  5. 如果索引为链表中	
	if (dwIndex==0)
	{
		PNODE pnode = new NODE;
		pnode->pNext = m_pList;
		memcpy(&pnode->Data,&Element,sizeof T_ELE);
		m_pList = pnode;
		m_dwLength++;
		return SUCCESS;
	}
	else if (dwIndex == m_dwLength)
	{
		PNODE pnode = new NODE;
		pnode->pNext = nullptr;
		memcpy(&pnode->Data, &Element, sizeof T_ELE);

		PNODE pnode_ = GetIndexCurrentNode(m_dwLength-1);
		pnode_->pNext = pnode;
		m_dwLength++;
		return SUCCESS;
	}
	else {
		PNODE pnode = new NODE;
		memcpy(&pnode->Data, &Element, sizeof T_ELE);

		PNODE _pnode = GetIndexPreviousNode(dwIndex);
		// _pnode->pNext = pnode; 该操作会切断 后面链表
		pnode->pNext = _pnode->pNext;

		 _pnode->pNext = pnode;
		m_dwLength++;
		return SUCCESS;
	}
}	

//根据索引删除节点	
template<class T_ELE> 
DWORD LinkedList<T_ELE>::erase(IN DWORD dwIndex)	
{	
	
	//  1. 判断链表是否为空	
	if (m_dwLength == 0)
	{
		std::cout << "Error: Empty LinkedList." << endl;
		return ERROR;
	}
	
	//  2. 判断索引值是否有效	
	if (dwIndex < 0 || dwIndex >= m_dwLength)
	{
		std::cout << "Error: Index error." << endl;
		return INDEX_IS_ERROR;
	}

	//  3. 如果链表中只有头节点，且要删除头节点	
	//  4. 如果要删除头节点	
	//  5. 如果是其他情况	
	if (m_dwLength == 1)
	{
		delete m_pList;
		m_pList = nullptr;
		m_dwLength--;
		return SUCCESS;
	}
	else if (dwIndex==0) {
		PNODE pnode = GetIndexCurrentNode(1);
		delete m_pList;
		m_pList = pnode;
		m_dwLength--;
		return SUCCESS;
	}
	else {
		PNODE _pnode = GetIndexPreviousNode(dwIndex);
		PNODE pnode = _pnode->pNext;
		_pnode->pNext = pnode->pNext;
		delete pnode;
		m_dwLength--;
		return SUCCESS;
	}
	
}	

//获取链表中节点的数量	
template<class T_ELE> 
DWORD LinkedList<T_ELE>::size()	
{	
	return m_dwLength;
}	


#endif // SIMPLELINKEDLIST_CPP