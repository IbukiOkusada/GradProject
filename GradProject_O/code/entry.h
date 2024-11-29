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

private:

	// �����o�֐�
	void AddPlayer(void);  // �v���C���[�Q������
	void DecreasePlayer(void);  // �v���C���[�Q������������

	// �����o�ϐ�
	CPlayer** m_ppPlayer;	// �v���C���[�̃|�C���^
	CMultiCamera** m_ppCamera;   // �J�����̃|�C���^

	bool m_IsFinish;  // �`���[�g���A�����I�����Ă��邩�ǂ���
};

#endif