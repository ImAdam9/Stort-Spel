RWTexture2D<unorm float4> backBuffer : register(u0);
//RWTexture2D<unorm float4> glowTexture : register(u1);
//Texture2D glow : register(t0);
//Texture2D glow1 : register(t1);
//Texture2D glow2 : register(t2);
//Texture2D glow3 : register(t3);
//Texture2D glow4 : register(t4);

static const int WIDTH = 32;
//static const float div = (1080.f / 1980.f);

static const int HEIGHT = 32;

cbuffer indexer
{
	float4 switcher;
}

float Luminosity(in float3 clr)
{
	return sqrt(dot(clr, float3(0.299f, 0.587f, 0.114f)));
}

//static const float oneFilter[5] =
//{
//	0.15338835280702454,
//	0.22146110682534667,
//	0.2503010807352574,
//	0.22146110682534667,
//	0.15338835280702454,
//};

[numthreads(WIDTH, HEIGHT, 1)]

void main(uint3 DTid : SV_DispatchThreadID)
{
	//int3 sampleIndices = int3(DTid.xy, 0);
	//float4 map;
	//switch (switcher.x)
	//{
		//case 0:
		//	map = backBuffer[DTid.xy];
		//	break;
		//case 1:
		//	map = glow.Load(sampleIndices);
		//	break;
		//case 2:
		//	map = glow1.Load(sampleIndices);
		//	break;
		//case 3:
		//	map = glow2.Load(sampleIndices);
		//	break;
		//case 4:
		//	map = glow3.Load(sampleIndices);
		//	break;
		//case 5:
		//	map = glow4.Load(sampleIndices);
		//	break;
		//case 6:
		//	map = float4(.2f, .2f, .2f, 1.f);
		//	break;
		//default:
		//	map = backBuffer[DTid.xy];
		//	break;

	//}
	
	//float avarage = 0.f;
	
	//float kernel = switcher.x;
	//float kernel = 0;
	//float divider = 1.f / switcher.x;
	
	
	/*for (int i = -kernel; i <= kernel; i++)
	{
		avarage += Luminosity(backBuffer[DTid.xy + i].xyz);
	}
	avarage /= ((kernel * 2) + 1);
	
	//backBuffer[DTid.xy]
	
	
	if (avarage >= switcher.y && switcher.w != 0.f)
	{
		float4 color = backBuffer[DTid.xy];
		float brightness = (color.r * .02126f) + (color.y * .7152f) + (color.z * .0722);
		backBuffer[DTid.xy] = color * brightness;

	}
	//backBuffer[DTid.xy] = float4(.2f, .2f, .2f, 1.f);
	//if (Luminosity(backBuffer[DTid.xy].xyz) > switcher.y)
	//	backBuffer[DTid.xy] = float4(.2f, .2f, .2f, 1.f);
	
	//backBuffer[DTid.xy] = map;
	//backBuffer[DTid.xy] = float4(1.f, 1.f, 1.f, 1.f) - backBuffer[DTid.xy];
*/
	float4 color = backBuffer[DTid.xy];
	float brightness = (color.r * .2126f) + (color.y * .7152f) + (color.z * .0722f);
	if (brightness > switcher.y)
	{
		backBuffer[DTid.xy] = color;

	}
	else
		backBuffer[DTid.xy] = float4(0, 0, 0, 0);

}