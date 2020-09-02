#pragma once
#include "pch.h"
#include "Keyframe.h"

using namespace std;

enum class ModelType {
	UNINITIALIZED,
	CAMERA,
	GEOPRIMITIVE,
	CMO,
	LIGHT
};

class Object
{
private:
	int object_id;
	vector<Keyframe> keyframes;
	bool already_initialized;
	std::shared_ptr<DirectX::Model> file_model;
	std::shared_ptr<DirectX::GeometricPrimitive> primitive_model;
	ModelType model_type;
	int currentKeyframeIndex;

	void InitializeKeyframes();
	int FindKeyframeByTime(double time);
public:
	Object(int object_id) noexcept(false);
	~Object();
	void InitializeFromFile(ID3D11Device* device, wstring filename, IEffectFactory& fxFactory);
	void InitializeCone(ID3D11DeviceContext* deviceContext, float diameter = 1, float height = 1, size_t tessellation = 32);
	void InitializeCube(ID3D11DeviceContext* deviceContext, float size = 1);
	void InitializeDodecahedron(ID3D11DeviceContext* deviceContext, float size = 1);
	void InitializeGeoSphere(ID3D11DeviceContext* deviceContext, float diameter = 1, size_t tessellation = 3);
	void InitializeIcosahedron(ID3D11DeviceContext* deviceContext, float size = 1);
	void InitializeOctahedron(ID3D11DeviceContext* deviceContext, float size = 1);
	void InitializeSphere(ID3D11DeviceContext* deviceContext, float diameter = 1, size_t tessellation = 16);
	void InitializeTeapot(ID3D11DeviceContext* deviceContext, float size = 1, size_t tessellation = 8);
	void InitializeTetrahedron(ID3D11DeviceContext* deviceContext, float size = 1);
	void InitializeTorus(ID3D11DeviceContext* deviceContext, float diameter = 1, float thickness = 0.333f, size_t tessellation = 32);
	void InitializeCamera();
	void InitializeLight();
	void SetTexture();
	Vector3 GetLightDirection(double time);
	void Render(double time, ID3D11DeviceContext* deviceContext, BasicEffect* basicEffect, ID3D11InputLayout* inputLayout, CommonStates* states, Matrix view, Matrix projection, Vector4 lightDiffuseColor, Vector3 lightDirection);
};

