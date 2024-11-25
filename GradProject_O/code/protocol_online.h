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
	const int MAX_SEND_DATA = 4096;	// �ő�f�[�^��

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
		COMMAND_MAX
	};
}

#endif