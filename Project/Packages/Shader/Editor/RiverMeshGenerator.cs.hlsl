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
    uint samplePerSegment;
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



[numthreads(16, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    uint index = DTid.x;

    if(index < params.totalVertices) {
        uint sampleNum = index / 2;
        uint side   = index % 2;

        float tPerSegment = 1.0 / float(params.samplePerSegment);
        float localT = (sampleNum % params.samplePerSegment) * tPerSegment;
        uint segIndex = sampleNum / params.samplePerSegment;

        float3 p0 = controlPoints[segIndex + 0].position;
        float3 p1 = controlPoints[segIndex + 1].position;
        float3 p2 = controlPoints[segIndex + 2].position;
        float3 p3 = controlPoints[segIndex + 3].position;

        float3 pos = CatmullRom(p0,p1,p2,p3,localT);

        // tangent 安全化
        float eps = 0.01;
        float tPrev = max(localT - eps, 0.0);
        float tNext = min(localT + eps, 1.0);
        float3 posPrev = CatmullRom(p0,p1,p2,p3,tPrev);
        float3 posNext = CatmullRom(p0,p1,p2,p3,tNext);
        float3 tangent = normalize(posNext - posPrev);

        float3 up = float3(0,1,0);
        float3 right = cross(up,tangent);
        if(length(right)<1e-6) {
            up = float3(1,0,0);
            right = cross(up,tangent);
        }
        right = normalize(right);

        float width = lerp(controlPoints[segIndex+1].width, controlPoints[segIndex+2].width, localT);
        pos += right * width * (side==0 ? -0.5 : 0.5);

        vertices[index].position = float4(pos,1.0);
        vertices[index].uv       = float2((float)sampleNum/params.totalSegments, (float)side);
        vertices[index].normal   = up;
    }

    // -------- index生成 --------
    uint numPairs = params.totalVertices / 2;
    if (index < numPairs - 1) {
        uint i0 = 2 * index + 0;
        uint i1 = 2 * index + 1;
        uint i2 = 2 * (index + 1) + 0;
        uint i3 = 2 * (index + 1) + 1;

        uint outIdx = index * 6;
        indices[outIdx + 0] = i0;
        indices[outIdx + 1] = i2;
        indices[outIdx + 2] = i1;

        indices[outIdx + 3] = i1;
        indices[outIdx + 4] = i2;
        indices[outIdx + 5] = i3;
    }

}