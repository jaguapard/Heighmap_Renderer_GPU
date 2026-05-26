cbuffer CBuf
{
    matrix transform;
    float time;
    float3 camPos;
}
float4 main(float3 worldPos : WorldPos, uint id : SV_PrimitiveID) : SV_Target
{
    float fid = id;
    float4 color = float4(fmod(fid / 21, 1), fmod(fid / 53, 1), fmod(fid / 93, 1), 1.f);
    float dist = length((worldPos - camPos).xyz);
    float scaledDist = dist / 5000;
    float distShadingMult = saturate(exp(-scaledDist));
    return color * distShadingMult;
}