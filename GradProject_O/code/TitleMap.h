//<=================================================
//�^�C�g���ł̃}�b�v����(TitleMap.h)
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _TITLEMAP_H_
#define _TITLEMAP_H_

#include "list_manager.h"
#include "task.h"
#include "list.h"
#include "map_obstacle.h"

//<=================================================
//�^�C�g���}�l�[�W���[�p��
//<=================================================
class CTitleMap : public CListManager
{
public:

	CTitleMap() {}
	~CTitleMap() {}

	// ���X�g�Ǘ������o�֐�
	HRESULT Init(void) { return S_OK; }
	void Uninit(void);
	void Load(void);

	//�C���X�^���X�擾
	static CTitleMap* GetInstance(void) 
	{
		if (!m_pInstance)
		{
			m_pInstance = DEBUG_NEW CTitleMap;
		}

		return m_pInstance;
	}
	//�j��
	static void Release(void) 
	{
		if (m_pInstance)
		{
			m_pInstance->Uninit();
		}
	}
	std::vector<std::string>& GetFileNameList() { return m_LoadFileName; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void LoadRoad(const std::string& filename);
	void LoadObstacle(const std::string& filename);
	void LoadModelName(const std::string& filename);

	// �����o�ϐ�
	std::vector<std::string> m_LoadFileName;
	static CTitleMap* m_pInstance;
};
//<=================================================
//�^�C�g���p�̏�Q��(���̏�Q���N���X���p��)
//<=================================================
class CTitleObstacle : public CMapObstacle
{
public:

	CTitleObstacle(const SInfo& info = SInfo());	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CTitleObstacle();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;

	static CTitleObstacle* Create(const SInfo& info);

private:

	//�����o�ϐ�
	CObjectX* m_pObj;	// ��Q��
	SInfo m_Info;		// ��{���
	static Clist<CTitleObstacle*>* m_pList;	// ���X�g

};
#endif
