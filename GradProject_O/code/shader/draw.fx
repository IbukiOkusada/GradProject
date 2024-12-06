float4x4 g_mtxWorld	: WORLD;		// ���[���h�}�g���b�N�X
float4x4 g_mtxView	: VIEW;			// �r���[�}�g���b�N�X
float4x4 g_mtxProj	: PROJECTION;	// �v���W�F�N�V�����}�g���b�N�X

float4 m_LightDir;                //���s�����̕����x�N�g��
float4 m_LightCol;                //���s�����̐F
float4   m_diffus = 0.0f;		  //�I�u�W�F�N�g�̃}�e���A���F

sampler tex0 : register(s0);      //�I�u�W�F�N�g�̃e�N�X�`��
bool bUseTex = false;

struct VS_OUTPUT
{
    float4 Pos    : POSITION;
    float2 Tex    : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 Col    : COLOR0;
};
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

    Out.Normal = normalize(Normal.xyz);

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
   //�e�N�X�`���̎g�p�ۊm�F
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