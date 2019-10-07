//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	return pos;
//}
//struct VSIn
//{
//	uint vertexId : SV_VertexID;
//};
//
//struct VSOut
//{
//	float4 pos : SV_Position;
//	float4 color : color;
//};

//float4 main(in uint vertexId : SV_VertexID) : SV_Position
//{
//	//VSOut output;
//	float4 pos;
//
//	if (vertexId == 0)
//		pos = float4(0.0, 1.0, 0.0, 1.0);
//	else if (vertexId == 2)
//		pos = float4(0.5, -0.5, 0.0, 1.0);
//	else if (vertexId == 1)
//		pos = float4(-0.5, -0.5, 0.0, 1.0);
//
//	//output.color = clamp(output.pos, 0, 1);
//	//color = float4(1.0, 1.0, 1.0, 1.0);
//	return pos;
//}


struct VSQuadOut {

	float4 position : SV_Position;

	//float2 texcoord: TexCoord;

};

VSQuadOut main(uint VertexID: SV_VertexID) {// ouputs a full screen quad with tex coords

	VSQuadOut Out;

	//Out.texcoord = float2((VertexID << 1) & 2, VertexID & 2);
	float4 pos;
	if (VertexID == 0)
		pos = float4(1.0, 0.0, 0.0, 1.0);
	else if (VertexID == 1)
		pos = float4(-1.0, 0.0, 0.0, 1.0);
	else if (VertexID == 2)
		pos = float4(0.0, -1.0, 0.0, 1.0);

	//Out.position = float4(float2((VertexID << 1) & 2, VertexID & 2) * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
	Out.position = float4(pos.xy /** float2(2.0f, -2.0f) + float2(-1.0f, 1.0f)*/, 0.0f, 1.0f);
	return Out;
}