//==========================================================
//
// リストテンプレート [list.h]
// Author :	丹野竜之介
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
	void Regist(T data) { list->push_back(data); }																			// 要素登録	
	void Delete(T data) { if (!list->empty()) { list->erase(std::find(list->begin(), list->end(), data)); } }				// 要素削除	
	bool Empty() { return list->empty(); }																					// 空かの確認
	T Find(T data) { if (!list->empty()) { return *std::find(list->begin(), list->end(), data); } else { return NULL; } }	// 要素検索
	void Clear() { list->clear(); }																							// 要素全削除
	
	std::list<T> * GetList() { return list; }																				// リスト本体の取得
	int GetNum() {return (int)list->size(); }																				// 要素数の取得
	T Get(int nID) { return *std::next(list->begin(), nID); }																// 番号を指定した要素の取得
	static Clist<T> * Create() { Clist<T>* pList = DEBUG_NEW  Clist<T>; return pList; }											// 動的確保
private:
	std::list<T> * list;																									// 本体
};


#endif // !_List_H_

