float4 main(uint id : SV_PrimitiveID) : SV_Target
{
    float fid = id;
    return float4(fmod(fid / 21, 1), fmod(fid / 53, 1), fmod(fid / 93, 1), 1.f);
}