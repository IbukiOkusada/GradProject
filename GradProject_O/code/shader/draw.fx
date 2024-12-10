float4x4 g_mtxWorld	: WORLD;		// ワールドマトリックス
float4x4 g_mtxView	: VIEW;			// ビューマトリックス
float4x4 g_mtxProj	: PROJECTION;	// プロジェクションマトリックス
float4x4 g_mMatScaleReverse;
float4 m_LightDir;                //平行光源の方向ベクトル
float4 m_LightCol;                //平行光源の色
float4 viewDir;                    //視点方向
float4 m_diffus = 0.0f;		  //オブジェクトのマテリアル拡散色
float4 m_ambient = 0.0f;		  //オブジェクトのマテリアル環境色
float4  m_Emissive = 0.0f;//オブジェクトのマテリアル発光色
float4  m_specula = 0.0f;//オブジェクトのマテリアル反射色
float m_power=0.0f;//反射強度
sampler tex0 : register(s0);      //オブジェクトのテクスチャ
bool bUseTex = false;

//============================================================
// ライトの構造体
struct Light
{
    float3 position;  // ライトの位置 (ポイントライトの場合)
    float3 direction; // スポットライトの方向
    float3 color;     // ライトの色
    float intensity;  // 光の強度
    float range;      // ライトの影響範囲
    float spotAngle;  // スポットライトの角度 (スポットライトの場合)
};

// 複数ライト用の配列
#define MAX_LIGHTS 8
Light lights[MAX_LIGHTS];
int numLights = 0; // 使用するライトの数
//============================================================
// 出力構造体
struct VS_OUTPUT
{
    float4 Pos    : POSITION;
    float2 Tex    : TEXCOORD0;
    float3 Normal : TEXCOORD1;
};

float3 TransformNormal(float3 normal, float3x3 inverseTransposeWorld)
{
    // 法線を変換
    float3 transformedNormal = mul(normal, inverseTransposeWorld);

    // 正規化して方向を保つ
    return normalize(transformedNormal);
}
//============================================================
//	頂点座標の射影変換
//============================================================
float4 TransVertex
(
    in float4	inPos		: POSITION,		// 頂点座標
    in float4x4	inMtxWorld : WORLD,		// ワールドマトリックス
    in float4x4	inMtxView : VIEW,			// ビューマトリックス
    in float4x4	inMtxProj : PROJECTION	// プロジェクションマトリックス
)
{
    // 頂点座標を射影変換
    inPos = mul(inPos, inMtxWorld);
    inPos = mul(inPos, inMtxView);
    inPos = mul(inPos, inMtxProj);

    // 変換後の頂点座標を返す
    return inPos;
}
//============================================================
//	頂点シェーダー
//============================================================
VS_OUTPUT VS(float4 Pos    : POSITION,
    float4 Normal : NORMAL,
    float2 Tex : TEXCOORD0)
{
    VS_OUTPUT Out;
    //頂点座標の射影変換
    Out.Pos = TransVertex(Pos, g_mtxWorld, g_mtxView, g_mtxProj);
    Out.Tex = Tex;
    Out.Normal = TransformNormal(Normal, (float3x3)g_mMatScaleReverse);
    Out.Normal = normalize(Out.Normal.xyz);
   // Out.Normal = normalize(Normal.xyz);
    return Out;
}
//============================================================
//	フラグメントシェーダー
//============================================================
float4 PS(VS_OUTPUT In) : COLOR0
{
   float4 Out;
   float p = dot(In.Normal, -m_LightDir.xyz);
   p = p * 0.5f + 0.5f;
  // p = p * p;
   //テクスチャの使用可否確認
  
 
   Out = m_diffus * p * m_LightCol;
   Out += m_ambient * m_LightCol;
   Out.w = m_diffus.w;
   if ((tex2D(tex0, In.Tex).r+ tex2D(tex0, In.Tex).g+ tex2D(tex0, In.Tex).b)!= 0.0f)
   {
       In.Tex.xy = frac(In.Tex.xy);
       Out *= tex2D(tex0, In.Tex);
   }
   Out.rgb += m_Emissive.rgb;
   float3 reflection = reflect(-m_LightDir.xyz, In.Normal.xyz);
   float specFactor = saturate(dot(reflection, viewDir));
   float specPower = pow(specFactor, m_power);
   Out += m_specula * m_LightCol * specPower;
   return Out;
}

technique TShader
{
    pass P0
    {
        VertexShader = compile vs_3_0 VS();
        PixelShader = compile ps_3_0 PS();
    }
}