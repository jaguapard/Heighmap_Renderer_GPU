#pragma once
cbuffer CBuf
{
    matrix transform;
    float4 time, fieldSize;
    float4 camPos, lightDir;
}