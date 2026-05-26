
cbuffer CBuf
{
	matrix transform;
    float time;
}
float4 main(float2 pos : Pos) :  SV_Position
{
    float func = sin(pos.x/600+time/6) * cos(pos.y/300+time/3) * (pos.x + pos.y)/3;
	return mul(float4(pos.x, func, pos.y, 1.f), transform);
}