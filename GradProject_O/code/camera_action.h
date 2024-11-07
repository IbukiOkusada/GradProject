//==========================================================
//
// �J�����A�N�V�����w�b�_�[ [camera_action.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _CAMERAACTION_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _CAMERAACTION_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

// �O���錾
class CCamera;

//==========================================================
// �J�����A�N�V�����̃N���X��`
//==========================================================
class CCameraAction
{
public:

	// �ړ��񋓌^
	enum MOVE
	{
		MOVE_POSV = 0,
		MOVE_POSR,
		MOVE_MAX
	};

private:

	// �J�n�n�_�\����
	struct SStartInfo
	{
		D3DXVECTOR3 posV;	// ���_
		D3DXVECTOR3 posR;	// �����_
		D3DXVECTOR3 rot;	// ����
		float fLength;	// ����

		SStartInfo() : posV(VECTOR3_ZERO), posR(VECTOR3_ZERO), rot(VECTOR3_ZERO), fLength(0.0f) {}
	};

	// �ڕW�n�_�\����
	struct STargetInfo
	{
		D3DXVECTOR3 rot;	// ����
		float fLength;		// ����

		STargetInfo() : rot(VECTOR3_ZERO), fLength(0.0f) {}
	};

	// ���ԊǗ��\����
	struct STime
	{
		float fNow;		// ����
		float fEnd;		// �I��
		float fStopNow;	// ��~���Ԍ���
		float fStop;	// �I�����~����

		STime() : fNow(0.0f), fEnd(0.0f), fStopNow(0.0f), fStop(0.0f) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CCameraAction();	// �R���X�g���N�^
	~CCameraAction();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(CCamera* pCamera);

	/**
	@brief	�J�����̈ړ��n�_�ݒ�
	@param	pCamera		[in]	�ύX�������J����
	@param	rot			[in]	�ړ���̌���
	@param	fLength		[in]	�ړ���̋���
	@param	fTime		[in]	�ړ������܂ł̎���
	@param	fStopTime	[in]	�ړ������܂ł̎���
	@param	MOVE		[in]	�ړ����鎋�_
	@param	bNext		[in]	�������[�V����������\�肪���邩
	@return	void
	 */
	void Set(CCamera* pCamera, const D3DXVECTOR3& rot, const float& fLength, const float fTime, const float fStopTime, const MOVE& move, const bool& bNext = false);

	// �����o�֐�(�擾)
	bool IsFinish() { return m_bFinish; }
	bool IsNext() { return m_bNext; }
	bool IsPause() { return m_bPause; }

	// �����o�֐�(�ݒ�)
	void SetFinish(bool bFinish) { m_bFinish = bFinish; }
	void SetNext(bool bNext) { m_bNext = bNext; }
	void SetPause(bool bPause) { m_bPause = bPause; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void Adjust(D3DXVECTOR3& rot);

	// �����o�ϐ�
	SStartInfo m_startInfo;		// �J�n�n�_���
	STargetInfo m_targetInfo;	// �^�[�Q�b�g���
	STime m_time;				// ����
	bool m_bFinish;				// �I��
	bool m_bNext;				// ���̓��삪���邩�ǂ���
	bool m_bPause;				// �|�[�Y
	MOVE m_move;
};

#endif

