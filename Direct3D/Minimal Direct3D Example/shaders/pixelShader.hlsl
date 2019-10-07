//float4 main() : SV_TARGET
//{
//	return float4(0.8, 0.2, 0.7, 1.0);
//}


//
//struct PSIn
//{
//	float4 pos : SV_Position;
//	linear float4 color : color;
//};
//
//struct PSOut
//{
//	float4 color : SV_Target;
//};
//
//
//float4 main(PSIn input) : SV_TARGET
//{
//	PSOut output;
//
//	output.color = input.color;
//
//	return output.color;
//}

float4 main(in float4 Pos : SV_Position) : SV_Target
{
	float4 color;
	return color = float4(0.5, 0.0, 0.5, 1.0);
}