struct Input {
    float4 pos : POSITION0;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 instancePos : INSTANCEPOS0;
};

cbuffer ModelData : register(b0) {
    float4x4 Model;
    bool isInstance;
};
cbuffer CameraData : register(b1) {
    float4x4 View;
    float4x4 Projection;
};

struct Output {
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

Output main(Input input) {
	Output output = (Output)0;
    
    
    if (isInstance)
    {
        input.pos += float4(input.instancePos, 0.0f);
    }

    output.pos = mul(input.pos, Model);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);
    output.normal = mul(input.normal, Model);
    output.uv = input.uv; 

	return output;
}