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