//===============================================
//
// �}���`���U���g��ʂ̊Ǘ����� [multi_result.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _MULTI_RESULT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MULTI_RESULT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "manager.h"

// �O���錾
class CMultiResultManager;
class CScrollText2D;
class CPlayer;

//===============================================
// ���U���g�N���X�̒�`(�h���N���X)
//===============================================
class CMultiResult : public CScene
{
private:

	// �v���C���[���
	struct SPlayerInfo
	{
		bool bActive;			// �g�p����Ă��邩�ۂ�
		int nScore;				// �X�R�A
		int nId;				// ID
		CScrollText2D* pString;	// ����
		CPlayer* pPlayer;		// �v���C���[

		// �R���X�g���N�^
		SPlayerInfo(bool _active = false,int _score = 0, int _id = 0, CScrollText2D* _string = nullptr, CPlayer* _player = nullptr) :
			bActive(_active), nScore(_score), nId(_id), pString(_string), pPlayer(_player) {}
	};

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
	CMultiResult();	// �R���X�g���N�^
	~CMultiResult();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

	// �����o�֐�
	void Sort();
	void StrCheck();
	void EndStr();
	void InitCameraSet();
	void RankPlayerMove();

	// �����o�ϐ�
	CMultiResultManager* m_pMgr;	// �}�l�[�W���[
	CScrollText2D* m_pEndStr;		// �ŏI����
	CScrollText2D* m_pTitleStr;	// �ŏI����
	SPlayerInfo* m_pInfo;			// ���
	int m_nNowScrPlayer;

};

#endif
