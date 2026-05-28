
TextureCube tex;
SamplerState sam;
float4 main(uint id : SV_PrimitiveID, float2 uv : TEXCOORD0) : SV_Target
{
    return tex.Sample(sam, float3(uv, 1));
    float r = id & 1;
    float g = id & 2;
    float b = id & 4;
    float3 cl;
    if (r == 0 && g == 0 && b == 0) cl = float3(1,1,1);
    else cl = float3(r,g,b);
    
    cl *= (id + 1) / 25.f;
    return float4(cl, 1);
}