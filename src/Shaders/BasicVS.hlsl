#include "surface_func.hlsli"
cbuffer CBuf
{
	matrix transform;
    float time;
}
struct VSOut
{
    float3 originalWorldPos : WorldPos;
    float4 transformedPos : SV_Position;
};
VSOut main(float2 pos : Pos)
{
    float func = surface_func(pos, time);
    VSOut ret;
    ret.originalWorldPos = float3(pos.x, func, pos.y);
    ret.transformedPos = mul(float4(ret.originalWorldPos, 1.f), transform);
    return ret;
}