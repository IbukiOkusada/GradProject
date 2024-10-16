//==========================================================
//
// �ԃ}�l�[�W���[ [car_manager.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _CARMANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _CARMANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

// �O���錾
class CCar;

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CCarManager
{
private:

	CCarManager();		// �R���X�g���N�^
	~CCarManager();	// �f�X�g���N�^

public:	// �N�ł��A�N�Z�X�\

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CCarManager* GetInstance(void);
	static void Release(void);
	CCar* GetTop(void) { return m_pTop; }
	CCar* GetCur(void) { return m_pCur; }
	void ListIn(CCar* pCar);
	void ListOut(CCar* pCar);
	int GetNum(void) { return m_nNum; }
	bool Hit(D3DXVECTOR3& pos, const float fRange, const float fHeight, const int nDamage);

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	// 
	// �����o�ϐ�
	CCar* m_pTop;	// �擪
	CCar* m_pCur;	// �Ō��
	int m_nNum;
	static CCarManager* m_pInstance;	// �C���X�^���X
};

#endif
