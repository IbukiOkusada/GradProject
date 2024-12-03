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

//==========================================================
// �S�[���Ǘ��̃N���X��`
//==========================================================
class CInspectionManager
{
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

	static void Release();

	// �擾�n
	static CInspectionManager* GetInstance(void);

private:

	// �����o�֐�
	void Uninit(void);   // �I������

	// �����o�ϐ�
	Cmaplist<CInspection*> m_List;
	static CInspectionManager* m_pInstance;  // ���g�̃|�C���^
};

#endif
