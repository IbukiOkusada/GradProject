//===============================================
//
// �G���g���[��ʂ̊Ǘ����� [entry.h]
// Author : Ryosuke Oohara
//
//===============================================
#ifndef _ENTRY_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _ENTRY_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "manager.h"

class CPlayer;
class CMultiCamera;
class CObjectX;
class CObject2D;
class CGoalManager;
class CScrollText2D;

namespace
{
	const int MAX_PLAYER = 4;      // �v���C���[�̍ő吔
	const int NUM_CONTROL_UI = 4;  // ����UI�̑���
}

//===============================================
// �G���g���[�N���X�̒�`(�h���N���X)
//===============================================
class CEntry : public CScene
{
public:

	// ��ޗ񋓌^
	enum TYPE
	{
		TYPE_MULTI_WIN,		// �}���`����
		TYPE_MULTI_LOSE,	// �}���`�s�k
		TYPE_MAX			// �ʏ�
	};

	// ����UI���
	enum CONTROL_UI_TYPE
	{
		UI_TURN,   // ����
		UI_ACCEL,  // �A�N�Z��
		UI_BRAKE,  // �u���[�L
		UI_BOOST,  // �u�[�X�g
		UI_MAX
	};

public:

	// �����o�֐�
	CEntry();	// �R���X�g���N�^
	~CEntry();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetID(const int id) override;
	void ChangeFlag(bool value) override;
	void EndTutorial(void) override;
	void ReadyUp(const int id) override;

private:

	// �����o�֐�
	void AddPlayer(void);       // �v���C���[�Q������
	void DecreasePlayer(void);  // �v���C���[�Q������������
	void ControlsUI(void);      // ������@��UI
	void ReadyUp(void);         // ����OK����

	// �����o�ϐ�
	CMultiCamera** m_ppCamera;                 // �J�����̃|�C���^
	CObjectX** m_ppObjX;                       // ��ʉ��ɂł�v���C���[�̃|�C���^
	CObject2D* m_pControlsUI[NUM_CONTROL_UI];  // ����UI�̃|�C���^
	CObject2D* m_pReady[MAX_PLAYER];           // ����UI�̃|�C���^
	CObject2D* m_pPPP = nullptr;
	CGoalManager* m_pGoalManager;              // �S�[���}�l�[�W���[�̃|�C���^
	CScrollText2D* m_pString;	               // ����

	int m_nID;
	bool m_IsFinish;   // �`���[�g���A�����I�����Ă��邩�ǂ���
	bool m_bSetReady;  // �������������Ă��邩�ǂ���
};

// �`���[�g���A���X�e�b�v�N���X
class CTutorialStep
{
public:
	CTutorialStep();
	~CTutorialStep();

	virtual void Update(CEntry* pEntry) = 0;

private:

};

#endif