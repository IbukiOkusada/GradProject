float4x4 g_mtxWorld	: WORLD;		// ワールドマトリックス
float4x4 g_mtxView	: VIEW;			// ビューマトリックス
float4x4 g_mtxProj	: PROJECTION;	// プロジェクションマトリックス
float4x4 g_mMatScaleReverse;
float4 m_LightDir;                //平行光源の方向ベクトル
float4 m_LightCol;                //平行光源の色
float4 viewDir;                    //視点方向
float4 viewPos;                    //視点
float4 m_diffus = 0.0f;		  //オブジェクトのマテリアル拡散色
float4 m_ambient = 0.0f;		  //オブジェクトのマテリアル環境色
float4  m_Emissive = 0.0f;//オブジェクトのマテリアル発光色
float4  m_specula = 0.0f;//オブジェクトのマテリアル反射色
float m_power=0.0f;//反射強度

float4 s_FogColor = float4(0.25f, 0.3f, 0.37f, 0.6f);		// フォグ色
float  s_FogNear = 500.0f;								// フォグの開始位置
float  s_FogFar = 30000.0f;								// フォグの終了位置

textureCUBE CubeMapSampler;
samplerCUBE cubeTexSampler =
sampler_state {
    Texture = <CubeMapSampler>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

sampler tex0 : register(s0);      //オブジェクトのテクスチャ
bool bUseTex = false;

//============================================================
// ライトの構造体
struct Light
{
    float3 position;    // ライトの位置 (12バイト)
    float  intensity;   // 光の強度 (4バイト, 16バイト境界に合わせる)

    float3 direction;   // スポットライトの方向 (12バイト)
    float  range;       // ライトの影響範囲 (4バイト, 16バイト境界に合わせる)

    float3 color;       // ライトの色 (12バイト)
    float  spotAngle;   // スポットライトの角度 (4バイト, 16バイト境界に合わせる)
};

// 複数ライト用の配列
#define MAX_LIGHTS 20
cbuffer MyBuffer : register(b0) {
    Light lights[MAX_LIGHTS];
   int numLights = 0; // 使用するライトの数
}


//============================================================
// 出力構造体
struct VS_OUTPUT
{
    float4 Pos    : POSITION;
    float2 Tex    : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 PosWVP : TEXCOORD2;
};

float3 TransformNormal(float3 normal, float3x3 inverseTransposeWorld)
{
    // 法線を変換
    float3 transformedNormal = mul(normal, inverseTransposeWorld);

    // 正規化して方向を保つ
    return normalize(transformedNormal);
}

//============================================================
// 光源計算
//============================================================
float4 CalculateLighting(float3 normal, float3 position, float3 viewDir)
{
    float3 finalColor = float3(0, 0, 0);
    float sp = 0.0f;
    for (int i = 0; i < numLights; i++)
    {
        // ライトからの方向を計算
        float3 lightDir = normalize(lights[i].position - position);
        float3 halfVector = normalize(lightDir + viewDir);

        // 点光源の減衰を計算 (範囲内のみに影響)
        float distance = length(lights[i].position - position);
        float attenuation = saturate(1.0 - (distance / lights[i].range));

        // 拡散光の計算
        float diffuse = saturate(dot(normal, lightDir)) * lights[i].intensity;

        // 鏡面反射光の計算
        float specular = pow(saturate(dot(normal, halfVector)), 16) * lights[i].intensity;
        sp += specular;
        if (lights[i].spotAngle > 0)
        {

            float spotEffect = saturate(dot(normalize(lights[i].direction), -lightDir));
            if (spotEffect > cos(lights[i].spotAngle))
            {
                float inner = cos(lights[i].spotAngle * 0.25f);
                float outer = cos(lights[i].spotAngle);
                float falloff;
                if (spotEffect > inner)
                {
                    falloff = 1.0f;
                }
                else
                {
                    falloff= saturate((spotEffect - outer) / (inner - outer));
                }
                // スポットライト内の計算
                finalColor += spotEffect * (diffuse + specular) * lights[i].color * attenuation* falloff ;
            }
        }
        else
        {
            // ライトの色を加算
            finalColor += (diffuse + specular) * lights[i].color * attenuation;
        }
        
    }

    return float4(finalColor, m_diffus.a);
}
//============================================================
//	環境マッピング
//============================================================
float4 PS_EnvironmentMap(float3 ReflectionVector : TEXCOORD3) : COLOR
{
    // キューブマップから反射ベクトルに基づいて色をサンプリング
    return texCUBE(cubeTexSampler, ReflectionVector);
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
    Out.PosWVP = mul(Pos, g_mtxWorld);
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
   p = p * p;
  
 
   Out = m_diffus * p * m_LightCol;
   Out += m_ambient * m_LightCol;
   Out.w = m_diffus.w;

   Out.rgb += m_Emissive.rgb;
   float3 reflection = reflect(-m_LightDir.xyz, In.Normal.xyz);
   float specFactor = saturate(dot(reflection, viewDir));
   float specPower = pow(specFactor, m_power);
   Out += m_specula * m_LightCol * specPower;
 
   Out += CalculateLighting(In.Normal.xyz,In.PosWVP.xyz,viewDir.xyz);
   float3 fdef = 1.0f;
 
   Out.w = m_diffus.w;
   if ((tex2D(tex0, In.Tex).r + tex2D(tex0, In.Tex).g + tex2D(tex0, In.Tex).b) != 0.0f)
   {
       In.Tex.xy = frac(In.Tex.xy);
       Out *= tex2D(tex0, In.Tex);
   }
   float4 refCol = PS_EnvironmentMap(reflect(m_LightDir.xyz, In.Normal.xyz)) * m_specula;
   Out.xyz += refCol.xyz;
   Out.w += (refCol.x + refCol.y + refCol.z) * 0.333;

    //===============================
    //			フォグ
    //===============================
   float d = distance(In.PosWVP.xyz, viewPos.xyz);
   float f = (s_FogFar - d) / (s_FogFar - s_FogNear); //フォグの適応割合をもとめる
   f = clamp(f, 0.0f, 1.0f);
 
   Out.rgb = Out.rgb * (f)+s_FogColor.rgb * (1.0f - f);
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