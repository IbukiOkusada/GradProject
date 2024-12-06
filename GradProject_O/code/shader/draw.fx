float4x4 g_mtxWorld	: WORLD;		// ワールドマトリックス
float4x4 g_mtxView	: VIEW;			// ビューマトリックス
float4x4 g_mtxProj	: PROJECTION;	// プロジェクションマトリックス

float4 m_LightDir;                //平行光源の方向ベクトル
float4 m_LightCol;                //平行光源の色
float4   m_diffus = 0.0f;		  //オブジェクトのマテリアル色

sampler tex0 : register(s0);      //オブジェクトのテクスチャ
bool bUseTex = false;

struct VS_OUTPUT
{
    float4 Pos    : POSITION;
    float2 Tex    : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 Col    : COLOR0;
};
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

    Out.Normal = normalize(Normal.xyz);

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
   p = p * p;
   //テクスチャの使用可否確認
   if (bUseTex)
   {
       Out = p * tex2D(tex0, In.Tex) * m_diffus;
   }
   else
   {
       Out = m_diffus * p;
   }
   Out *= m_LightCol;
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