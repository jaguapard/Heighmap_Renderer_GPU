
TextureCube tex;
SamplerState sam;
float4 main(uint id : SV_PrimitiveID, float2 uv : TEXCOORD0) : SV_Target
{
    float4 sample = tex.Sample(sam, float3(uv, 1));
    return sample;
}