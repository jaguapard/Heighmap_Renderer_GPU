#include "cbuff.hlsli"

float4 main(float3 pos : Pos, float2 uv: UV) : SV_Position
{
    return mul(float4(pos, 1), transform);
}