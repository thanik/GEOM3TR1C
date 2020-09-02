#include "pch.h"
#include "Object.h"
#include "TextFileReader.h"

Object::Object(int initializing_object_id)
{
	object_id = initializing_object_id;
	currentKeyframeIndex = 0;
	model_type = ModelType::UNINITIALIZED;
	already_initialized = false;
}

Object::~Object()
{

}

void Object::InitializeCamera()
{
	model_type = ModelType::CAMERA;
	already_initialized = true;
	InitializeKeyframes();
}

void Object::InitializeFromFile(ID3D11Device* device, wstring filename, IEffectFactory& fxFactory)
{
	file_model = Model::CreateFromCMO(device, filename.c_str(), fxFactory);
	already_initialized = true;
	model_type = ModelType::CMO;
	InitializeKeyframes();
}

void Object::InitializeCone(ID3D11DeviceContext* deviceContext, float diameter, float height, size_t tessellation)
{
	primitive_model = GeometricPrimitive::CreateCone(deviceContext, diameter, height, tessellation, true);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeCube(ID3D11DeviceContext* deviceContext, float size)
{
	primitive_model = GeometricPrimitive::CreateCube(deviceContext, size);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeDodecahedron(ID3D11DeviceContext* deviceContext, float size)
{
	primitive_model = GeometricPrimitive::CreateDodecahedron(deviceContext, size);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeGeoSphere(ID3D11DeviceContext* deviceContext, float diameter, size_t tessellation)
{
	primitive_model = GeometricPrimitive::CreateGeoSphere(deviceContext, diameter, tessellation);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeIcosahedron(ID3D11DeviceContext* deviceContext, float size)
{
	primitive_model = GeometricPrimitive::CreateIcosahedron(deviceContext, size);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeOctahedron(ID3D11DeviceContext* deviceContext, float size)
{
	primitive_model = GeometricPrimitive::CreateOctahedron(deviceContext, size);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeSphere(ID3D11DeviceContext* deviceContext, float diameter, size_t tessellation)
{
	primitive_model = GeometricPrimitive::CreateSphere(deviceContext, diameter, tessellation);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeTeapot(ID3D11DeviceContext* deviceContext, float size, size_t tessellation)
{
	primitive_model = GeometricPrimitive::CreateTeapot(deviceContext, size, tessellation);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeTetrahedron(ID3D11DeviceContext* deviceContext, float size)
{
	primitive_model = GeometricPrimitive::CreateTetrahedron(deviceContext, size);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeTorus(ID3D11DeviceContext* deviceContext, float diameter, float thickness, size_t tessellation)
{
	primitive_model = GeometricPrimitive::CreateTorus(deviceContext, diameter, thickness, tessellation);
	already_initialized = true;
	model_type = ModelType::GEOPRIMITIVE;
	InitializeKeyframes();
}

void Object::InitializeLight()
{
	model_type = ModelType::LIGHT;
	already_initialized = true;
	InitializeKeyframes();
}

void Object::SetTexture()
{

}

Vector3 Object::GetLightDirection(double time)
{
	Vector3 calculated_position = Vector3();
	bool ended = false;
	if (already_initialized && keyframes.size() > 0)
	{
		currentKeyframeIndex = FindKeyframeByTime(time);
		if (currentKeyframeIndex == -1)
		{
			currentKeyframeIndex = keyframes.size() - 1;
			ended = true;
		}
		Keyframe currentKeyframe = keyframes[currentKeyframeIndex];
		float keyframe_percentage = (time - currentKeyframe.start_time) / (currentKeyframe.end_time - currentKeyframe.start_time);

		if (ended)
		{
			keyframe_percentage = 1.0f;
		}

		if (currentKeyframe.key_type == KeyframeType::EASEIN)
		{
			keyframe_percentage = keyframe_percentage * keyframe_percentage;
		}
		else if (currentKeyframe.key_type == KeyframeType::EASEOUT)
		{
			keyframe_percentage = 1 - keyframe_percentage;
			keyframe_percentage = keyframe_percentage * keyframe_percentage;
			keyframe_percentage = 1 - keyframe_percentage;
		}
		else if (currentKeyframe.key_type == KeyframeType::EASEINOUT)
		{
			float flipped = 1 - keyframe_percentage;
			flipped = flipped * flipped;
			flipped = 1 - flipped;
			float square = keyframe_percentage * keyframe_percentage;
			keyframe_percentage = square + (flipped - square) * keyframe_percentage;
		}
		calculated_position = Vector3::Lerp(currentKeyframe.start_position, currentKeyframe.end_position, keyframe_percentage);
	}
	return calculated_position;
}

void Object::Render(double time, ID3D11DeviceContext* deviceContext, BasicEffect* basicEffect, ID3D11InputLayout* inputLayout, CommonStates* states, Matrix view, Matrix projection, Vector4 lightDiffuseColor, Vector3 lightDirection)
{
	if (already_initialized)
	{
		Vector3 calculated_position = Vector3();
		Vector3 calculated_rotation = Vector3();
		float calculated_scale = 1.f;
		bool wireframe = false;
		bool ended = false;
		if (keyframes.size() > 0)
		{
			currentKeyframeIndex = FindKeyframeByTime(time);
			if (currentKeyframeIndex == -1)
			{
				currentKeyframeIndex = keyframes.size() - 1;
				ended = true;
			}
			Keyframe currentKeyframe = keyframes[currentKeyframeIndex];
			float keyframe_percentage = (time - currentKeyframe.start_time) / (currentKeyframe.end_time - currentKeyframe.start_time);
			if (ended)
			{
				keyframe_percentage = 1.0f;
			}

			if (currentKeyframe.key_type == KeyframeType::EASEIN)
			{
				keyframe_percentage = keyframe_percentage * keyframe_percentage;
			}
			else if (currentKeyframe.key_type == KeyframeType::EASEOUT)
			{
				keyframe_percentage = 1 - keyframe_percentage;
				keyframe_percentage = keyframe_percentage * keyframe_percentage;
				keyframe_percentage = 1 - keyframe_percentage;
			}
			else if (currentKeyframe.key_type == KeyframeType::EASEINOUT)
			{
				float flipped = 1 - keyframe_percentage;
				flipped = flipped * flipped;
				flipped = 1 - flipped;
				float square = keyframe_percentage * keyframe_percentage;
				keyframe_percentage = square + (flipped - square) * keyframe_percentage;
			}

			calculated_position = Vector3::Lerp(currentKeyframe.start_position, currentKeyframe.end_position, keyframe_percentage);
			calculated_rotation = Vector3::Lerp(currentKeyframe.start_rotation, currentKeyframe.end_rotation, keyframe_percentage);
			calculated_scale = currentKeyframe.start_scale + (currentKeyframe.end_scale - currentKeyframe.start_scale) * keyframe_percentage;
			wireframe = currentKeyframe.wireframe;
		}

		DirectX::SimpleMath::Matrix m_world;
		m_world = SimpleMath::Matrix::Identity;
		SimpleMath::Matrix newRotation = SimpleMath::Matrix::CreateFromYawPitchRoll(calculated_rotation.y * XM_PI / 180, calculated_rotation.x * XM_PI / 180, calculated_rotation.z * XM_PI / 180);
		SimpleMath::Matrix newScale = SimpleMath::Matrix::CreateScale(calculated_scale);
		SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(calculated_position);
		m_world = m_world * newScale * newRotation * newPosition;
		basicEffect->SetWorld(m_world);
		if (model_type == ModelType::GEOPRIMITIVE)
		{
			primitive_model->Draw(basicEffect, inputLayout, false, wireframe);
		}
		else if (model_type == ModelType::CMO)
		{
			file_model->UpdateEffects([&](IEffect* effect)
			{
				auto lights = dynamic_cast<IEffectLights*>(effect);
				if (lights)
				{
					lights->SetLightingEnabled(true);
					lights->SetPerPixelLighting(true);
					lights->SetLightEnabled(0, true);
					lights->SetLightDiffuseColor(0, lightDiffuseColor);
					lights->SetLightDirection(0, lightDirection);
					lights->SetLightEnabled(1, false);
					lights->SetLightEnabled(2, false);
				}
			});
			file_model->Draw(deviceContext, *states, m_world, view, projection, wireframe);
		}
	}
}

void Object::InitializeKeyframes()
{
	std::vector<std::vector<std::wstring>> readData;
	wstring filename = L"Assets/Keyframes/" + to_wstring(object_id) + L".csv";
	readData = readCSVFile(filename.c_str());
	for (vector<vector<wstring>>::iterator it = readData.begin(); it != readData.end(); ++it)
	{
		vector<wstring> current_item = *it;
		if (current_item.size() == 18)
		{
			KeyframeType key_type = KeyframeType::LINEAR;
			if (current_item[2].compare(L"LINEAR") == 0)
			{
				key_type = KeyframeType::LINEAR;
			}
			else if (current_item[2].compare(L"EASEIN") == 0)
			{
				key_type = KeyframeType::EASEIN;
			}
			else if (current_item[2].compare(L"EASEOUT") == 0)
			{
				key_type = KeyframeType::EASEOUT;
			}
			else if (current_item[2].compare(L"EASEINOUT") == 0)
			{
				key_type = KeyframeType::EASEINOUT;
			}

			keyframes.push_back(Keyframe(stod(current_item[0]), stod(current_item[1]), key_type, Vector3(stof(current_item[3]), stof(current_item[4]), stof(current_item[5])), Vector3(stof(current_item[6]), stof(current_item[7]), stof(current_item[8])), Vector3(stof(current_item[9]), stof(current_item[10]), stof(current_item[11])), Vector3(stof(current_item[12]), stof(current_item[13]), stof(current_item[14])), stof(current_item[15]), stof(current_item[16]), (stoi(current_item[17]) == 1 ? true : false)));
		}
	}
	wstring debugText = L"ObjectID: " + to_wstring(object_id) + L" has " + to_wstring(keyframes.size()) + L" keyframes.\n";
	OutputDebugStringW(debugText.c_str());
}

int Object::FindKeyframeByTime(double time)
{
	for (vector<Keyframe>::iterator it = keyframes.begin(); it != keyframes.end(); ++it)
	{
		Keyframe current_item = *it;
		if (current_item.start_time <= time && time <= current_item.end_time)
		{
			return it - keyframes.begin();
		}
	}
	return -1;
}
