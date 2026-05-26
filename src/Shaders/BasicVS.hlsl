
cbuffer CBuf
{
	matrix transform;
}
float4 main(float2 pos : Pos) :  SV_Position
{
	return mul(float4(pos.x, pos.y, 1.f, 1.f), transform);
}