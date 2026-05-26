
float surface_func(float2 pos, float time)
{
    float f = sin(pos.x / 600 + time / 6) * cos(pos.y / 300 + time / 3) * (pos.x + pos.y) / 3;
    f *= sin(time / 2);
    return f;
}