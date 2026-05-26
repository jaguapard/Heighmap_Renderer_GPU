#include "surface_func.hlsli"
cbuffer CBuf
{
    matrix transform;
    float time;
    float3 camPos;
}
float4 main(float3 worldPos : WorldPos, uint id : SV_PrimitiveID) : SV_Target
{
    float fid = id;
    float4 base_color = float4(fmod(fid / 21, 1), fmod(fid / 53, 1), fmod(fid / 93, 1), 1.f);
    float dist = length((worldPos - camPos).xyz);
    float scaledDist = dist / 5000;
    float distShadingMult = saturate(exp(-scaledDist));
    
    float derivativeStep = 1;
    float pos2d = float2(worldPos.x, worldPos.z);
    float f = surface_func(pos2d, time);
    float f_stepX = surface_func(pos2d + float2(derivativeStep, 0), time);
    float f_stepY = surface_func(pos2d + float2(0, derivativeStep), time);
    float dx = (f_stepX - f) / derivativeStep;
    float dy = (f_stepY - f) / derivativeStep;
    
    float3 normal = normalize(float3(dx, -1, dy)); //-1 intentional
    
    float3 lightDir = normalize(float3(0, -1, 0));
    float normalShadingMult = max(0.05, dot(lightDir, normal));
    
    return base_color * distShadingMult * normalShadingMult;
}