Texture2D tex : register(t0);
Texture2D shadowTex : register(t1);
SamplerState samplerState : register(s0);

cbuffer LightData : register(b0)
{
    float4 LightPosition;
    float3 Direction;
    float4 Ambiant;
    float4 Diffuse;
    float4x4 LightView;
    float4x4 LightProjection;
    float pad;
};

struct Input {
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    bool underWater : HEIGHT;
};



float4 main(Input input) : SV_TARGET {

    // normalize normal
    input.normal = normalize(input.normal);

    // sample texture
    float4 color = tex.Sample(samplerState, input.uv / 16);
    
    float4 finalColor;

    // Apply ambiant color
    finalColor = color * Ambiant;
    
    // Apply Diffuse color
    finalColor += saturate(dot(float4(Direction, 0), input.normal) * Diffuse * color);
    
    // Underwater pass
    if (input.underWater)
    {
        finalColor *= float4(0.1, 0.1, 0.5, 1.0);
    }
    
    // Clipping
    finalColor.a = color.a;
    clip(finalColor.a - 0.01f);
    
    return finalColor;
    
}

