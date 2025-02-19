//====================================
//エフェクシアコントロールクラス
//Author: 丹野 竜之介
//====================================
#ifndef  _EFFEKSEERCONTROL_H_//二重インクルード防止のマクロ
#define _EFFEKSEERCONTROL_H_
//インクルード
#include "main.h"
#include "convenience.h"
#include <vector>
#include <functional>
#include <string>
class CEffekseer
{
public:
	//クラス内クラス
	class CEffectData//エフェクト情報
	{
	public:

		CEffectData();
		~CEffectData();
		void Erase();
		bool GetExist();
		void Trigger(int nValue);
		::Effekseer::Vector3D m_pos;	//座標
		::Effekseer::Vector3D m_rot;	//向き
		::Effekseer::Vector3D m_move;	//移動量
		Effekseer::Handle handle;		//本体
		Effekseer::EffectRef efcRef;	//参照情報
		std::string Path;				//パス
		::Effekseer::Vector3D m_Scale;	//スケール
		bool m_bLoop;					//ループ
		bool m_bAutoDelete;				//自動削除の可否
	};

	CEffekseer();
	~CEffekseer();
	void Init();
	void Update();
	void Draw();

	void Uninit();

	void AllClear();
	void Loading(const std::string& filename);
	Effekseer::EffectRef Regist(const std::string& filename);
	CEffectData* Create(std::string path, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, float fScale = 1.0f, bool bLoop = false, bool bAutoDelete = true);
	static CEffekseer* GetInstance() {
		if (pInstance == NULL) { pInstance = DEBUG_NEW CEffekseer;  return pInstance; }
		else
		{
			return pInstance;
		}
	}
	static void Release()
	{
			SAFE_DELETE(pInstance)
	}
	Effekseer::ManagerRef GetManager() { return m_EfkManager; }
	std::vector<CEffectData*> GetList() { return m_vEffect; }
	std::map<std::string, Effekseer::EffectRef> GetMap(){ return mapEffekseer; }
protected:
	std::function<void()> onLostDevice;
	std::function<void()> onResetDevice;

private:
	static CEffekseer* pInstance;						//コントロールクラス本体
	std::vector<CEffectData *> m_vEffect;				//エフェクトプール
	EffekseerRendererDX9::RendererRef m_efkRenderer;	//レンダラー
	Effekseer::ManagerRef m_EfkManager;					//マネージャ
	int32_t time = 0;									//時間
	std::map<std::string, Effekseer::EffectRef> mapEffekseer;
};


#endif // ! _EFFEKSEERCONTROL_H_