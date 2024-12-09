//==========================================================
//
// ����Ǘ��̏��� [inspection_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _INSPECTION_MANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _INSPECTION_MANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "map_list.h"

// �O���錾
class CInspection;
class CRoad;

//==========================================================
// �S�[���Ǘ��̃N���X��`
//==========================================================
class CInspectionManager
{
private:

	struct SNextInfo
	{
		bool bActive;
		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
		CRoad* pRoad;
		int nId;
		int nStartpdid;

		// �R���X�g���N�^
		SNextInfo() : bActive(false), pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), pRoad(nullptr), nId(-1), nStartpdid(-1) {}
	};

public:
	CInspectionManager();      // �R���X�g���N�^
	~CInspectionManager();     // �f�X�g���N�^

	HRESULT Init(void);  // ����������
	void Update(void);   // �X�V����

	// ���X�g�Ǘ�
	bool ListIn(CInspection* pInspection);
	bool ListOut(CInspection* pInspection);
	CInspection* Get(int nIdx);
	int GetCreateCnt() { return m_List.GetInCnt(); }
	void SetNextInspection(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, CRoad* pRoad, int nId, int startpdid);

	static void Release();

	// �擾�n
	static CInspectionManager* GetInstance(void);

private:

	// �����o�֐�
	void Uninit(void);   // �I������

	// �����o�ϐ�
	SNextInfo m_NextInfo;
	Cmaplist<CInspection*> m_List;
	static CInspectionManager* m_pInstance;  // ���g�̃|�C���^
};

#endif
