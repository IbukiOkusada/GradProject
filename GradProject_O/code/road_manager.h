//==========================================================
//
// ���}�l�[�W���[ [road_manager.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _ROADMANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _ROADMANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

// �O���錾
class CRoad;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CRoadManager
{
private:

	CRoadManager();		// �R���X�g���N�^
	~CRoadManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CRoadManager* GetInstance(void);
	static void Release(void);
	CRoad* GetTop(void) { return m_pTop; }
	CRoad* GetCur(void) { return m_pCur; }
	void ListIn(CRoad* pRoad);
	void ListOut(CRoad* pRoad);
	int GetNum(void) { return m_nNum; }
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

	// ���A���֐�
	void AllConnect(void);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void SideConnectCheck(CRoad* pRoad, CRoad* pCheckRoad);		// ���A������
	void VerticalConnectCheck(CRoad* pRoad, CRoad* pCheckRoad);	// �c�A������

	// �����o�ϐ�
	CRoad* m_pTop;	// �擪
	CRoad* m_pCur;	// �Ō��
	int m_nNum;
	static CRoadManager* m_pInstance;	// �C���X�^���X
};

#endif

