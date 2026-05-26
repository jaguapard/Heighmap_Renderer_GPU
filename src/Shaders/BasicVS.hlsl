
float4 main(float2 pos : Pos) :  SV_Position
{
	return float4(pos.x, pos.y, 1.f, 1.f);
}