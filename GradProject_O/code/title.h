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
#include "objectsound.h"

//<**********************************************
//�O���錾
//<**********************************************
class CCamera;
class CObject2D;
class CPlayerTitle;
class CPoliceTitle;
//<**********************************************
//���O�錾(�R�R�ł����g��Ȃ��ϐ�)
//<**********************************************
namespace
{
	const int POLICE_MAX = 3;					//�x�@�̐�


	//���ʉ���(�R�R�ł����g��Ȃ��̂�static�ɂ��܂���)
	const enum TITLE_SE : unsigned int
	{
		TITLE_SE_SELECT = 0,	//�I��
		TITLE_SE_CANCEL,		//�L�����Z��
		TITLE_SE_DECIDE0,		//����1
		TITLE_SE_DECIDE1,		//����2
		TITLE_SE_MAX
	};

	//�ړI�n�i���o�[
	const enum DEST : unsigned int
	{
		DEST_FIRST = 0,	//��Ԗ�
		DEST_SECOND,	//��Ԗ�
		DEST_THIRD,		//�O�Ԗ�
		DEST_FOUTH,		//�l�Ԗ�
		DEST_FIFTH,		//�ܔԖ�
		DEST_SIXTH,		//�Z�Ԗ�
		DEST_MAX,

	};

	//�ړI�n�̈ʒu
	const D3DXVECTOR3 DEST_POS_SELECT[DEST::DEST_MAX] =
	{
		D3DXVECTOR3(3000.0f, 0.0f, 6600.0f),		//�P��
		D3DXVECTOR3(-3160.0f, 0.0f, 6000.0f),		//�Q��
		D3DXVECTOR3(-3160.0f, 0.0f, 3300.0f),		//�R��
		D3DXVECTOR3(-5050.0f, 0.0f, 4000.0f),		//�S��
		D3DXVECTOR3(-4750.0f, 0.0f, -200.0f),		//�T��
		D3DXVECTOR3(3000.0f, 0.0f, -200.0f)			//�U��
	};

	//�ړI�̌���
	const D3DXVECTOR3 DEST_ROT_SELECT[DEST::DEST_MAX] =
	{
		D3DXVECTOR3(0.0f,-3.14f,0.0f),				//�P��
		D3DXVECTOR3(0.0f,1.57f,0.0f),				//�Q��
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//�R��
		D3DXVECTOR3(0.0f,1.57f,0.0f),				//�S��
		D3DXVECTOR3(0.0f,0.0f,0.0f),				//�T��
		D3DXVECTOR3(0.0f,-1.57f,0.0f)				//�U��
	};

	//�^�C�g���Ɏg��SE�̃t�@�C����(�R�R�ł����g��Ȃ��̂�static�ɂ��܂���)
	static const std::string TitleSEName[TITLE_SE_MAX] =
	{
		"data/SE/TITLE/Title-Select.wav",		//�I��
		"data/SE/TITLE/Title-Cancel.wav",		//�L�����Z��
		"data/SE/TITLE/Title-Decide0.wav",		//���艹1
		"data/SE/TITLE/Title-Decide1.wav",		//���艹2
	};
}
//===============================================
// �^�C�g���N���X�̒�`(�h���N���X)
//===============================================
class CTitle : public CScene
{
public:

	// �����o�֐�
	CTitle();	//�R���X�g���N�^
	~CTitle();	//�f�X�g���N�^

	//�����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void) { CScene::Draw(); }

	//<************************************
	//Get�n�֐�
	//<************************************
	//�V���O���ƃ}���`�ǂ������I������Ă��邩�̎擾�֐�
	inline int GetSelectSingleMulti(void) { return m_nSelect; }

	//�v���C���[�擾
	inline static CPlayerTitle* GetPlayer(void)
	{
		//���g�Ȃ����nullptr��Ԃ�
		if (!m_pPlayer) { return nullptr; }

		//���g����΂��̌x�@����Ԃ�
		else { return m_pPlayer; }
	}
private:

	// �I�u�W�F�N�g2D�̗񋓌^
	const enum OBJ2D : unsigned int
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
	const enum STATE : unsigned int
	{
		STATE_TEAMLOGO = 0,	//�`�[�����S
		STATE_PRE,			//����
		STATE_PRESSENTER,	//�v���X�G���^�[
		STATE_CHASING,		//�x�@���v���C���[��ǐՒ�
		STATE_ICETHROW,		//�A�C�X�𓊂������V�[��
		STATE_MAX
	};

	//�V���O�����}���`���̑I����
	const enum SELECT : unsigned int
	{
		SELECT_SINGLE = 0,		//�V���O��
		SELECT_MULTI,			//�}���`
		SELECT_MAX
	};

	//�I����
	const enum SELECT_YN : unsigned int
	{
		SELECT_YN_YES = 0,
		SELECT_YN_NO,
		SELECT_YN_MAX
	};

	//�T�C�Y�Ɋ֌W����񋓌^
	const enum SIZING : unsigned int
	{
		SIZING_WIDTH = 0,	//��
		SIZING_HEIGHT,		//�c
		SIZING_MAX
	};

	//�`�[�����S�̍ۂɎg���֐�
	void StateLogo(void);

	//�����̍ۂɎg���֐�
	void StatePre(void);

	//�v���X�G���^�[�X�e�[�g�̎��Ɏg���֐�
	void MoveP_E(void);
	void StateP_E(void);
	void InitingP_E(void);
	void ColChange(CObject2D *pObj2D,const int nCntMax);
	void MovingLogo(void);
	void BlackCoverM(void);
	void SkipMovement(void);
	void PreMove(void);
	void LightOff(void);

	//�ǐՏ�Ԃ̎��̓���
	void ChaseMovement(void);
	void Chasing(void);
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
	void InitingIce(void);

	//�f�o�b�O�p
	void DebugCam(void);

	bool TriggerEnter(void);

	void SelectNum(int *nNum,const int nMax);

	//���ʉ��Đ�
	inline void SEPlay(const TITLE_SE eTitleSE)
	{
		m_apMSound[eTitleSE]->Play();
	}

	//�I�u�W�F�N�g2D�̏�����
	CObject2D* InitObj2D(const D3DXVECTOR3 rPos,		
		const D3DXVECTOR3 rRot,								
		const int nPri,										
		const float fWidth,									
		const float fHeight,								
		const bool bDraw,									
		const char* pTexName,								
		const D3DXCOLOR rCol = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));

	STATE m_eState;												//�X�e�[�g
	//<************************************
	//�|�C���^�^
	//<************************************
	static CPlayerTitle* m_pPlayer;								//�v���C���[�̃|�C���^
	CPoliceTitle* m_apPolice[POLICE_MAX];						//�x�@�̃|�C���^

	CObject2D* m_pObject2D[OBJ2D::OBJ2D_MAX];					//�I�u�W�F�N�g2D�̃|�C���^
	CObject2D* m_apSelect[SELECT::SELECT_MAX];					//�V���O���ƃ}���`�̑I�����̃|�C���^
	CObject2D* m_apYesNoObj[SELECT_YN::SELECT_YN_MAX];			//YES��NO�̑I�����̃|�C���^
	CCamera* m_pCam;
	CMasterSound::CObjectSound* m_apMSound[TITLE_SE_MAX];

	//<************************************
	//int�^
	//<************************************
	int m_nCounterRanking;						//�����L���O�����J�ڃ^�C�}�[
	int m_nLogoAlpgha;							//�^�C�g�����S�̐F�ω��^�C�}�[
	int m_nLogoCou;								//���C�g�I�t����܂ł̃^�C�}�[
	int m_nLogoStart;
	int m_nBlinkCount;
	int m_nCounter;								//�ėp�J�E���^�[
	int m_nSelect;								//�V���O���ƃ}���`�ǂ��炪�I������Ă��邩
	int m_nSelectYN;							//YES��NO�ǂ������I������Ă��邩

	float m_fDis;								//�����ϐ�

	//<************************************
	//bool�^
	//<************************************
	bool m_bPush;								//�`���[�g���A���J�ڂɕK�v�ȃ{�^����������Ă��邩
	bool m_bIniting;							//�I�u�W�F�N�g�̏��������I��������ǂ����̃`�F�b�N
	bool m_bCol;								//�F�ύX���Ă��邩�ǂ���		
	bool m_bNext;								//���ɍs���邩�̐���
	bool m_bSkipped;							//�X�L�b�v�������ǂ���
	bool m_bSizing;								//�T�C�Y�����������������ǂ���
	bool m_bSelected;							//�Z���N�g���ꂽ���ǂ���
};

#endif