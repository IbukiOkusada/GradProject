//==========================================================
//
// ������ [road.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "road.h"
#include "object3D.h"
#include "road_manager.h"
#include "manager.h"
#include "texture.h"
#include "effect3D.h"
namespace
{
	const char* FILENAME[CRoad::TYPE_MAX] =	{
		"data\\TEXTURE\\road\\road009.jpg",
		"data\\TEXTURE\\road\\road005.jpg",
		"data\\TEXTURE\\road\\road006.jpg",
		"data\\TEXTURE\\road\\road007.jpg",
		"data\\TEXTURE\\road\\road008.jpg",
	};
}

//==========================================================
// �R���X�g���N�^
//==========================================================
CRoad::CRoad(const SInfo& info)
{
	// �l�̃N���A
	m_Info = info;
	m_pObj = nullptr;
	m_nIdx = -1;
	m_Type = TYPE_MAX;

	for (int i = 0; i < DIRECTION::DIC_MAX; i++)
	{
		m_apConnectRoad[i] = nullptr;
		m_aSearchRoad[i] = SSearch();
	}

	m_nIdx = CRoadManager::GetInstance()->GetList()->GetNum();
	CRoadManager::GetInstance()->ListIn(this);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CRoad::~CRoad()
{

}

//==========================================================
// ����������
//==========================================================
HRESULT CRoad::Init(void)
{
	// �I�u�W�F�N�g�̐���
	CTexture* pTex = CManager::GetInstance()->GetTexture();
	m_pObj = CObject3D::Create(m_Info.pos, m_Info.rot);
	m_pObj->SetpVtx(m_Info.size.x, m_Info.size.y);
	m_pObj->BindTexture(pTex->Regist(FILENAME[TYPE_CROSSING]));
	m_Searchself.pos = m_Info.pos;
	m_Searchself.pRoad = this;
	for (int i = 0; i < 4; i++)
	{
		m_aVtxPos[i] = VECTOR3_ZERO;
	}
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CRoad::Uninit(void)
{
	// �A�����H�̋L����p��
	for (int i = 0; i < DIRECTION::DIC_MAX; i++)
	{
		m_apConnectRoad[i] = nullptr;
		m_aSearchRoad[i] = SSearch();
	}

	// �`��I�u�W�F�N�g��p��
	if (m_pObj != nullptr)
	{
		m_pObj->Uninit();
		m_pObj = nullptr;
	}

	// ���X�g����J��
	CRoadManager::GetInstance()->ListOut(this);

	// �J��
	Release();
}

//==========================================================
// �X�V����
//==========================================================
void CRoad::Update(void)
{

}

//==========================================================
// ����
//==========================================================
CRoad* CRoad::Create(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR2& size)
{
	// ����
	CRoad* pRoad = nullptr;
	pRoad = DEBUG_NEW CRoad(SInfo(pos, rot, size));

	// ��������
	if (pRoad != nullptr)
	{
		// ����������
		pRoad->Init();
	}

	return pRoad;
}

//==========================================================
// �A��
//==========================================================
void CRoad::Connect(CRoad* pRoad, const DIRECTION dic)
{
	// �͈͊O�m�F
	if (dic < DIRECTION::DIC_UP || dic >= DIRECTION::DIC_MAX) { return; }

	m_apConnectRoad[dic] = pRoad;

	if (pRoad != nullptr)
	{
		m_apConnectLength[dic] = D3DXVec3Length(&(m_Info.pos - pRoad->GetPosition()));
	}
	else
	{
		m_apConnectLength[dic] = FLT_MAX;
	}
}

//==========================================================
// �o�H�T���p�A��
//==========================================================
void CRoad::SearchConnect(CRoad* pRoad, const DIRECTION dic)
{
	// �͈͊O�m�F
	if (dic < DIRECTION::DIC_UP || dic >= DIRECTION::DIC_MAX) { return; }

	m_aSearchRoad[dic].pRoad = pRoad;

	if (pRoad != nullptr)
	{
		m_aSearchRoad[dic].pos = pRoad->GetPosition();
		m_aSearchRoad[dic].bActive = true;
		return;
	}

	// �g�p����Ă��Ȃ���Ԃɂ���
	m_aSearchRoad[dic] = SSearch();
}

//==========================================================
// �e�N�X�`���ݒ�
//==========================================================
void CRoad::BindTexture()
{
	int num = 0;
	CTexture* pTex = CManager::GetInstance()->GetTexture();

	// �����m�F
	for (int i = 0; i < DIC_MAX; i++)
	{
		if (m_apConnectRoad[i] == nullptr) { continue; }
		num++;
	}

	// ���蓖��
	m_Type = static_cast<TYPE>(num);

	// �������ǂ������m�F
	if (num == 2 && 
		((m_apConnectRoad[DIC_UP] != nullptr && m_apConnectRoad[DIC_DOWN] != nullptr) || 
		(m_apConnectRoad[DIC_LEFT] != nullptr && m_apConnectRoad[DIC_RIGHT] != nullptr)))
	{
		m_Type = TYPE_NONE;
	}
	else if (num == 1)
	{
		m_Type = TYPE_STOP;
	}


	// �͈͊O�Ȃ�e�N�X�`�������蓖�ĂȂ�
	if (m_Type >= TYPE_MAX) { return; }

	if (m_pObj == nullptr) { return; }
	m_pObj->BindTexture(pTex->Regist(FILENAME[m_Type]));

	// �����ݒ�
	Rotation(m_Type);
}

//==========================================================
// ��]
//==========================================================
void CRoad::Rotation(TYPE type)
{
	switch (type)
	{
	case CRoad::TYPE_STOP:
	{
		if (m_apConnectRoad[DIC_UP] != nullptr)
		{
			m_Info.rot.y = D3DX_PI;
		}
		else if (m_apConnectRoad[DIC_LEFT] != nullptr)
		{
			m_Info.rot.y = D3DX_PI * 0.5f;
		}
		else if (m_apConnectRoad[DIC_RIGHT] != nullptr)
		{
			m_Info.rot.y = -D3DX_PI * 0.5f;
		}
	}
		break;
	case CRoad::TYPE_NONE:
	{
		if (m_apConnectRoad[DIC_LEFT] != nullptr && m_apConnectRoad[DIC_RIGHT] != nullptr)
		{
			m_Info.rot.y = D3DX_PI * 0.5f;
		}
	}
		break;
	case CRoad::TYPE_CURVE:
	{
		if (m_apConnectRoad[DIC_UP] != nullptr)
		{
			if (m_apConnectRoad[DIC_LEFT] != nullptr)
			{
				m_Info.rot.y = D3DX_PI;
			}
			else if (m_apConnectRoad[DIC_RIGHT] != nullptr)
			{
				m_Info.rot.y = -D3DX_PI * 0.5f;
			}
		}
		else if (m_apConnectRoad[DIC_DOWN] != nullptr)
		{
			if (m_apConnectRoad[DIC_LEFT] != nullptr)
			{
				m_Info.rot.y = D3DX_PI * 0.5f;
			}
			else if (m_apConnectRoad[DIC_RIGHT] != nullptr)
			{
				m_Info.rot.y = 0.0f;
			}
		}
	}
		break;
	case CRoad::TYPE_T_JUNCTION:
	{
		if (m_apConnectRoad[DIC_UP] == nullptr)
		{
			m_Info.rot.y = D3DX_PI * 0.5f;
		}
		else if (m_apConnectRoad[DIC_DOWN] == nullptr)
		{
			m_Info.rot.y = -D3DX_PI * 0.5f;
		}
		else if (m_apConnectRoad[DIC_LEFT] == nullptr)
		{
			m_Info.rot.y = 0.0f;
		}
		else if (m_apConnectRoad[DIC_RIGHT] == nullptr)
		{
			m_Info.rot.y = D3DX_PI;
		}
	}
		break;
	default:
		break;
	}

	// 4/1��]
	if (m_Info.rot.y == -D3DX_PI * 0.5f || m_Info.rot.y == D3DX_PI * 0.5f)
	{
		float f = m_Info.size.x;
		m_Info.size.x = m_Info.size.y;
		m_Info.size.y = f;
		m_pObj->SetpVtx(m_Info.size.x, m_Info.size.y);
	}

	m_pObj->SetRotation(m_Info.rot);

	// ���W�擾
	VERTEX_3D* pVtx = m_pObj->GetVertex();
	for (int i = 0; i < 4; i++)
	{
		m_aVtxPos[i] = pVtx[i].pos;
	}
}

//==========================================================
// ���W
//==========================================================
void CRoad::SetPosition(const D3DXVECTOR3& pos)
{
	m_Info.pos = pos;
	
	if (m_pObj == nullptr) { return; }

	m_pObj->SetPosition(m_Info.pos);
}

//==========================================================
// �T�C�Y
//==========================================================
void CRoad::SetSize(const D3DXVECTOR2& size)
{
	m_Info.size = size;
	m_Info.sizeOrigin = size;

	if (m_pObj == nullptr) { return; }

	m_pObj->SetpVtx(m_Info.size.x, m_Info.size.y);
}

//==========================================================
// �w������̌����_�܂ł�����o��
//==========================================================
bool CRoad::GetJunctionRoad(float fRot, CRoad** pGoalOut, CRoad** pGoalPrevOut)
{
	// �ŏ��Ɍ������������Y�o
	DIRECTION dic = GetDic(fRot);

	// �����i�V
	if (dic == DIRECTION::DIC_MAX) { return false; }

	// �A�����Ă��Ȃ�
	if (!m_aSearchRoad[dic].bActive) { return false; }

	// �A������������_�𓱂��o��
	{
		CRoad* pThis = this;
	}
	CRoad* pRoad = m_aSearchRoad[dic].pRoad;
	CRoad* pOld = this;

	// �s���~�܂�܂Ŋm�F
	while (pRoad->GetType() != TYPE::TYPE_STOP)
	{
		// �����_�Ȃ�I��!
		if (pRoad->GetType() == TYPE::TYPE_T_JUNCTION || pRoad->GetType() == TYPE::TYPE_CROSSING)
		{
			*pGoalPrevOut = pOld;
			*pGoalOut = pRoad;
			return true;
		}

		// �����_�ł͂Ȃ��̂Ŋm�F���܂�
		CRoad* pCheck = nullptr;

		for (int i = 0; i < DIRECTION::DIC_MAX; i++)
		{
			pCheck = pRoad->m_aSearchRoad[i].pRoad;

			// �������Ȃ�m�F���Ȃ�
			if (pCheck == pOld) { continue; }
			if (pCheck == nullptr) { continue; }

			break;
		}

		// �����Ȃ�
		if (pCheck == nullptr)
		{
			break;
		}

		// �s���~�܂�
		if (pCheck->GetType() == TYPE::TYPE_STOP)
		{
			break;
		}

		// �J�[�u�̏ꍇ����Ɍ�������
		pOld = pRoad;
		pRoad = pCheck;			// ����
	}

	// ���݂��Ă��Ȃ��̂Œ��g�i�V
	pGoalOut = nullptr;
	pGoalPrevOut = nullptr;

	return false;
}

//==========================================================
// ��������������Y�o
//==========================================================
CRoad::DIRECTION CRoad::GetDic(float fRot)
{
	float range = D3DX_PI * 0.25f;

	// ��������Ȃ��������������߂�
	if (fRot <= range && fRot >= -range)
	{// ��
		return DIRECTION::DIC_DOWN;
	}
	else if (fRot <= -range && fRot >= -range * 3)
	{// ��
		return DIRECTION::DIC_LEFT;
	}
	else if (fRot <= range * 3 && fRot >= range)
	{// �E
		return DIRECTION::DIC_RIGHT;
	}
	else if (fRot >= range * 3 && fRot <= -range * 3)
	{// ��
		return DIRECTION::DIC_UP;
	}

	return DIRECTION::DIC_MAX;
}