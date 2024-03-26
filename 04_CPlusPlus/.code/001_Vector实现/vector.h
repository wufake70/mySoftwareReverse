#ifndef VECTOR_H
#define VECTOR_H

//#include <iostream>
#include <Windows.h>  // 为了使用 DWORD 类型
#include <string.h>
#include <cstdarg>

#define SUCCESS           	 1 // 成功								
#define ERROR            	-1 // 失败								
#define MALLOC_ERROR		-2 // 申请内存失败								
#define INDEX_ERROR		 	-3 // 错误的索引号								

using namespace std;


template <class T_ELE>
class Vector
{
public:
	Vector();
	Vector(DWORD dwSize);
	~Vector();
public:
	T_ELE& operator[](DWORD dwIndex);

	DWORD	at(DWORD dwIndex, OUT T_ELE* pEle);		//根据给定的索引得到元素				
	DWORD   append(T_ELE Element);					//将元素存储到容器最后一个位置	
	DWORD	append(DWORD argc, T_ELE...);
	VOID	pop();									//删除最后一个元素				
	VOID	pop(DWORD dwLen);						//删除最后一个元素				
	DWORD	insert(DWORD dwIndex, T_ELE Element);	//向指定位置新增一个元素				
	DWORD	capacity();								//返回在不增容的情况下，还能存储多少元素				
	VOID	clear();								//清空所有元素				
	BOOL	empty();								//判断Vector是否为空 返回true时为空				
	VOID	erase(DWORD dwIndex);					//删除指定元素				
	DWORD	size();									//返回Vector元素数量的大小				
private:
	BOOL	expand();
private:
	DWORD  m_dwIndex;							//下一个可用索引				
	DWORD  m_dwIncrement;						//每次增容的大小				
	DWORD  m_dwLen;								//当前容器的长度				
	DWORD  m_dwInitSize;						//默认初始化大小				
	T_ELE* m_pVector;							//容器指针				
};
#include "vector.cpp"
#endif  // VECTOR_H