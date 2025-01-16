sampler2D SceneTexture : register(s0);

float2 ChromaticOffset = float2(0.0025, 0.0025);  // �F�����̋����i�����\�j

float4 PS_ChromaticAberration(float2 texCoord : TEXCOORD0) : COLOR
{
    // R�`���l���F����ɃV�t�g
    float4 colorR = tex2D(SceneTexture, texCoord + ChromaticOffset * float2(-1.0, -1.0));

    // G�`���l���F���̂܂�
    float4 colorG = tex2D(SceneTexture, texCoord);

    // B�`���l���F�E���ɃV�t�g
    float4 colorB = tex2D(SceneTexture, texCoord + ChromaticOffset * float2(1.0, 1.0));

    // �e�`���l�����č���
    return float4(colorR.r, colorG.g, colorB.b, 1.0);
}

technique ChromaticAberration
{
    pass P0
    {
        PixelShader = compile ps_2_0 PS_ChromaticAberration();
    }
}