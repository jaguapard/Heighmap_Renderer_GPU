#include "cbuff.hlsli"

struct VSOut
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};
VSOut main(float3 pos : Pos, float2 uv: UV)
{
    VSOut vso;
    vso.pos = mul(float4(pos, 1), viewProjection);
    vso.uv = uv;
    return vso;
}