//===============================================
//
// �^�C�g����ʂ̊Ǘ����� [title.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _TITLE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _TITLE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "manager.h"

class CFileLoad;
class CTitleEnter;
class CPlayer;
class CObject2D;

//===============================================
// �^�C�g���N���X�̒�`(�h���N���X)
//===============================================
class CTitle : public CScene
{
public:

	// �I�u�W�F�N�g2D�̗񋓌^
	enum OBJ2D
	{
		OBJ2D_TeamLogo = 0,		//�`�[�����S
		OBJ2D_PressEnter,		//�v���X�G���^�[
		OBJ2D_MAX

	};
	// �X�e�[�g
	enum STATE
	{
		STATE_TEAMLOGO = 0,	//�`�[�����S
		STATE_PRESSENTER,	//�v���X�G���^�[
		STATE_CHASING,		//�x�@���v���C���[��ǐՒ�
		STATE_ICETHROW,		//�A�C�X�𓊂������V�[��
		STATE_MAX
	};


	// �����o�֐�
	CTitle();	// �R���X�g���N�^
	~CTitle();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	void StateLogo(void);
	void StateP_E(void);
	void InitingP_E(void);
	void ColChange(void);


	CFileLoad *m_pFileLoad;		// �t�@�C���ǂݍ��݂̃|�C���^
	CTitleEnter *m_pEnter;
	CPlayer* m_pPlayer;						// �v���C���[�̃|�C���^
	CObject2D* m_pObject2D[OBJ2D_MAX];		// �`�[�����S�̃|�C���^
	STATE m_eState;							// �X�e�[�g
	int m_nCounterTutorial;					// �`���[�g���A���y�ѐl���I����ʑJ�ڃ^�C�}�[
	int m_nCounterRanking;					// �����L���O�����J�ڃ^�C�}�[

	int m_nCounter;							//�ėp�J�E���^�[

	bool m_bPush;							// �`���[�g���A���J�ڂɕK�v�ȃ{�^����������Ă��邩
	bool m_bDisplay;						// ��ʂɉf�����ǂ���
	bool m_bIniting;						// �I�u�W�F�N�g�̏��������I��������ǂ����̃`�F�b�N
	bool m_bCol;
};

#endif