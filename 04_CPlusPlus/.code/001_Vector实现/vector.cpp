#include "vector.h"

template <class T_ELE>
Vector<T_ELE>::Vector()
	:m_dwInitSize(100), m_dwIncrement(5)
{
	//1.创建长度为m_dwInitSize个T_ELE对象	
	this->m_pVector = new T_ELE[this->m_dwInitSize];
	//2.将新创建的空间初始化										
	memset(this->m_pVector, 0, this->m_dwInitSize*(sizeof T_ELE));
	//3.设置其他值			
	this->m_dwLen = this->m_dwInitSize;
	this->m_dwIndex = 0;
}
template <class T_ELE>
Vector<T_ELE>::Vector(DWORD dwSize)
	:m_dwIncrement(5)
{
	this->m_dwInitSize = dwSize;
	//1.创建长度为m_dwInitSize个T_ELE对象	
	this->m_pVector = new T_ELE[this->m_dwInitSize];
	//2.将新创建的空间初始化										
	memset(this->m_pVector, 0, this->m_dwInitSize * (sizeof T_ELE));
	//3.设置其他值			
	this->m_dwLen = this->m_dwInitSize;
	this->m_dwIndex = 0;

}											
template <class T_ELE>
Vector<T_ELE>::~Vector()
{
	//释放空间 delete[]										
	delete[] this->m_pVector;
	this->m_pVector = nullptr;

}

template <class T_ELE>
BOOL Vector<T_ELE>::expand()
{
	// 1. 计算增加后的长度		
	int newLen= this->m_dwLen+this->m_dwIncrement;
	// 2. 申请空间										
	T_ELE* temp = new T_ELE[newLen];
	memset(temp,0,newLen);
	if (temp)
	{
		// 3. 将数据复制到新的空间	
		// *** 编译时选择 release版	***								
		memcpy(temp, this->m_pVector, this->m_dwLen*(sizeof T_ELE));

		// 4. 释放原来空间										
		delete[] this->m_pVector;
		this->m_pVector = temp;
		// 5. 为各种属性赋值
		this->m_dwLen += this->m_dwIncrement;
		return SUCCESS;
	}
	return ERROR;
}

template <class T_ELE>
DWORD  Vector<T_ELE>::append(T_ELE Element)
{
	//1.判断是否需要增容，如果需要就调用增容的函数										
	if ((this->m_dwIndex + 1) > this->m_dwLen)
	{
		if (this->expand()!= SUCCESS)
		{
			return MALLOC_ERROR;
		}
	}
	//2.将新的元素复制到容器的最后一个位置										
	this->m_pVector[this->m_dwIndex] = Element;
	// std::cout << this->m_dwIndex << " " << this->m_pVector[this->m_dwIndex] << std::endl;
	//3.修改属性值										
	this->m_dwIndex++;
	return SUCCESS;
}

// *** 运算符重载 [],返回值设为 引用，可以向指针一样操作 ***
template<class T_ELE>
T_ELE& Vector<T_ELE>::operator[](DWORD dwIndex)
{	
	// 设置在合理索引范围 操作
	if (dwIndex >= this->m_dwLen||dwIndex<0)
	{
		int error = INDEX_ERROR;
		std::cout << "Error: inedex error." << std::endl;
		return error;
	}

	if (dwIndex == this->m_dwIndex)
	{
		this->m_dwIndex++;
		return this->m_pVector[dwIndex];
	}
	return this->m_pVector[dwIndex];
}


template <class T_ELE>
DWORD  Vector<T_ELE>::insert(DWORD dwIndex, T_ELE Element)
{
	//1.判断是否需要增容，如果需要就调用增容的函数										
	if ((this->m_dwIndex + 1) > this->m_dwLen)
	{
		if (this->expand() != SUCCESS)
		{
			return MALLOC_ERROR;
		}
	}

	//2.判断索引是否在合理区间										
	if (dwIndex == this->m_dwIndex)
	{
		//4.将Element元素复制到dwIndex位置										
		this->m_pVector[this->m_dwIndex] = Element;
		//5.修改属性值				
		this->m_dwIndex++;
		return SUCCESS;
	}
	else if(dwIndex > this->m_dwIndex)
	{
		std::cout << "Error: Index out of range." << std::endl;
		return INDEX_ERROR;
	}
	else if (dwIndex < 0)
	{
		std::cout << "Error: Index error." << std::endl;
		return INDEX_ERROR;
	}

	//3.将dwIndex只后的元素后移		
	T_ELE temp = this->m_pVector[dwIndex + 1];
	for (int i = dwIndex; i < (int)this->m_dwIndex+1;i++)
	{
		if (i == dwIndex) {
			this->m_pVector[i + 1] = this->m_pVector[i];
		}
		else {
			temp += this->m_pVector[i + 1];
			this->m_pVector[i + 1] = temp - this->m_pVector[i + 1];
			temp -= this->m_pVector[i + 1];
		}
	}

	//4.将Element元素复制到dwIndex位置										
	this[0][dwIndex] = Element;

	//5.修改属性值				
	this->m_dwIndex++;
	return SUCCESS;
}

template <class T_ELE>
DWORD Vector<T_ELE>::at(DWORD dwIndex, OUT T_ELE* pEle)
{
	//判断索引是否在合理区间										
	if (dwIndex >= this->m_dwIndex)
	{
		std::cout << "Error: Index out of range." << std::endl;
		return INDEX_ERROR;
	}
	else if (dwIndex < 0)
	{
		std::cout << "Error: Index error." << std::endl;
		return INDEX_ERROR;
	}

	//将dwIndex的值复制到pEle指定的内存										
	pEle[0] = this->m_pVector[dwIndex];
	//pEle[0] = (T_ELE)&this->m_pVector[dwIndex];
	return SUCCESS;
}

template<class T_ELE>
void Vector<T_ELE>::pop()
{
	if (this->m_dwIndex != 0)
	{
		this->m_pVector[--this->m_dwIndex] = 0;
	}
}

template<class T_ELE>
void Vector<T_ELE>::pop(DWORD dwLen)
{
	if (this->m_dwIndex != 0&&this->m_dwIndex>=dwLen)
	{
		while(dwLen--) this->m_pVector[--this->m_dwIndex] = 0;
	}
}

template<class T_ELE>
DWORD Vector<T_ELE>::capacity()
{
	return this->m_dwLen - this->m_dwIndex;
}

template<class T_ELE>
void Vector<T_ELE>::clear()
{	
	DWORD dwLen = this->m_dwIndex;
	while (dwLen--) this->m_pVector[--this->m_dwIndex] = 0;
}

template<class T_ELE>
BOOL Vector<T_ELE>::empty()
{
	if (this->m_dwIndex == 0) return true;
	return false;
}

template<class T_ELE>
void Vector<T_ELE>::erase(DWORD dwIndex)
{
	//1.判断索引是否在合理区间										
	if ((dwIndex+1) >this->m_dwIndex)
	{
		std::cout << "Error: Index out of range." << std::endl;
		return;
	}
	else if (dwIndex < 0)
	{
		std::cout << "Error: Index error." << std::endl;
		return;
	}

	//2.将dwIndex只后的元素前移		
	T_ELE temp = this->m_pVector[dwIndex+1];
	for (int i = dwIndex; i < (int)this->m_dwIndex + 1; i++)
	{
		if (i == dwIndex) {
			this->m_pVector[i] = this->m_pVector[i+1];
		}
		else {
			temp += this->m_pVector[i + 1];
			this->m_pVector[i] = temp - this->m_pVector[i];
			temp -= this->m_pVector[i];
		}
	}

	//3.修改属性值				
	this->m_dwIndex--;
}

template<class T_ELE>
DWORD Vector<T_ELE>::size()
{
	return this->m_dwIndex;
}

