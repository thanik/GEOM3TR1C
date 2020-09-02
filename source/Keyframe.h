#include "pch.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

enum class KeyframeType {
	LINEAR,
	EASEIN,
	EASEOUT,
	EASEINOUT
};

struct Keyframe {
	double start_time;
	double end_time;
	Vector3 start_position;
	Vector3 end_position;
	Vector3 start_rotation;
	Vector3 end_rotation;
	float start_scale;
	float end_scale;
	bool wireframe;
	KeyframeType key_type;

	Keyframe(double start_time_in, double end_time_in, KeyframeType key_type_in, Vector3 start_position_in, Vector3 end_position_in, Vector3 start_rotation_in, Vector3 end_rotation_in, float start_scale_in, float end_scale_in, bool wireframe_in) :
		start_time(start_time_in), end_time(end_time_in), key_type(key_type_in), start_position(start_position_in), end_position(end_position_in), start_rotation(start_rotation_in), end_rotation(end_rotation_in), start_scale(start_scale_in), end_scale(end_scale_in), wireframe(wireframe_in)
	{ }
};