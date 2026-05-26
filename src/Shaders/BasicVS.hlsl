
cbuffer CBuf
{
	matrix transform;
    float time;
}
struct VSOut
{
    float4 transformedPos : SV_Position;
    float3 originalWorldPos : WorldPos;
};
VSOut main(float2 pos : Pos)
{
    float func = sin(pos.x/600+time/6) * cos(pos.y/300+time/3) * (pos.x + pos.y)/3;
    func *= sin(time / 2);
    VSOut ret;
    ret.originalWorldPos = float3(pos.x, func, pos.y);
    ret.transformedPos = mul(float4(ret.originalWorldPos, 1.f), transform);
    return ret;
}