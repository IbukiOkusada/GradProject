//==========================================================
//
// �I�����C���ΐ푗��M�v���g�R�� [protocol_online.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _PROTOCOL_RANKING_H_
#define _PROTOCOL_RANKING_H_

// �}�N����`

//**********************************************************
// ����M�p�v���g�R���񋓂��`
//**********************************************************
namespace NetWork
{
	const int MAX_CONNECT = 4;	// �ő�ڑ���

	// �R�}���h��
	enum COMMAND
	{
		COMMAND_NONE = 0,		// �������Ȃ�
		COMMAND_JOIN,			// �ڑ�����
		COMMAND_GETID,			// ID�擾
		COMMAND_DELETE,			// �폜
		COMMAND_MAX
	};
}

#endif