
cbuffer CBuf
{
	matrix transform;
}
float4 main(float2 pos : Pos) :  SV_Position
{
    float func = sin(pos.x/600) * cos(pos.y/300) * sqrt(pos.x * pos.x + pos.y * pos.y); //TODO: calcuate function value
	return mul(float4(pos.x, func, pos.y, 1.f), transform);
}