#include "cbuff.hlsli"

struct VSOut
{
    float4 pos : SV_Position;
    float3 worldPos : TEXCOORD0;
};
VSOut main(float3 pos : Pos, float2 uv: UV)
{
    VSOut vso;
    vso.pos = mul(float4(pos, 0), viewProjection);
    vso.worldPos = pos;
    //TODO: force w to 0 in pos mult, and uncomment:
    //vso.pos.z = vso.pos.w; //Or not? Reverse z-buffer! infinity? Force 0 depth after perspecive divide. Z_NEAR?
    vso.pos.z = vso.pos.w;
    return vso;
}