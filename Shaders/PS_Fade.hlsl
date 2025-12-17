
Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);

Texture2D<float4> MaskTexture : register(t1);

cbuffer Parameters : register(b1)
{
    float Rate;
    int flag;
};

float4 main(float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    float4 col = Texture.Sample(TextureSampler, texCoord) * color;
    float4 mask = MaskTexture.Sample(TextureSampler, texCoord);

    // 横方向にワイプ
//    float rate = saturate((texCoord.x - 1.0f) + Rate * 2.0f);
//    float rate = saturate(-texCoord.x + Rate * 2.0f);

    // マスク用テクスチャを使用
    float rate = ((mask.x - 1.0f) + Rate * 2.0f);
    rate = step(0, rate);
    
    return float4(0.01f, 0.02f, 0.08f, rate);
}
