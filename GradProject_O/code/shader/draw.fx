float4x4 g_mtxWorld	: WORLD;		// ���[���h�}�g���b�N�X
float4x4 g_mtxView	: VIEW;			// �r���[�}�g���b�N�X
float4x4 g_mtxProj	: PROJECTION;	// �v���W�F�N�V�����}�g���b�N�X
float4x4 g_mMatScaleReverse;
float4 m_LightDir;                //���s�����̕����x�N�g��
float4 m_LightCol;                //���s�����̐F
float4 viewDir;                    //���_����
float4 viewPos;                    //���_
float4 m_diffus = 0.0f;		  //�I�u�W�F�N�g�̃}�e���A���g�U�F
float4 m_ambient = 0.0f;		  //�I�u�W�F�N�g�̃}�e���A�����F
float4  m_Emissive = 0.0f;//�I�u�W�F�N�g�̃}�e���A�������F
float4  m_specula = 0.0f;//�I�u�W�F�N�g�̃}�e���A�����ːF
float m_power=0.0f;//���ˋ��x

float4 s_FogColor = float4(0.25f, 0.3f, 0.37f, 0.6f);		// �t�H�O�F
float  s_FogNear = 500.0f;								// �t�H�O�̊J�n�ʒu
float  s_FogFar = 30000.0f;								// �t�H�O�̏I���ʒu

textureCUBE CubeMapSampler;
samplerCUBE cubeTexSampler =
sampler_state {
    Texture = <CubeMapSampler>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;
};

sampler tex0 : register(s0);      //�I�u�W�F�N�g�̃e�N�X�`��
bool bUseTex = false;

//============================================================
// ���C�g�̍\����
struct Light
{
    float3 position;    // ���C�g�̈ʒu (12�o�C�g)
    float  intensity;   // ���̋��x (4�o�C�g, 16�o�C�g���E�ɍ��킹��)

    float3 direction;   // �X�|�b�g���C�g�̕��� (12�o�C�g)
    float  range;       // ���C�g�̉e���͈� (4�o�C�g, 16�o�C�g���E�ɍ��킹��)

    float3 color;       // ���C�g�̐F (12�o�C�g)
    float  spotAngle;   // �X�|�b�g���C�g�̊p�x (4�o�C�g, 16�o�C�g���E�ɍ��킹��)
};

// �������C�g�p�̔z��
#define MAX_LIGHTS 20
cbuffer MyBuffer : register(b0) {
    Light lights[MAX_LIGHTS];
   int numLights = 0; // �g�p���郉�C�g�̐�
}


//============================================================
// �o�͍\����
struct VS_OUTPUT
{
    float4 Pos    : POSITION;
    float2 Tex    : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 PosWVP : TEXCOORD2;
};

float3 TransformNormal(float3 normal, float3x3 inverseTransposeWorld)
{
    // �@����ϊ�
    float3 transformedNormal = mul(normal, inverseTransposeWorld);

    // ���K�����ĕ�����ۂ�
    return normalize(transformedNormal);
}

//============================================================
// �����v�Z
//============================================================
float4 CalculateLighting(float3 normal, float3 position, float3 viewDir)
{
    float3 finalColor = float3(0, 0, 0);
    float sp = 0.0f;
    for (int i = 0; i < numLights; i++)
    {
        // ���C�g����̕������v�Z
        float3 lightDir = normalize(lights[i].position - position);
        float3 halfVector = normalize(lightDir + viewDir);

        // �_�����̌������v�Z (�͈͓��݂̂ɉe��)
        float distance = length(lights[i].position - position);
        float attenuation = saturate(1.0 - (distance / lights[i].range));

        // �g�U���̌v�Z
        float diffuse = saturate(dot(normal, lightDir)) * lights[i].intensity;

        // ���ʔ��ˌ��̌v�Z
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
                // �X�|�b�g���C�g���̌v�Z
                finalColor += spotEffect * (diffuse + specular) * lights[i].color * attenuation* falloff ;
            }
        }
        else
        {
            // ���C�g�̐F�����Z
            finalColor += (diffuse + specular) * lights[i].color * attenuation;
        }
        
    }

    return float4(finalColor, m_diffus.a);
}
//============================================================
//	���}�b�s���O
//============================================================
float4 PS_EnvironmentMap(float3 ReflectionVector : TEXCOORD3) : COLOR
{
    // �L���[�u�}�b�v���甽�˃x�N�g���Ɋ�Â��ĐF���T���v�����O
    return texCUBE(cubeTexSampler, ReflectionVector);
}
//============================================================
//	���_���W�̎ˉe�ϊ�
//============================================================
float4 TransVertex
(
    in float4	inPos		: POSITION,		// ���_���W
    in float4x4	inMtxWorld : WORLD,		// ���[���h�}�g���b�N�X
    in float4x4	inMtxView : VIEW,			// �r���[�}�g���b�N�X
    in float4x4	inMtxProj : PROJECTION	// �v���W�F�N�V�����}�g���b�N�X
)
{
    // ���_���W���ˉe�ϊ�
    inPos = mul(inPos, inMtxWorld);
    inPos = mul(inPos, inMtxView);
    inPos = mul(inPos, inMtxProj);

    // �ϊ���̒��_���W��Ԃ�
    return inPos;
}
//============================================================
//	���_�V�F�[�_�[
//============================================================
VS_OUTPUT VS(float4 Pos    : POSITION,
    float4 Normal : NORMAL,
    float2 Tex : TEXCOORD0)
{
    VS_OUTPUT Out;
    //���_���W�̎ˉe�ϊ�
    Out.Pos = TransVertex(Pos, g_mtxWorld, g_mtxView, g_mtxProj);
    Out.PosWVP = mul(Pos, g_mtxWorld);
    Out.Tex = Tex;
    Out.Normal = TransformNormal(Normal, (float3x3)g_mMatScaleReverse);
    Out.Normal = normalize(Out.Normal.xyz);
   // Out.Normal = normalize(Normal.xyz);
    return Out;
}
//============================================================
//	�t���O�����g�V�F�[�_�[
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
    //			�t�H�O
    //===============================
   float d = distance(In.PosWVP.xyz, viewPos.xyz);
   float f = (s_FogFar - d) / (s_FogFar - s_FogNear); //�t�H�O�̓K�����������Ƃ߂�
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