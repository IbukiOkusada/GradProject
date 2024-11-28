//==========================================================
//
// �}�b�v�Ǘ��̏��� [map_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MAP_MANAGER_H_	// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MAP_MANAGER_H_	// ��d�C���N���[�h�h�~�p�}�N�����`

#include "list_manager.h"

// �O���錾
class CMap;

//==========================================================
// �^�X�N�}�l�[�W���[�̃N���X��`
//==========================================================
class CMapManager : public CListManager
{
private:
	CMapManager();	// �R���X�g���N�^
	~CMapManager();	// �f�X�g���N�^

public:
	// ���X�g�Ǘ������o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Load(void);

	// �V���O���g��
	static CMapManager* GetInstance(void);
	static void Release(void);
	std::vector<std::string>& GetFileNameList() { return m_LoadFileName; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void LoadRoad(const std::string& filename);
	void LoadObstacle(const std::string& filename);
	void LoadModelName(const std::string& filename);
	void LoadGimmick(const std::string& filename);
	void LoadGoal(const std::string& filename);

	// �����o�ϐ�
	std::vector<std::string> m_LoadFileName;
	static CMapManager* m_pInstance;
};

#endif
