//==========================================================
//
// �G�f�B�b�g��� [edit.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _EDIT_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _EDIT_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

// �G�f�B�b�g���ۑ��t�@�C����
namespace EDITFILENAME
{
	const std::string OBSTACLE =	"data\\FILE\\map\\obstacle.bin";
	const std::string ROAD =		"data\\FILE\\map\\road.bin";
	const std::string GIMMICK =		"data\\FILE\\map\\gimmick.bin";
	const std::string GOAL =		"data\\FILE\\map\\goal.bin";
}

//==========================================================
// �T���v���̃N���X��`
//==========================================================
class CEdit
{
public:	// �N�ł��A�N�Z�X�\�Ȓ�`

	enum TYPE
	{
		TYPE_OBJ = 0,	// ��Q��
		TYPE_ROAD,		// ��
		TYPE_GIMMICK,	// �M�~�b�N
		TYPE_GOAL,		// �S�[��
		TYPE_MAX
	};

public:	// �N�ł��A�N�Z�X�\

	CEdit();	// �R���X�g���N�^(�I�[�o�[���[�h)
	virtual ~CEdit();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void);
	virtual void Update(void) = 0;
	static CEdit* Create(const TYPE& type);

	// �����o�֐�(�擾)
	TYPE GetType() { return m_type; }

	// �����o�֐�(�ݒ�)

protected:

	void SetType(const TYPE& type) { m_type = type; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�
	TYPE m_type;
};

#endif
