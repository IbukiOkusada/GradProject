//==========================================================
//
// �}�b�v��Q����� [map_obstacle.h]
// Author : Ibuki Okusada
//
//==========================================================
#ifndef _MAP_OBSTACLE_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MAP_OBSTACLE_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "task.h"
#include "list.h"

//�O���錾
class CObjectX;

//==========================================================
// ��Q���̊��N���X��`
//==========================================================
class CMapObstacle : public CTask
{
public:	// �N�ł��A�N�Z�X�\�Ȓ�`

	// ��{���\����
	struct SInfo
	{
		D3DXVECTOR3 pos;	// ���W
		D3DXVECTOR3 rot;	// ����
		D3DXVECTOR3 scale;	// ��
		int fileidx;		// �t�@�C���ԍ�

		// �R���X�g���N�^
		SInfo() : pos(VECTOR3_ZERO), rot(VECTOR3_ZERO), scale(VECTOR3_ZERO), fileidx(-1) {}
		SInfo(const D3DXVECTOR3& _pos, const D3DXVECTOR3& _rot, const D3DXVECTOR3& _scale) : pos(_pos), rot(_rot), scale(scale), fileidx(-1) {}
		SInfo(const SInfo& _info) : pos(_info.pos), rot(_info.rot), scale(_info.scale), fileidx(_info.fileidx) {}
	};

public:	// �N�ł��A�N�Z�X�\

	CMapObstacle(const SInfo& info = SInfo());	// �R���X�g���N�^(�I�[�o�[���[�h)
	~CMapObstacle();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(void) override;
	HRESULT Init(const std::string& filename);
	void Uninit(void) override;
	void Update(void) override;
	
	// �擾�֐�
	D3DXVECTOR3& GetPosition() { return m_Info.pos; }
	D3DXVECTOR3& GetRotation() { return m_Info.rot; }
	D3DXVECTOR3& GetScale() { return m_Info.scale; }
	CObjectX* GetObj() { return m_pObj; }
	SInfo& GetInfo() { return m_Info; }

	// �ݒ�֐�
	void SetPosition(const D3DXVECTOR3& pos);
	void SetRotation(const D3DXVECTOR3& rot);
	void SetScale(const D3DXVECTOR3& scale);
	void BindModel(const int& idx);

	// �ÓI
	static CMapObstacle* Create(const SInfo& info);
	static Clist<CMapObstacle*>* GetList() { return m_pList; }
	static Clist<CMapObstacle*>* GetInstance() // ���X�g�擾
	{ if (m_pList == nullptr) { m_pList = m_pList->Create(); }return m_pList; }
	static void ListRelease() // ���X�g���
	{ if (m_pList != nullptr) { delete m_pList; m_pList = nullptr; } }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void DrawCheck();

	// �����o�ϐ�
	CObjectX* m_pObj;	// ��Q��
	SInfo m_Info;		// ��{���
	static Clist<CMapObstacle*>* m_pList;	// ���X�g
};

#endif