//===============================================
//
// �^�C�g����ʂ̊Ǘ����� [title.h]
// Author : Ibuki Okusada
//
//===============================================
//���ώ�
//Kazuki Watanabe
//<==============================================
#ifndef _TITLE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _TITLE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "manager.h"

//<**********************************************
//�O���錾
//<**********************************************
class CFileLoad;
class CObject2D;
class CPlayerTitle;
class CCamera;
class CPoliceTitle;
class CGole;
class CNumber;

//<**********************************************
//���O�錾(�R�R�ł����g��Ȃ��ϐ�)
//<**********************************************
namespace
{
	const int POLICE_MAX = 3;					//�x�@�̐�
}

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
		OBJ2D_BLACKCOVER,		//���J�o�[
		OBJ2D_TITLELOGO,		//�^�C�g�����S
		OBJ2D_PressEnter,		//�v���X�G���^�[
		OBJ2D_FRAME,			//�I����
		OBJ2D_NUMCHAR,			//"���l�I�����܂���"�̕���
		OBJ2D_CHECK,			//�m�F���b�Z�[�W
		OBJ2D_MAX

	};
	// �X�e�[�g
	enum STATE
	{
		STATE_TEAMLOGO = 0,	//�`�[�����S
		STATE_PRE,			//����
		STATE_PRESSENTER,	//�v���X�G���^�[
		STATE_CHASING,		//�x�@���v���C���[��ǐՒ�
		STATE_ICETHROW,		//�A�C�X�𓊂������V�[��
		STATE_MAX
	};

	//�V���O�����}���`���̑I����
	enum SELECT
	{
		SELECT_SINGLE = 0,		//�V���O��
		SELECT_MULTI,			//�}���`
		SELECT_MAX
	};

	//�I����
	enum SELECT_YN
	{
		SELECT_YN_YES = 0,
		SELECT_YN_NO,
		SELECT_YN_MAX
	};

	// �����o�֐�
	CTitle();	//�R���X�g���N�^
	~CTitle();	//�f�X�g���N�^

	//�����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	//�`�[�����S�̍ۂɎg���֐�
	void StateLogo(void);

	//�����̍ۂɎg���֐�
	void StatePre(void);

	//�v���X�G���^�[�X�e�[�g�̎��Ɏg���֐�
	void MoveP_E(void);
	void StateP_E(void);
	void InitingP_E(void);
	void ColChange(CObject2D *pObj2D);
	void MovingLogo(void);
	void BlackCoverM(void);
	void SkipMovement(void);
	void PreMove(void);

	//�ǐՏ�Ԃ̎��̓���
	void ChaseMovement(void);
	void ChaseCamera(void);

	//�I��
	void InitingSelect(void);
	void Sizing(void);
	void Selecting(void);
	void SelectSingleMulti(void);
	void SelectYesNO(void);
	void SelectCol(void);

	//�A�C�X�X�e�[�g
	void IceMovement(void);

	//�f�o�b�O�p
	void DebugCam(void);

	//<************************************
	//Get�n�֐�
	//<************************************
	//�V���O���ƃ}���`�ǂ������I������Ă��邩�̎擾�֐�
	int GetSelectSingleMulti(void) { return m_nSelect; }		


	STATE m_eState;								//�X�e�[�g
	//<************************************
	//�|�C���^�^
	//<************************************
	CPlayerTitle* m_pPlayer;					//�v���C���[�̃|�C���^
	CPoliceTitle* m_apPolice[POLICE_MAX];		//�x�@�̃|�C���^
	CGole* m_pGoal;								//�S�[���̃|�C���^

	CObject2D* m_pObject2D[OBJ2D_MAX];			//�I�u�W�F�N�g2D�̃|�C���^
	CObject2D* m_apSelect[SELECT::SELECT_MAX];	//�V���O���ƃ}���`�̑I�����̃|�C���^
	CObject2D* m_apYesNoObj[SELECT_YN_MAX];		//YES��NO�̑I�����̃|�C���^
	CCamera* m_pCam;

	//<************************************
	//int�^
	//<************************************
	int m_nCounterRanking;						//�����L���O�����J�ڃ^�C�}�[
	int m_nLogoAlpgha;							//�^�C�g�����S�̐F�ω��^�C�}�[
	int m_nCounter;								//�ėp�J�E���^�[
	int m_nSelect;								//�V���O���ƃ}���`�ǂ��炪�I������Ă��邩
	int m_nSelectYN;							//YES��NO�ǂ������I������Ă��邩

	float m_fDis;								//�����ϐ�

	//<************************************
	//bool�^
	//<************************************
	bool m_bPush;								//�`���[�g���A���J�ڂɕK�v�ȃ{�^����������Ă��邩
	bool m_bDisplay;							//��ʂɉf�����ǂ���
	bool m_bIniting;							//�I�u�W�F�N�g�̏��������I��������ǂ����̃`�F�b�N
	bool m_bCol;								//�F�ύX���Ă��邩�ǂ���		
	bool m_bNext;								//���ɍs���邩�̐���
	bool m_bSkipped;							//�X�L�b�v�������ǂ���
	bool m_bSizing;								//�T�C�Y�����������������ǂ���
	bool m_bSelected;							//�Z���N�g���ꂽ���ǂ���
};

#endif