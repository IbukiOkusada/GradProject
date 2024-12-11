//===============================================
//
// �}���`���U���g��ʂ̏�ԊǗ����� [multi_result_state.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _MULTI_RESULT_STATE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MULTI_RESULT_STATE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

// �O���錾
class CMultiResult;
class CPolice;

//===============================================
// �}���`���U���g�̏�ԃN���X��`(���N���X)
//===============================================
class CMultiResultState
{
public:

	// ��ԗ�
	enum STATE
	{
		STATE_START = 0,	// �J�n
		STATE_RANK,			// �����N�\��
		STATE_MULTIRANK,		// ���ʕ\��
		STATE_END,
		STATE_MAX
	};

public:
	
	// �R���X�g���N�^
	CMultiResultState(STATE _state) : m_bCreate(false), m_state(_state) {}

	// �f�X�g���N�^
	virtual ~CMultiResultState(){}

	// �����o�֐�
	void Update(CMultiResult* pResult) 
	{
		if (pResult == nullptr) { return; }

		// ����
		if (!m_bCreate) {
			Create(pResult);
			m_bCreate = true;
		}

		// ����
		Controll(pResult);
	}

protected:
	
	//===============================================
	// �����o�֐�
	//===============================================
	// �K�v����
	virtual void Controll(CMultiResult* pResult) = 0;
	// ����
	virtual void Create(CMultiResult* pResult){}

private:

	// �����o�ϐ�
	bool m_bCreate;	// ��x�����������ۂ�
	STATE m_state;	// ���
};

//*************************************************************************************
// �J�n��ԃN���X(�h���N���X)
//===============================================
class CMultiResultStateStart : public CMultiResultState
{
public:

	// �R���X�g���N�^
	CMultiResultStateStart();

	// �f�X�g���N�^
	~CMultiResultStateStart() override;

private:

	// �����o�֐�
	void Controll(CMultiResult* pResult) override;
};

//===============================================
// �����N�\����ԃN���X(�h���N���X)
//===============================================
class CMultiResultStateRank : public CMultiResultState
{
public:

	// �R���X�g���N�^
	CMultiResultStateRank();

	// �f�X�g���N�^
	~CMultiResultStateRank() override;

private:

	// �����o�֐�
	void Controll(CMultiResult* pResult) override;
	void RankPlayerMove(CMultiResult* pResult);
};

//===============================================
// ���ʕ\����ԃN���X(�h���N���X)
//===============================================
class CMultiResultStateResult : public CMultiResultState
{
public:

	// �R���X�g���N�^
	CMultiResultStateResult();

	// �f�X�g���N�^
	~CMultiResultStateResult() override;

private:

	// �����o�֐�
	void Controll(CMultiResult* pResult) override;
	void Create(CMultiResult* pResult) override;
	void Uninit(CMultiResult* pResult);
	void PoliceUpdate(CMultiResult* pResult);

	// �����o�ϐ�
	CPolice** m_ppPolice;	// �x�@�̐�����
	int m_nNumCreatePolice;	// �x�@�̐�����
};

//===============================================
// �I���\��ԃN���X(�h���N���X)
//===============================================
class CMultiResultStateEnd : public CMultiResultState
{
public:

	// �R���X�g���N�^
	CMultiResultStateEnd();

	// �f�X�g���N�^
	~CMultiResultStateEnd() override;

private:

	// �����o�֐�
	void Controll(CMultiResult* pResult) override;

	// �����o�ϐ�
	float m_fFadeCnt;	// �t�F�[�h�܂ł̃J�E���g
};

#endif
