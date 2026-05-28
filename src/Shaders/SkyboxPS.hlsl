
TextureCube tex;
SamplerState sam;
float4 main(uint id : SV_PrimitiveID, float3 worldPos : TEXCOORD0) : SV_Target
{
    float4 sample = tex.Sample(sam, worldPos);
    return sample;
}