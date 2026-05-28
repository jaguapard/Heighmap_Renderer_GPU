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
    //TODO: force w to 0 in pos mult, and uncomment:
    //vso.pos.z = vso.pos.w; //Or not? Reverse z-buffer! infinity? Force 0 depth after perspecive divide. Z_NEAR?
    return vso;
}