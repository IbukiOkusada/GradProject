//==========================================================
//
// �^�X�N�̏��� [task.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _TASK_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _TASK_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CTask
{
public:	// �N�ł��A�N�Z�X�\

	// ��ޗ񋓌^�̒�`
	enum TYPE
	{
		TYPE_NONE = 0,	// �����Ȃ�
		TYPE_BG,		// �w�i
		TYPE_PLAYER,	// �v���C���[
		TYPE_ENEMY,		// �G
		TYPE_EFFECT,	// �G�t�F�N�g
		TYPE_MODEL,		// ���f��
		TYPE_CAR,		// ��
		TYPE_PAUSE,		// �|�[�Y���
		TYPE_MAP,		// �}�b�v�\��
		TYPE_MAX
	};

public:

	CTask();	// �R���X�g���N�^
	virtual ~CTask();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) = 0; 
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	TYPE GetType(void) { return m_type; }
	bool GetDeath(void) { return m_bDeath; }

protected:	// �h���N���X������A�N�Z�X�\

	// �����o�֐�
	void Release(void);

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	bool m_bDeath;	// ���S�t���O
	TYPE m_type;	// ���
};

#endif


