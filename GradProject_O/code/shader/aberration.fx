sampler2D SceneTexture : register(s0);

float2 ChromaticOffset = float2(0.0025, 0.0025);  // 色収差の強さ（調整可能）

float4 PS_ChromaticAberration(float2 texCoord : TEXCOORD0) : COLOR
{
    // Rチャネル：左上にシフト
    float4 colorR = tex2D(SceneTexture, texCoord + ChromaticOffset * float2(-1.0, -1.0));

    // Gチャネル：そのまま
    float4 colorG = tex2D(SceneTexture, texCoord);

    // Bチャネル：右下にシフト
    float4 colorB = tex2D(SceneTexture, texCoord + ChromaticOffset * float2(1.0, 1.0));

    // 各チャネルを再合成
    return float4(colorR.r, colorG.g, colorB.b, 1.0);
}

technique ChromaticAberration
{
    pass P0
    {
        PixelShader = compile ps_2_0 PS_ChromaticAberration();
    }
}