//<=================================================
//�^�C�g���ł̃}�b�v����
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _TITLEMAP_H_
#define _TITLEMAP_H_

#include "list_manager.h"
#include "task.h"

//<=================================================
//�^�C�g���}�l�[�W���[�p��
//<=================================================
class CTitleMap : public CListManager
{
public:

	CTitleMap();
	~CTitleMap();

	// ���X�g�Ǘ������o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Load(void);

	// �V���O���g��
	static CTitleMap* GetInstance(void) 
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = DEBUG_NEW CTitleMap;
		}
		return m_pInstance;
	}
	static void Release(void) 
	{
		if (m_pInstance != nullptr)
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
#endif
