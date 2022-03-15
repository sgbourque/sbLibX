#error "FWIW, this is not made to be compiled : this is only a code-brainstorming session"

RWTexture<float>  input_1R;   // mono channel
RWTexture<float>  input_2L;   // mono channel
RWTexture<float2> input_3_4;  // stereo track or bi-channel
RWTexture<float4> input_5_6_7_8; // quad-channel

RWTexture<float4> output_1_2_3_4; // quad channel
RWTexture<float4> output_5_6_7_8; // quad channel


// 64 buffer size at 44100Hz corresponds to ~1.45m.
// If ASIO buffer smaller than 64 are to be supported, this size could be reduced.
// There is currently no benefit of letting BUFFER_SIZE bigger than 64 on current architectures.
// If required, number of groups will have to be adjusted to cover all buffers, aligned up to 64.
#define BUFFER_SIZE		64 
[numthread( BUFFER_SIZE, 1, 1 )]
void test_thru( uint sample_id : SV_GroupThreadID, uint subbuffer_id : SV_DispatchThreadID, uint sample_offset: SV_GroupIndex )
{
	output_1_2_3_4[sample_offset] = float4( input_1R[sample_offset], input_1R[sample_offset], input_2L[sample_offset], input_3_4[sample_offset].xy );
	output_5_6_7_8[sample_offset] = input_5_6_7_8[sample_offset];
}
