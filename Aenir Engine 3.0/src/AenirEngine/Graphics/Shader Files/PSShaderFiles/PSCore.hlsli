Texture2DArray g_TexArray : register(t0);
Texture1D g_RandomTex : register(t1);
SamplerState g_SamLinear : register(s0);

cbuffer CBInput : register(b0)
{
    float3 velocity;
    float lifeTime;
    float3 acceleration;
    int maxParticles;
    int emitCount;
    float deltaTime;
    float3 initalPos;
    float emitInterval;
}

struct Particle
{
    float3 Pos;
    float3 Velocity;
    float4 Color;
    float2 UV;
    float Alive;
    float Age;
    float2 padding;
};

float3 RandUnitVec3(float offset)
{
    float u = (deltaTime + offset);
    float3 v = g_RandomTex.SampleLevel(g_SamLinear, u, 0).xyz;
    return normalize(v);
}

float3 RandVec3(float offset)
{
    float u = (deltaTime + offset);
    float3 v = g_RandomTex.SampleLevel(g_SamLinear, u, 0).xyz;
    
    return v;
}


