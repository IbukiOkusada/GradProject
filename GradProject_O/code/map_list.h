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

	// �w�b�_�[�Ŏg����悤�ɖ��O��t����! Iteretor���Ė��O��iteretor���g���܂���
	using Iteretor = typename std::map<int, T>::iterator;
	
	Cmaplist();
	~Cmaplist();

	// �����o�֐�
	bool Regist(int nId, T data);	// ���X�g�ɘA��
	bool Delete(int nId, T data);	// ���X�g�ɘA��
	bool Delete(int nId);			// ID����폜
	bool Delete(T data);			// �f�[�^����폜
	bool Empty();					// ���X�g���󂩂��m�F
	void Clear();					// ���X�g����ɂ���

	// �擾�n�֐�
	std::map<int, T>& GetList();		// ���X�g���擾
	int GetNum();					// ���X�g�̃T�C�Y�擾
	T Get(int nId);					// ID����{�̂��擾
	int Get(T data);				// �f�[�^����ID���擾
	Cmaplist<T>::Iteretor DataFind(T data);			// �f�[�^���猟��
	bool IdFind(int nId);			// ID���猟��
	Cmaplist<T>::Iteretor GetBegin();	// �擪�擾
	Cmaplist<T>::Iteretor GetEnd();		// �I�[�擾

private:

	// �����o�ϐ�
	std::map<int, T> m_list;	// ���X�g�{��
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
// �擪���擾
//===============================================
template <typename T>
typename Cmaplist<T>::Iteretor Cmaplist<T>::GetBegin()
{
	return m_list.begin();
}

//===============================================
// �I�[���擾
//===============================================
template <typename T>
typename Cmaplist<T>::Iteretor Cmaplist<T>::GetEnd()
{
	return m_list.end();
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
	// ��������
	if (IdFind(nId))
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
bool Cmaplist<T>::Delete(T data)
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
std::map<int, T>& Cmaplist<T>::GetList()
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
// ID����f�[�^���擾
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
// �f�[�^�����݂��Ă��邩���f�[�^����m�F����
//===============================================
template <typename T>
typename Cmaplist<T>::Iteretor Cmaplist<T>::DataFind(T data)
{
	return find_if(
		m_list.begin(),
		m_list.end(),
		[&data](const std::pair<int, T>& pair) {// �l����v���邩�𔻒�
			return pair.second == data;}
	);
}

//===============================================
// �f�[�^�����݂��Ă��邩��ID����m�F����
//===============================================
template <typename T>
bool Cmaplist<T>::IdFind(int nId)
{
	Iteretor it = m_list.find(nId);

	// ���݂��Ă���
	if (it != m_list.end())
	{
		return true;
	}

	return false;
}

#endif // !_List_H_

