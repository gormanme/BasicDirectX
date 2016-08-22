Texture2D shaderTexture;
SamplerState SampleType;


cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


cbuffer LightBuffer
{
	float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};


struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


//--- Vertex Shader ---//
PixelInputType LightVertexShader( VertexInputType input )
{
    PixelInputType output;

    //Change position vector to be 4 units for proper matrix calculations
    input.position.w = 1.0f;

    //Calculate position of vertex against world, view, and projection matrices
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    //Store texture coordinates for pixel shader
    output.tex = input.tex;

    //Calculate normal vector against world matrix only
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    //Normalize normal vector
    output.normal = normalize(output.normal);

    return output;
}


//--- Pixel Shader ---//
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    //Sample pixel color from texture using sampler at this texture coordinate location
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	//Set the default output color to the ambient light value for all pixels
	color = ambientColor;

    //Invert light direction for calculations
    lightDir = -lightDirection;

    //Calculate amount of light on this pixel
    lightIntensity = saturate(dot(input.normal, lightDir));

	if (lightIntensity > 0.0f)
	{
		//Determine final diffuse color based on diffuse color and amount of light intensity
		color += (diffuseColor * lightIntensity);
	}

	//Saturate final light color
	color = saturate(color);

    //Multiply texture pixel and final diffuse color to get fina lpixel color result
    color = color * textureColor;

    return color;

}