//<=================================================
//�^�C�g���ł̉ו��֘A����
//
//Author : Kazuki Watanabe
//<=================================================
#ifndef _TITLEBAGGAGE_H_
#define _TITLEBAGGAGE_H_

#include "baggage.h"
#include "task.h"

class CCharacter;
//<===============================================
//�ו��N���X���p�����Ē�`
//<===============================================
class CTitleBaggage : public CBaggage
{
public:

	CTitleBaggage() {}	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CTitleBaggage() {}	// �f�X�g���N�^

	//�����o�֐�
	HRESULT Init(void);
	void Uninit(void) { CBaggage::Uninit(); }
	void Update(void) { CBaggage::Update(); }
	static CTitleBaggage* Create(const D3DXVECTOR3& pos);

private:

};
//<===============================================
//�S�[���N���X���p�����Ē�`
//<===============================================
class CTitleGoal : public CTask
{
public:

	CTitleGoal() {}	// �f�X�g���N�^
	~CTitleGoal() {}	// �f�X�g���N�^

	//�����o�֐�
	HRESULT Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	HRESULT Init(void) { return S_OK; }
	void Uninit(void);
	void Update(void);
	static CTitleGoal* Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);

	//�擾
	CCharacter* GetCHR(void) {return m_pPeople;}
	D3DXVECTOR3 GetPos(void) {return m_rPos;}

private:

	CCharacter* m_pPeople;
	D3DXVECTOR3 m_rPos;

};

#endif
