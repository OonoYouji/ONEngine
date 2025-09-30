struct ControlPoint {
    float3 position;
    float width;
};

struct Vertex {
    float4 position;
    float2 uv;
    float3 normal;
};

struct Params {
    uint totalSegments;
    uint totalVertices;
    uint totalSamples;
};

/// ----- buffer ----- ///
ConstantBuffer<Params> params : register(b0);
StructuredBuffer<ControlPoint> controlPoints : register(t0);
RWStructuredBuffer<Vertex> vertices : register(u0);
RWStructuredBuffer<uint> indices : register(u1);


/// ----- methods ----- ///
float3 CatmullRom(float3 p0, float3 p1, float3 p2, float3 p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5 * ((2.0 * p1) +
                  (-p0 + p2) * t +
                  (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t2 +
                  (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3);
}

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

    uint index = DTid.x;

    /// ----- 頂点の生成 ----- ///
    if(index < params.totalVertices) {
        /// ----- 左右の頂点を1スレッドで二回生成する ----- ///
        uint segmentIndex = DTid.x / 2;
        uint side = DTid.x % 2;
    
        float t = (DTid.x / 2) / float(params.totalSegments);
        
        /// ----- 制御点の補間 ----- ///
        float3 pos = CatmullRom(
            controlPoints[0].position,
            controlPoints[1].position,
            controlPoints[2].position,
            controlPoints[3].position,
            t
        );
    
        /// 幅方向へのベクトル計算
        float3 tangent = normalize(CatmullRom(
            controlPoints[0].position,
            controlPoints[1].position,
            controlPoints[2].position,
            controlPoints[3].position,
            t + 0.01) - pos);
    
        float3 up = float3(0, 1, 0);
        float3 right = normalize(cross(up, tangent));
    
        float width = lerp(controlPoints[segmentIndex + 1].width, controlPoints[segmentIndex + 2].width, t);
        pos += right * width * (side == 0 ? - 0.5 : 0.5);
    
        /// 頂点の書き込み
        vertices[DTid.x].position = float4(pos, 1.0);
        vertices[DTid.x].uv = float2(t, side);
        vertices[DTid.x].normal = up;
    }


    /// ----- インデックスの生成 ----- ///
    if(index < params.totalSamples) {
        uint i0 = 2 * index + 0;
        uint i1 = 2 * index + 1;
        uint i2 = 2 * (index + 1) + 0;
        uint i3 = 2 * (index + 1) + 1;

        uint outIdx = index * 6;
        indices[outIdx + 0] = i0;
        indices[outIdx + 1] = i1;
        indices[outIdx + 2] = i2;

        indices[outIdx + 3] = i1;
        indices[outIdx + 4] = i3;
        indices[outIdx + 5] = i2;
    }
}