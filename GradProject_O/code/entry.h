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

namespace
{
	const int MAX = 4;
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

private:

	// �����o�֐�
	void AddPlayer(void);       // �v���C���[�Q������
	void DecreasePlayer(void);  // �v���C���[�Q������������
	void ControlsUI(void);      // ������@��UI
	void ReadyUp(void);         // ����OK����

	// �����o�ϐ�
	CMultiCamera** m_ppCamera;   // �J�����̃|�C���^
	CObjectX** m_ppObjX;
	CObject2D* m_pControlsUI[MAX];
	CObject2D* m_pReady[MAX];
	int m_nID;

	bool m_IsFinish;   // �`���[�g���A�����I�����Ă��邩�ǂ���
	bool m_bSetReady;  // �������������Ă��邩�ǂ���
};

#endif