
cbuffer CBuf
{
	matrix transform;
}
float4 main(float2 pos : Pos) :  SV_Position
{
    float func = 0; //TODO: calcuate function value
	return mul(float4(pos.x, func, pos.y, 1.f), transform);
}