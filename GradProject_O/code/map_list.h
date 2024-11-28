//===============================================
//
// map�Ǘ����X�g [map_list.h]
// Author :	Ibuki Okusada
//
//===============================================
#ifndef _MAP_LIST_H_
#define _MAP_LIST_H_

//===============================================
// �e���v���[�g�N���X(�}�b�v���X�g)��`
//===============================================
template <typename T> class Cmaplist
{
public:
	
	Cmaplist();
	~Cmaplist();

	// �����o�֐�
	bool Regist(int nId, T data);	// ���X�g�ɘA��
	bool Delete(int nId, T data);	// ���X�g�ɘA��
	bool Delete(int nId);			// ID����폜
	bool Delete(T* data);			// �f�[�^����폜
	bool Empty();					// ���X�g���󂩂��m�F
	void Clear();					// ���X�g����ɂ���

	// �擾�n�֐�
	std::list<T*>& GetList();		// ���X�g���擾
	int GetNum();					// ���X�g�̃T�C�Y�擾
	T* Get(int nId);					// ID����{�̂��擾
	int* Get(T data);				// �f�[�^����ID���擾
	T DataFind(T data);				// �f�[�^���猟��
	bool IdFind(int nId);			// ID���猟��

private:

	// �����o�ϐ�
	std::map<int, T> m_list;	// �{��
};

//===============================================
// �R���X�g���N�^
//===============================================
template <typename T>
Cmaplist<T>::Cmaplist()
{
	m_list.clear();
}

//===============================================
// �f�X�g���N�^
//===============================================
template <typename T>
Cmaplist<T>::~Cmaplist()
{

}


//===============================================
// ���X�g�ɑ}��
//===============================================
template <typename T>
bool Cmaplist<T>::Regist(int nId, T data)
{
	// �f�[�^�����݂��Ă��Ȃ�
	if (!IdFind(nId))
	{
		m_list[nId] = data;
	}

	return false;
}

//===============================================
// ���X�g����폜
//===============================================
template <typename T>
bool Cmaplist<T>::Delete(int nId, T data)
{
	// ��������
	if (IdFind(nId))
	{
		m_list.erase(nId);
		return true;
	}

	return false;
}

//===============================================
// ���X�g����폜(ID�̂�
//===============================================
template <typename T>
bool Cmaplist<T>::Delete(int nId)
{
	int* pId = IdFind(nId);

	// ��������
	if (*pId != m_list.end())
	{
		m_list.erase(nId);
		return true;
	}

	return false;
}

//===============================================
// ���X�g����폜(�f�[�^�̂�)
//===============================================
template <typename T>
bool Cmaplist<T>::Delete(T* data)
{
	T* pData = DataFind(T);

	// ��������
	if (pData != nullptr)
	{
		m_list.erase(data);
		return true;
	}

	return false;
}

//===============================================
// ���X�g���󂩂��m�F
//===============================================
template <typename T>
bool Cmaplist<T>::Empty()
{
	return m_list.empty();
}

//===============================================
// ���X�g����ɂ���
//===============================================
template <typename T>
void Cmaplist<T>::Clear()
{
	m_list.clear();
}

//===============================================
// ���X�g���擾
//===============================================
template <typename T>
std::list<T*>& Cmaplist<T>::GetList()
{
	return m_list;
}

//===============================================
// �����擾
//===============================================
template <typename T>
int Cmaplist<T>::GetNum()
{
	return m_list.size();
}

//===============================================
// �f�[�^����ID���擾
//===============================================
template <typename T>
int* Cmaplist<T>::Get(T data)
{
	for (int i = 0; i < GetNum(); i++)
	{
		if (T == m_list[i])
		{
			return m_list[i].first;
		}
	}

	return nullptr;
}

//===============================================
// ID����f�[�^���擾
//===============================================
template <typename T>
T* Cmaplist<T>::Get(int nId)
{
	std::map<int, T>::iterator it = m_list.find(nId);

	if (it != m_list.end())
	{
		return &(it->second);
	}

	return nullptr;
}

//===============================================
// �f�[�^�����݂��Ă��邩���f�[�^����m�F����
//===============================================
template <typename T>
T Cmaplist<T>::DataFind(T data)
{
	for (int i = 0; i < GetNum(); i++)
	{
		if (T == m_list[i])
		{
			return m_list[i].second;
		}
	}

	return nullptr;
}

//===============================================
// �f�[�^�����݂��Ă��邩��ID����m�F����
//===============================================
template <typename T>
bool Cmaplist<T>::IdFind(int nId)
{
	std::map<int, T>::iterator it = m_list.find(nId);

	if (it != m_list.end())
	{
		return true;
	}

	return false;
}


#endif // !_List_H_

