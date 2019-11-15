struct VSQuadOut {
	float4 position : SV_Position;
};


VSQuadOut main(uint VertexID: SV_VertexID) {

	VSQuadOut Out;

	//Out.texcoord = float2((VertexID << 1) & 2, VertexID & 2);
	float4 pos;
	if (VertexID == 0)
		pos = float4(1.0, 0.0, 0.0, 1.0);
	else if (VertexID == 1)
		pos = float4(0.0, 1.0, 0.0, 1.0);
	else if (VertexID == 2)
		pos = float4(-1.0, 0.0, 0.0, 1.0);
	else if (VertexID == 3)
		pos = float4(-1.0, 0.0, 0.0, 1.0);
	else if (VertexID == 4)
		pos = float4(0.0, -1.0, 0.0, 1.0);
	else if (VertexID == 5)
		pos = float4(1.0, 0.0, 0.0, 1.0);

	//Out.position = float4(float2((VertexID << 1) & 2, VertexID & 2) * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
	Out.position = float4(pos.xy /** float2(2.0f, -2.0f) + float2(-1.0f, 1.0f)*/, 0.0f, 1.0f);
	return Out;
}