//============================================================
//
//	��������e�L�X�g2D�w�b�_�[ [scrollText2D.h]
//	Author�F���c�E��
//
//============================================================
//************************************************************
//	��d�C���N���[�h�h�~
//************************************************************
#ifndef _SCROLL_TEXT2D_H_
#define _SCROLL_TEXT2D_H_

//************************************************************
//	�C���N���[�h�t�@�C��
//************************************************************
#include "text2D.h"
#include "sound.h"

//************************************************************
//	�N���X��`
//************************************************************
// ��������e�L�X�g2D�N���X
class CScrollText2D : public CText2D
{
public:
	// �R���X�g���N�^
	CScrollText2D();

	// �f�X�g���N�^
	~CScrollText2D() override;

	// �I�[�o�[���C�h�֐�
	HRESULT Init() override;	// ������
	void Uninit() override;		// �I��
	void Update() ;	// �X�V
	void Draw() ;	// �`��
	void SetEnableDraw(const bool bDraw) ;	// �`��󋵐ݒ�
	HRESULT PushFrontString(const std::string& rStr) override;	// ������̐擪�ǉ� (�}���`�o�C�g������)
	HRESULT PushFrontString(const std::wstring& rStr) override;	// ������̐擪�ǉ� (���C�h������)
	HRESULT PushBackString(const std::string& rStr) override;	// ������̍Ō���ǉ� (�}���`�o�C�g������)
	HRESULT PushBackString(const std::wstring& rStr) override;	// ������̍Ō���ǉ� (���C�h������)
	void DeleteString(const int nStrIdx) override;				// ������폜
	void DeleteStringAll() override;							// ������S�폜

	// �ÓI�����o�֐�
	static CScrollText2D* Create	// ����
	( // ����
		const std::string& rFilePath,			// �t�H���g�p�X
		const bool bItalic,						// �C�^���b�N
		const D3DXVECTOR3& rPos,					// ���_�ʒu
		const float fNextTime = 0.1f,			// �����\���̑ҋ@����
		const float fCharHeight = 100.0f,		// �����c��
		const float fLineHeight = 100.0f,		// �s�ԏc��
		const EAlignX alignX = XALIGN_CENTER,	// ���z�u
		const EAlignY alignY = YALIGN_CENTER,	// �c�z�u
		const D3DXVECTOR3& rRot = VECTOR3_ZERO,		// ���_����
		const D3DXCOLOR& rCol = D3DXCOLOR(1.0f,1.0,1.0f,1.0f)		// �F
	);

	// �����o�֐�
	inline void SetNextTime(const float fNextTime)			{ m_fNextTime = fNextTime; }		// �����\���̑ҋ@���Ԑݒ�
	inline float GetNextTime() const						{ return m_fNextTime; }				// �����\���̑ҋ@���Ԏ擾
	inline void SetEnableScroll(const bool bScroll)			{ m_bScroll = bScroll; }			// ��������󋵐ݒ�
	inline bool IsScroll() const							{ return m_bScroll; }				// ��������󋵎擾
	inline int GetNextCharIdx()								{ return m_nNextIdx; }				// ���̕\�������C���f�b�N�X�擾
	inline bool GetEnd() { return m_bEnd; }				// ���̕\�������C���f�b�N�X�擾
	inline CChar2D* GetNextChar2D()							{ return m_vecChar[m_nNextIdx]; }	// ���̕\�������擾

private:
	// �����o�֐�
	void UpdateScroll(const float fDeltaTime);	// ��������X�V
	void PlayScrollSE(CChar2D* pChar2D);		// ����������ʉ��̍Đ�

	// �����o�ϐ�
	std::vector<CChar2D*> m_vecChar;	// �S�������
	int m_nNextIdx;		// ���\�����镶���C���f�b�N�X
	float m_fNextTime;	// ���\���܂ł̑ҋ@����
	float m_fCurTime;	// ���݂̑ҋ@����
	bool m_bScroll;		// ���������
	bool m_bEnd;
};

#endif	// _SCROLL_TEXT2D_H_
