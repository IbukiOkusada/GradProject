//===============================================
//
// ���U���g��ʂ̊Ǘ����� [result.h]
// Author : Kenta Hashimoto
//
//===============================================
#ifndef _RESULT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _RESULT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "manager.h"

class CScore;
class CFileLoad;
class CMeshDome;
class CTime;
class CCharacter;
class CObject2D;
class CNumber;
class CObjectX;
class CPlayerResult;

#define MAX_RANK	(4)		// �����L���O�̏��ʐ�

//===============================================
// ���U���g�N���X�̒�`(�h���N���X)
//===============================================
class CResult : public CScene
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
	CResult();	// �R���X�g���N�^
	~CResult();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void SetScore(const int nScore) { m_nScore = static_cast<float>(nScore); }

private:

	enum TYPE_OBJ
	{
		TYPE_TIME = 0,
		TYPE_LIFE,
		TYPE_EVAL,
		TYPE_RANKING,
		TYPE_NUM
	};
	void ScoreObjCreat();	// ����̃X�R�A�̃I�u�W�F�N�g�𐶐�
	void RankObjCreat();	// �����L���O�̃I�u�W�F�N�g�𐶐�

	void Calculation(int* Obj, float Score, int Cnt, int ObjMax, TYPE_OBJ Type);
	void AlphaJudge(float Alpha);		// �A���t�@�l�̔���
	void RankAlphaJudge(float Alpha);	// �A���t�@�l�̔���
	void Display();						// �\������
	void DisplayRanking();				// �����L���O�̕\������
	void AllDisplay();					// �S�ĕ\������
	void RoadScore();					// �����L���O�̃X�R�A�����[�h
	void SaveScore();					// �����L���O�̃X�R�A���Z�[�u
	void SortScore();					// �����L���O�̃X�R�A���\�[�g


	// �����o�ϐ�
	CFileLoad *m_pFileLoad;			// �t�@�C���ǂݍ��݂̃|�C���^
	CMeshDome *m_pMeshSky;			// ��p
	CTime *m_pTime;					// �^�C�}�[
	CObject2D *m_pObjClear;			// clear�������ǂ���
	CPlayerResult* m_pPlayer;		// ���U���g�v���C���[�̃|�C���^

	CObject2D* m_pObj;					// ���sor�����̃I�u�W�F�N�g
	CObject2D* m_pRankingObj;			// �����L���O�̃I�u�W�F�N�g
	CObject2D* m_pScoreObj[4];			// �e�X�R�A�̕����̃I�u�W�F�N�g
	CObject2D* m_pEvalStarObj[5];		// �����X�R�A�̐��̃I�u�W�F�N�g
	CObject2D* m_pEvalStarFreamObj[5];	// �����X�R�A�̐��̘g�̃I�u�W�F�N�g
	CObject2D* m_pStarObj[25];			// ���̃I�u�W�F�N�g
	CObject2D* m_pStarFreamObj[25];		// ���̘g�̃I�u�W�F�N�g
	CObject2D* m_pDecPointEval;			// �����X�R�A�̏����_�̃I�u�W�F�N�g
	CObject2D* m_pDecPointRank[5];		// �����L���O�̏����_�̃I�u�W�F�N�g
	CNumber* m_pDeliNumber;				// �͂������̃I�u�W�F�N�g
	CNumber* m_pTimeNumber[3];			// �c��^�C���̃I�u�W�F�N�g
	CNumber* m_pLifeNumber[3];			// �c��̗͂̃I�u�W�F�N�g
	CNumber* m_pEvalNumber[2];			// �����X�R�A�̃I�u�W�F�N�g
	CNumber* m_pRankingNumber[10];		// �����L���O�̃I�u�W�F�N�g

	static int m_nDeli;			// �z�B������
	static float m_fTime;		// �c�莞��
	static float m_fLife;		// �c��̗�
	static float m_nScore;		// �����X�R�A
	static bool m_bAuto;
	float m_RankingScore[5];	// �����L���O�X�R�A

	int m_TimeObj[3];			// �^�C���̃I�u�W�F�N�g�ɕK�v�ȕϐ�
	int m_LifeObj[3];			// �̗͂̃I�u�W�F�N�g�ɕK�v�ȕϐ�
	int m_EvalObj[2];			// �����X�R�A�̃I�u�W�F�N�g�ɕK�v�ȕϐ�
	int m_RankingObj[10];		// �����L���O�X�R�A�̃I�u�W�F�N�g�ɕK�v�ȕϐ�
	int m_Display;				// �\��������I�u�W�F�N�g�̕ϐ�
	int m_DisplayRank;			// �����L���O�̕\��������I�u�W�F�N�g�̕ϐ�
	int m_Timehid;
	int m_Lifehid;
	bool m_bSuccess;			// �N���A�������ǂ���
	bool m_bAllDisp;			// ���ׂĕ\���������ǂ���
};

#endif