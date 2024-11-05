//==========================================================
//
// ���X�g�e���v���[�g [list.h]
// Author :	�O�엳�V��
//
//==========================================================
#include "main.h"
#include <list>
#include <iterator>

#ifndef _List_H_
#define _List_H_


template <typename T> class Clist
{
public:
	Clist() { list = DEBUG_NEW std::list<T>; }
	~Clist() { delete list; list = nullptr; };
	void Regist(T data) { list->push_back(data); }																			// �v�f�o�^	
	void Delete(T data) { if (!list->empty()) { list->erase(std::find(list->begin(), list->end(), data)); } }				// �v�f�폜	
	bool Empty() { return list->empty(); }																					// �󂩂̊m�F
	T Find(T data) { if (!list->empty()) { return *std::find(list->begin(), list->end(), data); } else { return NULL; } }	// �v�f����
	void Clear() { list->clear(); }																							// �v�f�S�폜
	
	std::list<T> * GetList() { return list; }																				// ���X�g�{�̂̎擾
	int GetNum() {return (int)list->size(); }																				// �v�f���̎擾
	T Get(int nID) { return *std::next(list->begin(), nID); }																// �ԍ����w�肵���v�f�̎擾
	static Clist<T> * Create() { Clist<T>* pList = DEBUG_NEW  Clist<T>; return pList; }											// ���I�m��
private:
	std::list<T> * list;																									// �{��
};


#endif // !_List_H_

