#include "cbuff.hlsli"

TextureCube tex;
SamplerState sam;

static const float SUN_RADIUS_RADIANS = radians(1);
float4 main(uint id : SV_PrimitiveID, float3 worldPos : TEXCOORD0) : SV_Target
{
    float3 dir = normalize(worldPos);
    float d = dot(dir, normalize(lightDir.xyz));
    if (d >= cos(SUN_RADIUS_RADIANS)) return float4(1,1,1,1);
    float4 sample = tex.Sample(sam, worldPos);
    return sample;
}