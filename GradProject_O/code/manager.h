//===============================================
//
// �}�l�[�W���[�̏��� [manager.h]
// Author : Ibuki Okusada
//
//===============================================
#ifndef _MANAGER_H_		// ���̃}�N������`����Ă��Ȃ��ꍇ
#define _MANAGER_H_		// ��d�C���N���[�h�h�~�p�}�N�����`

#include "main.h"
#include "fxmanager.h"
// �O���錾
class CRenderer;
class CInput;
class CDebugProc;
class CSound;
class CCamera;
class CLight;
class CTexture;
class CXFile;
class CFileLoad;
class CSlow;
class CFade;
class CMultiCamera;
class CMapCamera;
class CTime;
class CPlayer;
class CFileLoad;
class CEnemyRoute;
class CCarManager;
class CEnemyManager;
class CMiniMap;
class CDeltaTime;
class CFont;
class CNetWork;

//===============================================
// �V�[���N���X�̒�`
//===============================================
class CScene
{
public:

	// ���[�h�񋓌^
	enum MODE
	{
		MODE_TITLE = 0,	// �^�C�g�����
		MODE_ENTRY,     // �G���g���[���
		MODE_GAME,		// �Q�[�����
		MODE_RESULT,	// ���U���g���
		MODE_MULTI_RESULT,	// �}���`���U���g���
		MODE_MAX
	};

	CScene();
	~CScene();

	// �����o�֐�
	static CScene *Create(MODE mode);
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual CMapCamera *GetMapCamera(void) { return NULL; }
	virtual CPlayer *GetPlayer(void) { return NULL; }
	virtual CFileLoad *GetFileLoad(void) { return NULL; }
	virtual CTime* GetTime(void) { return nullptr; }
	void SetMode(MODE mode) { m_mode = mode; }
	MODE GetMode(void) { return m_mode; }
	virtual void SetID(const int id) {};
	virtual void ChangeFlag(bool value) {};
	virtual void EndTutorial(void) {};
	virtual void ReadyUp(const int id) {};

private:
	MODE m_mode;	// ���[�h
};

//===============================================
// �}�l�[�W���[�N���X�̒�`
//===============================================
class CManager
{
public:	// �N�ł��A�N�Z�X�\

	CManager();	// �R���X�g���N�^
	~CManager();	// �f�X�g���N�^

	// �����o�֐�
	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	// �ÓI�����o�֐�
	CRenderer *GetRenderer(void);
	CDebugProc *GetDebugProc(void);
	CSound *GetSound(void);
	CMultiCamera *GetCamera(void);
	CLight *GetLight(void);
	CTexture *GetTexture(void);
	CXFile *GetModelFile(void);
	CSlow *GetSlow(void);
	CFade *GetFade(void);
	CFont *GetFont(void);
	CDeltaTime *GetDeltaTime(void);
	
	void SetMode(CScene::MODE mode);
	CScene::MODE GetMode(void) { return m_pScene->GetMode(); }
	CScene *GetScene(void) { return m_pScene; }
	static CManager *GetInstance(void);
	static void Release(void);

	void SetDeliveryStatus(int nScore) { m_nDeliveryStatus = nScore; }
	int GetDeliveryStatus() { return m_nDeliveryStatus; }
	void SetLife(float nLife) { m_fLife = nLife; }
	float GetLife() { return m_fLife; }
	void SetSuccess(bool bSuccess) { m_bSuccess = bSuccess; }
	bool GetSuccess() { return m_bSuccess; }

private:	// �����������A�N�Z�X�\

	// �����o�֐�
	void DataReset(void);

	// �ÓI�����o�ϐ�
	CRenderer* m_pRenderer;	// �����_���[�̃|�C���^
	CInput* m_pInput;		// ���͂̃|�C���g
	CDebugProc* m_pDebugProc;	// �f�o�b�O�\���̃|�C���^
	CMultiCamera* m_pCamera;		// �J�����̃|�C���^
	CLight* m_pLight;		// ���C�g�̃|�C���^
	CTexture* m_pTexture;	// �e�N�X�`���̃|�C���^
	CXFile* m_pModelFile;	// X�t�@�C���̃|�C���^
	CSound* m_pSound;		// �T�E���h�̃|�C���^
	CSlow* m_pSlow;			// �X���[�̃|�C���^
	CScene* m_pScene;		// �V�[���̃|�C���^
	CFade* m_pFade;			// �t�F�[�h�̃|�C���^
	CDeltaTime* m_pDeltaTime;  // �^�C�}�[�̃|�C���^
	CFont* m_pFont;
	CNetWork* m_pNetWork;

	static CManager *m_pManager;	// �}�l�[�W���[�̃|�C���^

	int m_nDeliveryStatus;
	float m_fLife;
	bool m_bSuccess;
};


#endif

