#pragma once
cbuffer CBuf
{
    matrix view, projection, viewProjection;
    float4 time, fieldSize;
    float4 camPos, lightDir;
}