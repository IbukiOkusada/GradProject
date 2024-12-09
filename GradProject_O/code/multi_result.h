//===============================================
//
// �}���`���U���g��ʂ̊Ǘ����� [multi_result.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _MULTI_RESULT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MULTI_RESULT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "manager.h"

//===============================================
// ���U���g�N���X�̒�`(�h���N���X)
//===============================================
class CMultiResult : public CScene
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
	CMultiResult();	// �R���X�g���N�^
	~CMultiResult();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	enum TYPE_OBJ
	{
		TYPE_TIME = 0,
		TYPE_LIFE,
		TYPE_EVAL,
		TYPE_RANKING,
		TYPE_NUM
	};

	// �����o�ϐ�
	CFileLoad* m_pFileLoad;			// �t�@�C���ǂݍ��݂̃|�C���^
	CTime* m_pTime;					// �^�C�}�[
};

#endif
