//==========================================================
//
// X�t�@�C�����f���̏����S�� [objectX.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _OBJECTX_H_
#define _OBJECTX_H_

#include "main.h"
#include "object.h"

// �}�N����`
#define NUM_TEXTURE	(64)	// �e�N�X�`���ő吔

//**********************************************************
// X�t�@�C�����f���N���X�̒�`
//**********************************************************
class CObjectX : public CObject
{
public:	// �����������A�N�Z�X�\

	enum TYPE
	{
		TYPE_NORMAL = 0,	// ��]
		TYPE_QUATERNION,	// �N�H�[�^�j�I��
		TYPE_MAX
	};

	
public:	// �N�ł��A�N�Z�X�\

	CObjectX(int nPriority = 0);	// �R���X�g���N�^
	virtual ~CObjectX();	// �f�X�g���N�^

	// �����o�֐�
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void BindFile(int nIdx);
	void BindFile(const char* file);

	static CObjectX *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, const char *pFileName, const int nPriority = 4);
	void SetRotSize(D3DXVECTOR3 &SetMax, D3DXVECTOR3 &SetMin, D3DXVECTOR3 vtxMax, D3DXVECTOR3 vtxMin, float fRot);

	// �����o�֐�(�ݒ�)
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetScale(const D3DXVECTOR3& scale);
	void SetEnableCollision(const bool bEnable) { m_bEnableCollision = bEnable; }
	void SetRotateType(const TYPE& type) { m_Type = type; }

	// �����o�֐�(�擾)
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3* GetPos(void) { return &m_pos; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	CObject2D *GetObject2D(void) { return NULL; }
	static CObjectX *GetTop(void) { return m_pTop; }
	CObjectX *GetNext(void) { return m_pNext; }
	virtual D3DXMATRIX *GetMtx(void) { return &m_mtxWorld; }
	int GetIdx(void) { return m_nIdxModel; }
	void ListOut(void);
	int GetModelType(void) { return m_nIdxModel; }
	D3DXVECTOR3& GetVtxMax(void);
	D3DXVECTOR3& GetVtxMin(void);
	D3DXCOLOR& GetColMuliti() { return m_ColMulti; }
	void SetColMulti(const D3DXCOLOR& col) { m_ColMulti = col; }
	D3DXCOLOR& GetColAdd() { return m_AddCol; }
	void SetColAdd(const D3DXCOLOR& col) { m_AddCol = col; }

protected:
	void Quaternion();
	void DrawOnry();
	void CalWorldMtx();

private:	// �����������A�N�Z�X�\

	// �����o�֐�

	// �����o�ϐ�c
	static CObjectX *m_pTop;	// �擪�̃I�u�W�F�N�g�ւ̃|�C���^
	static CObjectX *m_pCur;	// �Ō���̃I�u�W�F�N�g�ւ̃|�C���^
	CObjectX *m_pPrev;	// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CObjectX *m_pNext;	// ���̃I�u�W�F�N�g�ւ̃|�C���^
	D3DXVECTOR3 m_pos;	// �ʒu
	D3DXVECTOR3 m_rot;	// ����
	D3DXVECTOR3 m_scale;	// �g��k��
	D3DXMATRIX m_mtxWorld;	//���[���h�}�g���b�N�X
	D3DXCOLOR m_ColMulti;
	D3DXCOLOR m_AddCol;
	TYPE m_Type;			// ��]���
	int m_nIdxModel;		// ���f���ԍ�
	bool m_bEnableCollision;	//�����蔻��̗L���E����
};

#endif
