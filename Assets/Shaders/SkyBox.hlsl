struct VS_INPUT
{
	float4 position: POSITION0;
	float2 texcoord: TEXCOORD0;
	float3 normal: NORMAL0;
	float3 tangent: TANGENT0;
	float3 binormal: BINORMAL0;
};

struct VS_OUTPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
};


cbuffer constant: register(b0)
{
	row_major float4x4 world;
	row_major float4x4 view;
	row_major float4x4 proj;
};



VS_OUTPUT vsmain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//WORLD SPACE
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);
	output.texcoord = input.texcoord;

	return output;
}

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float2 texcoord: TEXCOORD0;
};

Texture2D Color: register(t0);
sampler ColorSampler: register(s0);

float4 psmain(PS_INPUT input) : SV_TARGET
{
	return Color.Sample(ColorSampler, input.texcoord);

}