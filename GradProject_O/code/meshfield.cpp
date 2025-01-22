//==========================================================
//
// ���b�V���t�B�[���h�̏��� [meshfield.cpp]
// Author : Ibuki Okusada
//
//==========================================================
#include "meshfield.h"
#include "texture.h"
#include "manager.h"
#include "debugproc.h"
#include "input.h"
#include "player_manager.h"

Clist<CMeshField*>* CMeshField::m_List = nullptr;

//==========================================================
// �R���X�g���N�^
//==========================================================
CMeshField::CMeshField()
{
	if (m_List == nullptr)
	{
		m_List = m_List->Create();
	}

	m_List->Regist(this);
	m_bCollision = true;
	m_tex = D3DXVECTOR2(0.0f, 0.0f);
	m_texmove = D3DXVECTOR2(0.0f, 0.0f);
}

//==========================================================
// �f�X�g���N�^
//==========================================================
CMeshField::~CMeshField()
{
	m_List->Delete(this);

	if (m_List->Empty())
	{
		delete m_List;
		m_List = nullptr;
	}
}

//==========================================================
// ����������
//==========================================================
HRESULT CMeshField::Init(void)
{
	return S_OK;
}

//==========================================================
// �I������
//==========================================================
void CMeshField::Uninit(void)
{
	//���_�o�b�t�@�̔p��
	CObjectMesh::Uninit();

	
}

//==========================================================
// �X�V����
//==========================================================
void CMeshField::Update(void)
{
	// �e�N�X�`���X�V
	m_tex += m_texmove;
	if (m_tex.x < 0.0f)
	{
		m_tex.x += 1.0f;
	}
	else if (m_tex.x > 1.0f)
	{
		m_tex.x -= 1.0f;
	}
	if (m_tex.y < 0.0f)
	{
		m_tex.y += 1.0f;
	}
	else if (m_tex.y > 1.0f)
	{
		m_tex.y -= 1.0f;
	}

	SetTex();
}

//==========================================================
// �`�揈��
//==========================================================
void CMeshField::Draw(void)
{
	// �`��
	CObjectMesh::Draw();
}

//==========================================================
// ���_���ݒ�
//==========================================================
void CMeshField::SetVtxInfo(void)
{
	int nVertex = GetVertex();			// ���_�����擾
	int nNumWidth = GetNumWidth();		// ���������擾
	int nNumHeight = GetNumHeight();	// �����������擾

	//���_���W�̐ݒ�(��������E��O�Ɍ������Ē��_����ݒ肷��
	for (int nCntpVtx = 0; nCntpVtx < nVertex; nCntpVtx++)
	{
		//���_���W
		m_pVtx[nCntpVtx].pos.x = -(m_fWidth * nNumWidth) + (nCntpVtx % (nNumWidth + 1) * (m_fWidth * 2));
		m_pVtx[nCntpVtx].pos.y = 0.0f;
		m_pVtx[nCntpVtx].pos.z = (m_fHeight * nNumHeight) + ((nCntpVtx / (nNumWidth + 1) * (-m_fHeight * 2)));

		//�F
		m_pVtx[nCntpVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		m_pVtx[nCntpVtx].tex = D3DXVECTOR2(m_tex.x + 1.0f * (nCntpVtx % (nNumWidth + 1)), m_tex.y + 1.0f * (nCntpVtx / (nNumWidth + 1)));
	}

	// �@���x�N�g���̐ݒ�
	D3DXVECTOR3 nor, vec1, vec2;
	VERTEX_3D *aVtx[4];

	for (int nCntHeight = 0; nCntHeight < GetNumHeight(); nCntHeight++)
	{
		for (int nCntWidth = 0; nCntWidth < GetNumWidth(); nCntWidth++)
		{

			aVtx[0] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 0];
			aVtx[1] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 1];
			aVtx[2] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 1];
			aVtx[3] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 2];

			//Pos0����̃x�N�g�������߂�
			vec1 = aVtx[1]->pos - aVtx[0]->pos;
			vec2 = aVtx[2]->pos - aVtx[0]->pos;

			D3DXVec3Cross(&aVtx[0]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[0]->nor, &aVtx[0]->nor);	// �x�N�g���𐳋K������

			//Pos3����̃x�N�g�������߂�
			vec1 = aVtx[2]->pos - aVtx[3]->pos;
			vec2 = aVtx[1]->pos - aVtx[3]->pos;

			D3DXVec3Cross(&aVtx[3]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[3]->nor, &aVtx[3]->nor);	// �x�N�g���𐳋K������

			aVtx[1]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
			aVtx[2]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
		}
	}

	//for (int nCntpVtx = 0; nCntpVtx < nVertex; nCntpVtx++)
	//{
	//	if (nCntpVtx > 0 && nCntpVtx < nVertex - 1)
	//	{// �ŏ��ƍŌ�ȊO
	//		if (nCntpVtx > GetNumWidth() + 1 && nCntpVtx < (GetNumWidth() + 1) * GetNumHeight() &&
	//			nCntpVtx % (GetNumWidth() + 1) > 0 && nCntpVtx % (GetNumWidth() + 1) < GetNumWidth())
	//		{// 6�̖ʂƊւ���Ă���
	//			D3DXVECTOR3 norAdd = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//			norAdd += m_pVtx[nCntpVtx - 1].nor;
	//			norAdd += m_pVtx[nCntpVtx + 1].nor;
	//			norAdd += m_pVtx[GetNumWidth() + 1].nor;
	//			norAdd += m_pVtx[GetNumWidth() + 2].nor;
	//			norAdd += m_pVtx[GetNumWidth() - 1].nor;
	//			norAdd += m_pVtx[GetNumWidth() - 2].nor;
	//			m_pVtx[nCntpVtx].nor = norAdd / 6;
	//		}
	//	}
	//}

	// ���_�ݒ�
	SetVtx();
}

//==========================================================
// �e�N�X�`�����ݒ�
//==========================================================
void CMeshField::SetTex(void)
{
	int nVertex = GetVertex();			// ���_�����擾
	int nNumWidth = GetNumWidth();		// ���������擾

	// �e�N�X�`�����W(��������E��O�Ɍ������Ē��_����ݒ肷��
	for (int nCntpVtx = 0; nCntpVtx < nVertex; nCntpVtx++)
	{
		m_pVtx[nCntpVtx].tex = D3DXVECTOR2(m_tex.x + 1.0f * (nCntpVtx % (nNumWidth + 1)), m_tex.y + 1.0f * (nCntpVtx / (nNumWidth + 1)));
	}

	SetVtx();
}

//==========================================================
// ����
//==========================================================
CMeshField *CMeshField::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, 
	float fWidth, float fHeight, const char *pFileName, const int nWidth, const int nHeight, const int nPriority)
{
	CMeshField *pMeshField = NULL;	// ���b�V���t�B�[���h�̃|�C���^
	CTexture *pTexture = CManager::GetInstance()->GetTexture();	// �e�N�X�`���ւ̃|�C���^

	// �������̊m��
	pMeshField = DEBUG_NEW CMeshField;

	if (pMeshField != NULL)
	{// �m�ۂł����ꍇ

		// ������
		pMeshField->Init();

		// ���W�ݒ�
		pMeshField->SetPosition(pos);

		// �����ݒ�
		pMeshField->SetRotation(rot);

		// ���_����
		pMeshField->CObjectMesh::Create(nWidth, nHeight);

		// �T�C�Y�ݒ�
		pMeshField->SetSize(fWidth, fHeight);

		// �e�N�X�`���ݒ�
		pMeshField->BindTexture(pTexture->Regist(pFileName));
	}

	return pMeshField;
}


//==========================================================
// ���ݒ�
//==========================================================
void CMeshField::SetSize(float fWidth, float fHeight)
{
	// �T�C�Y�ݒ�
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	// ���_���ݒ�
	SetVtxInfo();
}

//==========================================================
// �N���ǂݍ���
//==========================================================
void CMeshField::UpDownLoad(const char *pFileName)
{
	FILE *pFile;	// �t�@�C���ւ̃|�C���^
	int nVertex = 0;

	pFile = fopen(pFileName, "r");

	if (pFile != NULL)
	{//�t�@�C�����J�����ꍇ
		char aStr[256];

		//�J�n�����܂œǂݍ���
		while (1)
		{
			//�e�L�X�g�ǂݍ���
			int nResult = fscanf(pFile, "%s", &aStr[0]);

			if (strcmp(&aStr[0], "HEIGHT") == 0)
			{//�X�N���v�g�J�n�̕������m�F�ł����ꍇ
				fscanf(pFile, "%s", &aStr[0]);	// (=)�ǂݍ���
				fscanf(pFile, "%f", &m_pVtx[nVertex].pos.y);	// �����ǂݍ���
				nVertex++;

				if (nVertex >= GetVertex())
				{
					break;
				}
			}
			else if (nResult == EOF)
			{//�t�@�C���̍Ō�܂œǂݍ���ł��܂����ꍇ
				break;
			}
		}
		//�t�@�C�������
		fclose(pFile);
	}
	else
	{//�t�@�C�����J���Ȃ������ꍇ
		return;
	}

	// �@���x�N�g���̐ݒ�
	D3DXVECTOR3 nor, vec1, vec2;
	VERTEX_3D *aVtx[4];

	for (int nCntHeight = 0; nCntHeight < GetNumHeight(); nCntHeight++)
	{
		for (int nCntWidth = 0; nCntWidth < GetNumWidth(); nCntWidth++)
		{

			aVtx[0] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 0];
			aVtx[1] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + 1];
			aVtx[2] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 1];
			aVtx[3] = &m_pVtx[nCntHeight * (GetNumWidth() + 1) + nCntWidth + GetNumWidth() + 2];

			//Pos0����̃x�N�g�������߂�
			vec1 = aVtx[1]->pos - aVtx[0]->pos;
			vec2 = aVtx[2]->pos - aVtx[0]->pos;

			D3DXVec3Cross(&aVtx[0]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[0]->nor, &aVtx[0]->nor);	// �x�N�g���𐳋K������

			//Pos3����̃x�N�g�������߂�
			vec1 = aVtx[2]->pos - aVtx[3]->pos;
			vec2 = aVtx[1]->pos - aVtx[3]->pos;

			D3DXVec3Cross(&aVtx[3]->nor, &vec1, &vec2);

			D3DXVec3Normalize(&aVtx[3]->nor, &aVtx[3]->nor);	// �x�N�g���𐳋K������

			aVtx[1]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
			aVtx[2]->nor = (aVtx[0]->nor + aVtx[3]->nor) / 2;
		}
	}

	// ���_���ݒ�
	SetVtx();
}
