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