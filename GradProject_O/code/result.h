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
	static void SetScore(const int nScore) { m_nScore = nScore; }

private:

	enum TYPE_OBJ
	{
		TYPE_TIME = 0,
		TYPE_LIFE,
		TYPE_EVAL,
		TYPE_RANKING,
		TYPE_NUM
	};
	void ScoreObjInit();

	void Calculation(int* Obj, float Score, int Cnt, int ObjMax, TYPE_OBJ Type);
	void AlphaJudge(float Alpha);		// �A���t�@�l�̔���
	void RankAlphaJudge(float Alpha);	// �A���t�@�l�̔���
	void Display();						// �\������
	void DisplayRanking();				// �����L���O�̕\������
	void RoadScore();
	void SaveScore();
	void SortScore();


	// �����o�ϐ�
	CFileLoad *m_pFileLoad;			// �t�@�C���ǂݍ��݂̃|�C���^
	CMeshDome *m_pMeshSky;			// ��p
	CTime *m_pTime;					// �^�C�}�[
	CObject2D *m_pObjClear;			// clear�������ǂ���
	CPlayerResult* m_pPlayer;		// ���U���g�v���C���[�̃|�C���^

	CObject2D* m_pObj;				// ���sor�����̃I�u�W�F�N�g
	CObject2D* m_pScoreObj[4];		// �e�X�R�A�̕����̃I�u�W�F�N�g
	CObject2D* m_pStarObj[25];		// ���̃I�u�W�F�N�g
	CObject2D* m_pStarFreamObj[25];	// ���̘g�̃I�u�W�F�N�g
	CNumber* m_pDeliNumber;			// �͂������̃I�u�W�F�N�g
	CNumber* m_pTimeNumber[3];		// �c��^�C���̃I�u�W�F�N�g
	CNumber* m_pLifeNumber[3];		// �c��̗͂̃I�u�W�F�N�g
	CNumber* m_pEvalNumber[2];		// �����]���̃I�u�W�F�N�g
	CNumber* m_pRankingNumber[10];	// �����L���O�̃I�u�W�F�N�g

	static int m_nDeli;
	static float m_fTime;
	static float m_fLife;
	static float m_nScore;
	float m_RankingScore[5];

	int m_TimeObj[3];
	int m_LifeObj[3];
	int m_EvalObj[2];
	int m_RankingObj[10];
	int m_Display;
	int m_DisplayRank;

};

#endif