#ifndef SIMPLELINKEDLIST_H
#define SIMPLELINKEDLIST_H

#include <Windows.h>
#include <cstdarg>

#define SUCCESS           1 // 执行成功	
#define ERROR            -1 // 执行失败	
#define INDEX_IS_ERROR   -2 // 错误的索引号	
#define BUFFER_IS_EMPTY  -3 // 缓冲区已空	



template <class T_ELE>	
class LinkedList	
{	
public:	
	LinkedList();	
	~LinkedList();	
public:	
	BOOL  empty();										//判断链表是否为空 空返回1 非空返回0	
	void  clear();										//清空链表	
	DWORD at(IN DWORD dwIndex,OUT T_ELE& Element);		//根据索引获取元素	
	DWORD append(IN T_ELE Element);						//新增元素	
	DWORD append(DWORD argc, IN T_ELE ...);						//新增元素	
	DWORD insert(IN DWORD dwIndex, IN T_ELE Element);	//根据索引新增元素	
	DWORD erase(IN DWORD dwIndex);						//根据索引删除元素	
	DWORD size();										//获取链表中元素的数量	

private:	
	typedef struct _NODE 	
	{	
		T_ELE  Data;	
		_NODE *pNext;	
	}NODE,*PNODE;	

	PNODE m_pList;		//链表头指针，指向第一个节点	
	DWORD m_dwLength;	//元素的数量	

private:
	//PNODE GetIndexCurrentNode(DWORD dwIndex);	//获取索引为dwIndex的指针	
	//PNODE GetIndexPreviousNode(DWORD dwIndex);	//获取索引为dwIndex的前一个节点指针	
	//PNODE GetIndexNextNode(DWORD dwIndex);	//获取索引为dwIndex的后一个节点指针	
	
	//获取dwIndex前面节点的地址	
	PNODE GetIndexPreviousNode(DWORD dwIndex)
	{
		// 就是一个循环	
		PNODE pnode = m_pList;
		for (int i = 0; i < dwIndex-1; i++)
		{
			pnode = pnode->pNext;
		}
		return pnode;
	}

	//获取dwIndex节点的地址	
	PNODE GetIndexCurrentNode(DWORD dwIndex)
	{
		// 就是一个循环	
		PNODE pnode=m_pList;
		for (int i = 0; i < dwIndex; i++)
		{
			pnode = pnode->pNext;
		}
		return pnode;

	}

	//获取dwIndex后面节点的地址	
	PNODE GetIndexNextNode(DWORD dwIndex)
	{
		// 就是一个循环	
		PNODE pnode = m_pList;
		for (int i = 0; i < dwIndex+1; i++)
		{
			pnode = pnode->pNext;
		}
		return pnode;
	}

};

#include "SimpleLinkedList.cpp"
#endif // SIMPLELINKEDLIST_H
