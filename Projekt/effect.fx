//================================================================================
// Structs
//================================================================================
struct struct_VS
{
    float2 Tex		: TEXCOORD;    // texture coordinates
    float4 Pos		: SV_POSITION;    // vertex screen coordinates
	float3 posW		: POSITION;
    float3 normalW	: NORMAL;
};

struct struct_vsSimple
{
    float4 posH		: SV_POSITION;    // vertex screen coordinates
	float3 posW		: POSITION;
	float4 color	: COLOR;
};

struct struct_VSBlendmap
{
    float2 tiledUV		: TEXCOORD0;
	float2 stretchedUV	: TEXCOORD1;
    float4 Pos			: SV_POSITION;
	float3 posW			: POSITION;
    float3 normalW		: NORMAL;
};

struct struct_VSParticle
{
    float3 centerW : POSITION;
	float2 sizeW   : SIZE;
	float4 fade    : COLOR;
};

struct struct_GSParticle
{
    float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float2 texC    : TEXCOORD;
    uint primID    : SV_PrimitiveID;
	float4 fade    : COLOR;
};

struct Light
{
	float3 pos;
	float3 dir;
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 att;
	float  spotPower;
	float  range;
};

struct SurfaceInfo
{
    float4 diffuse;
    float4 spec;
};

//================================================================================
// Constant Buffers
//================================================================================
cbuffer perFrame
{
    float4x4 viewProj;
	float3 cameraPos;
	Light light;

	SurfaceInfo surface;
}

cbuffer perObject
{
	float4x4 world;
	float4x4 worldGroup;
	float texScale = 256; //should be set by hieghtmap object via cb or fx-var
}

//================================================================================
// Globals
//================================================================================
Texture2D texDefault;
Texture2D texBlendmap;
Texture2D tex0;
Texture2D tex1;
Texture2D tex2;
Texture2D tex3;
Texture2D tex4;

// Nonnumeric values cannot be added to a cbuffer.
Texture2DArray gDiffuseMapArray;

//================================================================================
// Shader Functions
//================================================================================
//Lights
float3 ParallelLight(SurfaceInfo surface, Light light, float3 eyePos, float3 normal, float3 pos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -light.dir;
	
	// Add the ambient term.
	litColor += surface.diffuse * light.ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, normal);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(surface.spec.a, 1.0f);
		float3 toEye     = normalize(eyePos - pos);
		float3 R         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
					
		// diffuse and specular terms
		litColor += diffuseFactor * surface.diffuse * light.diffuse;
		litColor += specFactor * surface.spec * light.spec;
	}
	
	return litColor;
}

float3 PointLight(SurfaceInfo surface, Light light, float3 eyePos, float3 normal, float3 pos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	
	// The vector from the surface to the light.
	float3 lightVec = light.pos - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	if( d > light.range )
		return float3(0.0f, 0.0f, 0.0f);
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Add the ambient light term.
	litColor += surface.diffuse * light.ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, normal);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(surface.spec.a, 1.0f);
		float3 toEye     = normalize(eyePos - pos);
		float3 R         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
	
		// diffuse and specular terms
		litColor += diffuseFactor * surface.diffuse * light.diffuse;
		litColor += specFactor * surface.spec * light.spec;
	}
	
	// attenuate
	return litColor / dot(light.att, float3(1.0f, d, d*d));
}

float3 Spotlight(SurfaceInfo surface, Light light, float3 eyePos, float3 normal, float3 pos)
{
	float3 litColor = PointLight(surface, light, eyePos, normal, pos);
	
	// The vector from the surface to the light.
	float3 lightVec = normalize(light.pos - pos);
	
	float s = pow(max(dot(-lightVec, light.dir), 0.0f), light.spotPower);
	
	// Scale color by spotlight factor.
	return litColor*s;
}

//================================================================================
// Vertex Shaders
//================================================================================
struct_vsSimple vsSimple(float3 pos : POSITION, float4 col : COLOR)
{
	struct_vsSimple output;

	//Homo clip space transform
	float4x4 final = mul(world,viewProj);
    output.posH = mul(float4(pos, 1.0f), final);    // transform the vertex from 3D to 2D

	//world space transform
	output.posW = mul(float4(pos, 1.0f), world);

	output.color = col;

	return output;
}

struct_VS VS(float3 Norm : NORMAL, float2 Tex : TEXCOORD, float3 Pos : POSITION)
{
    struct_VS Output;
	 
	 float4x4 worldFinal = mul(world, worldGroup); //add group transform

	//Homo clip space transform
	float4x4 final = mul(worldFinal,viewProj);
    Output.Pos = mul(float4(Pos, 1.0f), final);    // transform the vertex from 3D to 2D

	//world space transform
	Output.posW = mul(float4(Pos, 1.0f), world);
	Output.normalW = mul(float4(Norm, 0.0f), world);

	//passthrough
    Output.Tex = Tex;

    return Output;    // send the modified vertex data to the Rasterizer Stage
}

struct_VSBlendmap VSBlendmap(float3 Norm : NORMAL, float2 UV : TEXCOORD, float3 Pos : POSITION)
{
    struct_VSBlendmap Output;

	//float4x4 worldFinal = mul(worldGroup, world); //add group transform

	//Homo clip space transform
	float4x4 final = mul(world,viewProj);
    Output.Pos = mul(float4(Pos, 1.0f), final);   // transform the vertex from 3D to 2D

	//world space transform
	Output.posW = mul(float4(Pos, 1.0f), world);
	Output.normalW = mul(float4(Norm, 0.0f), world);

	//passthrough
    Output.tiledUV = texScale*UV;   // stretch texture
	Output.stretchedUV = UV;		// set the texture coordinates, unmodified

    return Output;    // send the modified vertex data to the Rasterizer Stage
}

struct_VSParticle vsParticle(struct_VSParticle vIn)
{
    struct_VSParticle vOut;
	
	// Just pass data into geometry shader stage.
	vOut.centerW  = vIn.centerW;
	vOut.sizeW    = vIn.sizeW;
	vOut.fade	  = vIn.fade;

	return vOut;
}

//================================================================================
// Geometry Shaders
//================================================================================
[maxvertexcount(4)]
void gsParticle(point struct_VSParticle gIn[1], 
        uint primID : SV_PrimitiveID, 
        inout TriangleStream<struct_GSParticle> triStream)
{	

	// Compute 4 triangle strip vertices (quad) in local space.
	// The quad faces down the +z axis in local space.
	float halfWidth  = 0.5f*gIn[0].sizeW.x;
	float halfHeight = 0.5f*gIn[0].sizeW.y;
	
	float4 v[4];
	v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
	v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
	v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
	v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);

	// Compute texture coordinates to stretch texture over quad.
	float2 texC[4];
	texC[0] = float2(0.0f, 1.0f);
	texC[1] = float2(1.0f, 1.0f);
	texC[2] = float2(0.0f, 0.0f);
	texC[3] = float2(1.0f, 0.0f);

	// Compute world matrix so that billboard is aligned with
	// the y-axis and faces the camera.
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = cameraPos - gIn[0].centerW;
	look = normalize(-look);
	float3 right = cross(up, look);
	right = normalize(right);
	up = cross(right, look);
	up = normalize(up);
		
	float4x4 W;
	W[0] = float4(right,          0.0f);
	W[1] = float4(up,             0.0f);
	W[2] = float4(look,           0.0f);
	W[3] = float4(gIn[0].centerW, 1.0f);

	float4x4 WVP = mul(W,viewProj);
	
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	struct_GSParticle gOut;
	[unroll]
	for(int i = 0; i < 4; ++i)
	{
		gOut.posH     = mul(v[i], WVP);
		gOut.posW     = mul(v[i], W);
		gOut.normalW  = look;
		gOut.texC     = texC[i];
		gOut.primID   = primID;
		gOut.fade     = gIn[0].fade;
		
		triStream.Append(gOut);
	}
}

//================================================================================
// SamplerStates
//================================================================================
SamplerState ss
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

//================================================================================
// Pixel Shaders
//================================================================================
float4 psSimple(struct_vsSimple input) : SV_TARGET
{
    float4 finalCol = input.color;
    return finalCol;    // return the resulting color for that pixel
}

float4 PS(struct_VS input) : SV_TARGET
{
	float4 lightCol = {PointLight(surface, light, cameraPos, input.normalW, input.posW),1.0f};
    float4 finalCol = texDefault.Sample(ss, input.Tex);    // sample the texture and multiply by the light
	//finalCol *= lightCol;
    return finalCol;    // return the resulting color for that pixel
}

float4 PSBlendmap(struct_VSBlendmap input) : SV_TARGET
{
	float4 lightCol = {PointLight(surface, light, cameraPos, input.normalW, input.posW),1.0f};
	float4 blendmap = texBlendmap.Sample(ss, input.stretchedUV);

	float4 color0 = tex0.Sample(ss, input.tiledUV);
	float4 color1 = tex1.Sample(ss, input.tiledUV);
	float4 color2 = tex2.Sample(ss, input.tiledUV);
	float4 color3 = tex3.Sample(ss, input.tiledUV);
	float4 color4 = tex4.Sample(ss, input.tiledUV);

	float4 finalColor;
	finalColor = color0;
	finalColor = lerp(finalColor, color1, blendmap.r);
	finalColor = lerp(finalColor, color2, blendmap.g);
	finalColor = lerp(finalColor, color3, blendmap.b);
	finalColor = lerp(finalColor, color4, 1-blendmap.a); //Without 1- this totally fucks your brain
	//finalColor *= lightCol;

	//finalColor *= Col;

	return finalColor;
}

float4 psParticle(struct_GSParticle pIn) : SV_TARGET
{
    float4 Col = texDefault.Sample(ss, pIn.texC); 
	Col = Col - pIn.fade;
    return Col;    // return the resulting color for that pixel
}

//================================================================================
// Techniques
//================================================================================
technique10 Technique_0 //for regular Objects
{
    pass Pass_0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}

technique10 Technique_1 //for particles
{
    pass Pass_0
    {
        SetVertexShader(CompileShader(	vs_4_0, vsParticle()));
        SetGeometryShader(CompileShader(gs_4_0, gsParticle() ) );
        SetPixelShader(CompileShader(	ps_4_0, psParticle()));
    }
}

technique10 Technique_2 //for heightmap with blendmap
{
    pass Pass_0
    {
        SetVertexShader(CompileShader(vs_4_0, VSBlendmap()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PSBlendmap()));
    }
}

technique10 Technique_3 //for Nodes
{
    pass Pass_0
    {
        SetVertexShader(CompileShader(vs_4_0, vsSimple()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, psSimple()));
    }
}