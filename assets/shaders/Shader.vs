uniform float4 PixelOffset : register(c5);

struct VSIn
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct VSOut
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};

VSOut main(VSIn vsIn)
{
	VSOut vsOut;
	vsOut.Position = float4(vsIn.Position.xyz, 1.0);
	vsOut.Position.xy += PixelOffset.xy;
	vsOut.TexCoord = vsIn.TexCoord;

	return vsOut;
}