//===============================================
//
// map管理リスト [map_list.h]
// Author :	Ibuki Okusada
//
//===============================================
#ifndef _MAP_LIST_H_
#define _MAP_LIST_H_

//===============================================
// テンプレートクラス(マップリスト)定義
//===============================================
template <typename T> class Cmaplist
{
public:

	// ヘッダーで使えるように名前を付ける! Iteretorって名前でiteretorを使いますよ
	using Iteretor = typename std::map<int, T>::iterator;
	
	Cmaplist();
	~Cmaplist();

	// メンバ関数
	bool Regist(int nId, T data);	// リストに連結
	bool Delete(int nId, T data);	// リストに連結
	bool Delete(int nId);			// IDから削除
	bool Delete(T data);			// データから削除
	bool Empty();					// リストが空かを確認
	void Clear();					// リストを空にする

	// 取得系関数
	std::map<int, T>& GetList();		// リストを取得
	int GetNum();					// リストのサイズ取得
	T Get(int nId);					// IDから本体を取得
	int Get(T data);				// データからIDを取得
	Cmaplist<T>::Iteretor DataFind(T data);			// データから検索
	bool IdFind(int nId);			// IDから検索
	Cmaplist<T>::Iteretor GetBegin();	// 先頭取得
	Cmaplist<T>::Iteretor GetEnd();		// 終端取得

private:

	// メンバ変数
	std::map<int, T> m_list;	// リスト本体
};

//===============================================
// コンストラクタ
//===============================================
template <typename T>
Cmaplist<T>::Cmaplist()
{
	m_list.clear();
}

//===============================================
// デストラクタ
//===============================================
template <typename T>
Cmaplist<T>::~Cmaplist()
{

}

//===============================================
// 先頭を取得
//===============================================
template <typename T>
typename Cmaplist<T>::Iteretor Cmaplist<T>::GetBegin()
{
	return m_list.begin();
}

//===============================================
// 終端を取得
//===============================================
template <typename T>
typename Cmaplist<T>::Iteretor Cmaplist<T>::GetEnd()
{
	return m_list.end();
}

//===============================================
// リストに挿入
//===============================================
template <typename T>
bool Cmaplist<T>::Regist(int nId, T data)
{
	// データが存在していない
	if (!IdFind(nId))
	{
		m_list[nId] = data;
	}

	return false;
}

//===============================================
// リストから削除
//===============================================
template <typename T>
bool Cmaplist<T>::Delete(int nId, T data)
{
	// 見つかった
	if (IdFind(nId))
	{
		m_list.erase(nId);
		return true;
	}

	return false;
}

//===============================================
// リストから削除(IDのみ
//===============================================
template <typename T>
bool Cmaplist<T>::Delete(int nId)
{
	// 見つかった
	if (IdFind(nId))
	{
		m_list.erase(nId);
		return true;
	}

	return false;
}

//===============================================
// リストから削除(データのみ)
//===============================================
template <typename T>
bool Cmaplist<T>::Delete(T data)
{
	T* pData = DataFind(T);

	// 見つかった
	if (pData != nullptr)
	{
		m_list.erase(data);
		return true;
	}

	return false;
}

//===============================================
// リストが空かを確認
//===============================================
template <typename T>
bool Cmaplist<T>::Empty()
{
	return m_list.empty();
}

//===============================================
// リストを空にする
//===============================================
template <typename T>
void Cmaplist<T>::Clear()
{
	m_list.clear();
}

//===============================================
// リストを取得
//===============================================
template <typename T>
std::map<int, T>& Cmaplist<T>::GetList()
{
	return m_list;
}

//===============================================
// 総数取得
//===============================================
template <typename T>
int Cmaplist<T>::GetNum()
{
	return m_list.size();
}

//===============================================
// データからIDを取得
//===============================================
template <typename T>
int Cmaplist<T>::Get(T data)
{
	Iteretor it = DataFind(data);
	if (it != m_list.end())
	{
		return it.first;;
	}

	return -1;
}

//===============================================
// IDからデータを取得
//===============================================
template <typename T>
T Cmaplist<T>::Get(int nId)
{
	Iteretor it = m_list.find(nId);

	if (it != m_list.end())
	{
		return it->second;
	}

	return nullptr;
}

//===============================================
// データが存在しているかをデータから確認する
//===============================================
template <typename T>
typename Cmaplist<T>::Iteretor Cmaplist<T>::DataFind(T data)
{
	return find_if(
		m_list.begin(),
		m_list.end(),
		[&data](const std::pair<int, T>& pair) {// 値が一致するかを判定
			return pair.second == data;}
	);
}

//===============================================
// データが存在しているかをIDから確認する
//===============================================
template <typename T>
bool Cmaplist<T>::IdFind(int nId)
{
	Iteretor it = m_list.find(nId);

	// 存在している
	if (it != m_list.end())
	{
		return true;
	}

	return false;
}

#endif // !_List_H_

