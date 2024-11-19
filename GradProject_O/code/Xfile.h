//==========================================================
//
// X�t�@�C���Ǘ� [modelfile.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MODELFILE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MODELFILE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "list.h"

namespace XFile
{
	const D3DXVECTOR3 START_VTXMAX = D3DXVECTOR3(-100000.0f, -100000.0f, -100000.0f);
	const D3DXVECTOR3 START_VTXMIN = D3DXVECTOR3(100000.0f, 100000.0f, 100000.0f);
}

//==========================================================
// X�t�@�C�����̃N���X��`
//==========================================================
class CXFile
{
public:	// �N�ł��A�N�Z�X�\�Ȓ�`

	// X�t�@�C�����
	struct SFileData
	{
		LPD3DXMESH pMesh;		// ���b�V��(���_���)�ւ̃|�C���^
		LPD3DXBUFFER pBuffMat;	// �}�e���A���ւ̃|�C���^
		int *pIdexTexture;		// �e�N�X�`���ԍ�
		DWORD dwNumMat;			// �}�e���A���̐�
		D3DXVECTOR3 vtxMin;		// �ŏ�
		D3DXVECTOR3 vtxMax;		// �ő�

		// �R���X�g���N�^
		SFileData() : pMesh(nullptr), pBuffMat(nullptr), pIdexTexture(nullptr), 
			dwNumMat(), vtxMin(XFile::START_VTXMIN), vtxMax(XFile::START_VTXMAX) {}
	};

private:	// �����������A�N�Z�X�\�Ȓ�`

	// �t�@�C���ǂݍ��ݏ��
	struct SFileInfo
	{
		std::string name;		// �t�@�C����
		SFileData data;			// �t�@�C�����

		// �R���X�g���N�^
		SFileInfo() : name(""), data(SFileData()) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CXFile();	// �R���X�g���N�^
	~CXFile();	// �f�X�g���N�^

	// �����o�֐�
	 int Regist(const char *pFileName);
	 void Unload(void);
	 void SetSizeVtxMax(int nIdx, D3DXVECTOR3 vtxMax) { m_List[nIdx]->data.vtxMax = vtxMax; }
	 void SetSizeVtxMin(int nIdx, D3DXVECTOR3 vtxMin) { m_List[nIdx]->data.vtxMin = vtxMin; }

	 // �����o�֐�(�擾)
	 SFileData*SetAddress(int nIdx);
	 int GetNumAll(void) { return m_List.size(); }
	 D3DXVECTOR3 GetMax(int nIdx) { return m_List[nIdx]->data.vtxMax; }
	 D3DXVECTOR3 GetMin(int nIdx) { return m_List[nIdx]->data.vtxMin; }
	 const char *GetFileName(int nIdx) { return m_List[nIdx]->name.c_str(); }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	 HRESULT FileLoad(int nIdx);

	// �����o�ϐ�
	std::vector<SFileInfo*> m_List;
};

#endif

