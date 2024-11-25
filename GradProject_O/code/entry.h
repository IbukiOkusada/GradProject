//===============================================
//
// �G���g���[��ʂ̊Ǘ����� [entry.h]
// Author : Ryosuke Oohara
//
//===============================================
#ifndef _ENTRY_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _ENTRY_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "manager.h"

class CScore;
class CFileLoad;
class CMeshDome;
class CTime;
class CCharacter;
class CObject2D;
class CCamera;

//===============================================
// �G���g���[�N���X�̒�`(�h���N���X)
//===============================================
class CEntry : public CScene
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
	CEntry();	// �R���X�g���N�^
	~CEntry();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void SetScore(const int nScore) { m_nScore = nScore; }

private:

	// �����o�ϐ�
	CFileLoad* m_pFileLoad;	// �t�@�C���ǂݍ��݂̃|�C���^
	CMeshDome* m_pMeshSky;	// ��p
	CTime* m_pTime;			// �^�C�}�[
	CObject2D* m_pObjClear;	// clear�������ǂ���
	CPlayer* m_ppPlayer;	// �v���C���[�̃|�C���^
	CCamera** m_ppCamera;   // �J�����̃|�C���^
	static int m_nScore;
};

#endif