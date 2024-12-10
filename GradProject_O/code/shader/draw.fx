float4x4 g_mtxWorld	: WORLD;		// ���[���h�}�g���b�N�X
float4x4 g_mtxView	: VIEW;			// �r���[�}�g���b�N�X
float4x4 g_mtxProj	: PROJECTION;	// �v���W�F�N�V�����}�g���b�N�X
float4x4 g_mMatScaleReverse;
float4 m_LightDir;                //���s�����̕����x�N�g��
float4 m_LightCol;                //���s�����̐F
float4 viewDir;                    //���_����
float4 m_diffus = 0.0f;		  //�I�u�W�F�N�g�̃}�e���A���g�U�F
float4 m_ambient = 0.0f;		  //�I�u�W�F�N�g�̃}�e���A�����F
float4  m_Emissive = 0.0f;//�I�u�W�F�N�g�̃}�e���A�������F
float4  m_specula = 0.0f;//�I�u�W�F�N�g�̃}�e���A�����ːF
float m_power=0.0f;//���ˋ��x
sampler tex0 : register(s0);      //�I�u�W�F�N�g�̃e�N�X�`��
bool bUseTex = false;

//============================================================
// ���C�g�̍\����
struct Light
{
    float3 position;  // ���C�g�̈ʒu (�|�C���g���C�g�̏ꍇ)
    float3 direction; // �X�|�b�g���C�g�̕���
    float3 color;     // ���C�g�̐F
    float intensity;  // ���̋��x
    float range;      // ���C�g�̉e���͈�
    float spotAngle;  // �X�|�b�g���C�g�̊p�x (�X�|�b�g���C�g�̏ꍇ)
};

// �������C�g�p�̔z��
#define MAX_LIGHTS 8
Light lights[MAX_LIGHTS];
int numLights = 0; // �g�p���郉�C�g�̐�
//============================================================
// �o�͍\����
struct VS_OUTPUT
{
    float4 Pos    : POSITION;
    float2 Tex    : TEXCOORD0;
    float3 Normal : TEXCOORD1;
};

float3 TransformNormal(float3 normal, float3x3 inverseTransposeWorld)
{
    // �@����ϊ�
    float3 transformedNormal = mul(normal, inverseTransposeWorld);

    // ���K�����ĕ�����ۂ�
    return normalize(transformedNormal);
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
  // p = p * p;
   //�e�N�X�`���̎g�p�ۊm�F
  
 
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