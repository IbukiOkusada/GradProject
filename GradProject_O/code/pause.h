//==========================================================
//
// �|�[�Y [pause.h]
// Author : Riku Nakamura
//
//==========================================================
#ifndef _SAMPLETASK_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _SAMPLETASK_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "object2D.h"

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CPause : public CTask
{

public:	// �N�ł��A�N�Z�X�\

	// AI�^�C�v�񋓌^
	enum TYPE
	{
		TYPE_QUIT = 0,	// �I��
		TYPE_RETRY,			// ��蒼��
		TYPE_CANCEL,			// �ĊJ
		TYPE_MAX
	};

	CPause();	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CPause();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	static CPause* Create(void);

	bool GetPause(void) { return m_bPause; }

	void ChangePause(void);

	// �����o�֐�(�擾)

	// �����o�֐�(�ݒ�)

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	
	// �����o�ϐ�
	CObject2D* m_pPauseFrame;
	CObject2D* m_pPauseButton[TYPE_MAX];

	int m_nNumSelect;
	float m_fTimerColor;
	bool m_bPause;
};

#endif
