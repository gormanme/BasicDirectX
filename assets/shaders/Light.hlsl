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
	float specularPower;
	float4 specularColor;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
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
	float3 viewDirection : TEXCOORD1;
};


//--- Vertex Shader ---//
PixelInputType LightVertexShader( VertexInputType input )
{
    PixelInputType output;
	float4 worldPosition;

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

	//Calculate position of vertex in world
	worldPosition = mul(input.position, worldMatrix);

	//Determine the viewing direction based on position of camera and position of vertex in world
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	//Normalize viewing direction vector
	output.viewDirection = normalize(output.viewDirection);

    return output;
}


//--- Pixel Shader ---//
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
	float3 reflection;
	float4 specular;

    //Sample pixel color from texture using sampler at this texture coordinate location
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	//Set the default output color to the ambient light value for all pixels
	color = ambientColor;

	//Initialize specular color
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //Invert light direction for calculations
    lightDir = -lightDirection;

    //Calculate amount of light on this pixel
    lightIntensity = saturate(dot(input.normal, lightDir));

	if (lightIntensity > 0.0f)
	{
		//Determine final diffuse color based on diffuse color and amount of light intensity
		color += (diffuseColor * lightIntensity);

		//Saturate ambient and diffuse color
		color = saturate(color);

		//Calculate reflection vector based on light intensity, normal vector, and light direction
		reflection = normalize(2 * lightIntensity * input.normal - lightDir);

		//Determine the amount of specular light based on reflection vector, viewing direction, and specular power
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	}

    //Multiply texture pixel and final diffuse color to get final pixel color result
	//Can comment out this line to remove texture
    color = color * textureColor;

	//Add specular component last to output color. Has to be added last
	color = saturate(color + specular);

    return color;

}