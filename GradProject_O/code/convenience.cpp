#include "manager.h"
#include "texture.h"
//=============================================
//�����擾�֐�
//=============================================
float GetDistance(D3DXVECTOR3 vec1, D3DXVECTOR3 vec2)
{
	D3DXVECTOR3 Dis = vec1 - vec2;
	if (Dis.x < 0.0f)
	{
		Dis.x *= -1.0f;
	}
	if (Dis.y < 0.0f)
	{
		Dis.y *= -1.0f;
	}
	if (Dis.z < 0.0f)
	{
		Dis.z *= -1.0f;
	}
	float fDis = Dis.x + Dis.y + Dis.z;
	return fDis;
}
//============================================================
//	�c������e�N�X�`���̃A�X�y�N�g����l�����������̎擾����
//============================================================
float GetTexWidthFromAspect(const float fHeight, const int nTexIdx)
{
	// �e�N�X�`���A�X�y�N�g����擾
	D3DXVECTOR2 aspect = CManager::GetInstance()->GetTexture()->GetTexFile(nTexIdx)->aspect;

	// �A�X�y�N�g�䂩��v�Z����������Ԃ�
	return fHeight * aspect.x;
}

//============================================================
//	���C�h������̃}���`�o�C�g������ϊ�
//============================================================
std::string WideToMultiByte(const std::wstring& rSrcStr)
{
	int nSrcSize = (int)rSrcStr.size();	// �ϊ��O�̕�����̃T�C�Y
	if (nSrcSize <= 0) { return ""; }	// �����񂪂Ȃ��ꍇ������

	// �ϊ���̕�����T�C�Y���擾
	int nDestSize = WideCharToMultiByte
	( // ����
		CP_ACP,				// �ϊ��R�[�h�y�[�W
		0,					// �ϊ��t���O
		&rSrcStr.front(),	// �ϊ��O������̐擪�A�h���X
		nSrcSize,			// �ϊ��O������̃T�C�Y
		nullptr,			// �ϊ��㕶����̐擪�A�h���X
		0,					// �ϊ��㕶����̃T�C�Y
		nullptr,			// �ϊ��s���̒u������
		nullptr				// �ϊ��s�ȕ��������݂�����
	);

	// �������ϊ�
	std::string sDest(nDestSize, '\0');	// �ϊ���̕�����
	WideCharToMultiByte
	( // ����
		CP_ACP,				// �ϊ��R�[�h�y�[�W
		0,					// �ϊ��t���O
		&rSrcStr.front(),	// �ϊ��O������̐擪�A�h���X
		nSrcSize,			// �ϊ��O������̃T�C�Y
		&sDest.front(),		// �ϊ��㕶����̐擪�A�h���X
		(int)sDest.size(),	// �ϊ��㕶����̃T�C�Y
		nullptr,			// �ϊ��s���̒u������
		nullptr				// �ϊ��s�ȕ��������݂�����
	);

	// �ϊ���̕������Ԃ�
	return sDest;
}

//=============================================
// �����␳(�S��)
//=============================================
void Adjust(D3DXVECTOR3& rot)
{
	// ����␳
	Adjust(rot.x);
	Adjust(rot.y);
	Adjust(rot.z);
}

//=============================================
// �����␳(�P��)
//=============================================
void Adjust(float& rot)
{
	// -3.14����3.14�̊Ԃɕ␳
	while (1)
	{
		if (rot > D3DX_PI)
		{
			rot += -D3DX_PI * 2;
		}
		else if (rot < -D3DX_PI)
		{
			rot += D3DX_PI * 2;
		}
		else
		{
			break;
		}
	}
}


// �x�N�g�����p�x�ɕϊ�����֐�
D3DXVECTOR3 VectorToAngles(const D3DXVECTOR3& vector)
{
	D3DXVECTOR3 angles;

	// Yaw�i���[�j���v�Z
	angles.y = atan2f(vector.x, vector.z);

	// Pitch�i�s�b�`�j���v�Z
	angles.x = atan2f(vector.y, sqrt(vector.x * vector.x + vector.z * vector.z));

	// Roll�i���[���j��0�x�ɐݒ�
	angles.z = 0.0f;



	return angles;
}

// �p�x���x�N�g���ɕϊ�����֐�
D3DXVECTOR3 AnglesToVector(const D3DXVECTOR3& angles)
{
	D3DXVECTOR3 vector;

	// ���W�A����x�ɕϊ�
	float yaw = (angles.y);
	float pitch = (angles.x);

	// �x�N�g�����v�Z
	vector.x = sinf(yaw) * cosf(pitch);
	vector.y = sinf(pitch);
	vector.z = cosf(yaw) * cosf(pitch);

	return vector;
}

//============================================================
//	�p�X�̃x�[�X�l�[���ϊ�
//============================================================
void PathToBaseName(std::string* pPath)
{
	std::filesystem::path fsPath(*pPath);	// �p�X

	// �p�X����x�[�X�l�[�����擾
	*pPath = fsPath.stem().string();
}

//============================================================
//	�}���`�o�C�g������̃��C�h������ϊ�
//============================================================
std::wstring MultiByteToWide(const std::string& rSrcStr)
{
	int nSrcSize = (int)rSrcStr.size();	// �ϊ��O�̕�����̃T�C�Y
	if (nSrcSize <= 0) { return L""; }	// �����񂪂Ȃ��ꍇ������

	// �������ϊ�
	std::wstring wsDest(nSrcSize, L'\0');	// �ϊ���̕�����
	int nDestSize = MultiByteToWideChar
	( // ����
		CP_ACP,				// �ϊ��R�[�h�y�[�W
		0,					// �ϊ��t���O
		&rSrcStr.front(),	// �ϊ��O������̐擪�A�h���X
		nSrcSize,			// �ϊ��O������̃T�C�Y
		&wsDest.front(),	// �ϊ��㕶����̐擪�A�h���X
		(int)wsDest.size()	// �ϊ��㕶����̃T�C�Y
	);

	// ������T�C�Y���C��
	wsDest.resize(nDestSize);

	// �ϊ���̕������Ԃ�
	return wsDest;
}