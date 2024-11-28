//==========================================================
//
// �I�����C������M�v���g�R�� [protocol_online.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PROTOCOL_ONLINE_H_
#define _PROTOCOL_ONLINE_H_

//**********************************************************
// ����M�p�v���g�R���񋓂��`
//**********************************************************
namespace NetWork
{
	const int MAX_CONNECT = 4;	// �ő�ڑ���
	const int MAX_COMMAND_DATA = ((sizeof(int) + sizeof(int) + 1024));	// �f�[�^�̃}�b�N�X�T�C�Y
	const int MAX_SEND_DATA = ((sizeof(int) + sizeof(int) + 1024));	// �ő�f�[�^��
	const double SEND_MS = 50.0f;	// ��ɑ���f�[�^�̑��M�t���[��

	// �R�}���h��
	enum COMMAND
	{
		COMMAND_NONE = 0,		// �������Ȃ�
		COMMAND_JOIN,			// �ڑ�����
		COMMAND_GETID,			// ID�擾
		COMMAND_DELETE,			// �폜
		COMMAND_PL_POS,			// �v���C���[���W
		COMMAND_PL_ROT,			// �v���C���[�̌���
		COMMAND_PL_DAMAGE,		// �v���C���\�_���[�W
		COMMAND_PL_GOAL,		// �v���C���[�S�[��
		COMMAND_GM_HIT,			// �M�~�b�N�ɏՓ�
		COMMAND_NEXT_GOAL,		// ���̃S�[������
		COMMAND_MAX
	};

	// ���ԑ���p�N���X
	class CTime
	{
		// �f�[�^
		clock_t start;
		clock_t end;

	public:

		// �R���X�g���N�^
		CTime() : start(), end() {}

		// �����o�֐�
		void Start() { start = clock(); }	// �J�n
		void End() { end = clock(); }		// �I��
		bool IsOK()	// �w�莞�Ԍo�߂������ǂ���
		{
			const double time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;
			return time >= SEND_MS;
		}
	};
}

#endif