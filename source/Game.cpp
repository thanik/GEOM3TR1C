//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "TextFileReader.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
}

float lightOffset = 0.f;

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	debug_build = false;
	#ifdef _DEBUG
	debug_build = true;
	#endif

	bpm = 148.f;
	beat_offset = 0.265f;
	seconds_per_beat = 60.f / bpm;
	m_gameTime = 0.f;
	last_beat_update = -1;
	m_isPlaying = false;
	m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();

	std::vector<std::vector<std::wstring>> readData;
	readData = readCSVFile(L"Assets/ObjectList.csv");

	for (vector<vector<wstring>>::iterator it = readData.begin(); it != readData.end(); ++it)
	{
		vector<wstring> current_item = *it;
		if (current_item.size() >= 2)
		{
			if (current_item[1].compare(L"CONE") == 0 && current_item.size() >= 5)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeCone(context, stof(current_item[2]), stof(current_item[3]), stoi(current_item[4]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"CUBE") == 0 && current_item.size() >= 3)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeCube(context, stof(current_item[2]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"DODECAHEDRON") == 0 && current_item.size() >= 3)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeDodecahedron(context, stof(current_item[2]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"GEOSPHERE") == 0 && current_item.size() >= 4)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeGeoSphere(context, stof(current_item[2]), stoi(current_item[3]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"ICOSAHEDRON") == 0 && current_item.size() >= 3)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeIcosahedron(context, stof(current_item[2]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"OCTAHEDRON") == 0 && current_item.size() >= 3)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeOctahedron(context, stof(current_item[2]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"SPHERE") == 0 && current_item.size() >= 4)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeSphere(context, stof(current_item[2]), stoi(current_item[3]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"TEAPOT") == 0 && current_item.size() >= 4)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeTeapot(context, stof(current_item[2]), stoi(current_item[3]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"TETRAHEDRON") == 0 && current_item.size() >= 3)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeTetrahedron(context, stof(current_item[2]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"TORUS") == 0 && current_item.size() >= 5)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeTorus(context, stof(current_item[2]), stof(current_item[3]));
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"FILE") == 0 && current_item.size() >= 3)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeFromFile(device, current_item[2], *m_fxFactory);
				objects.emplace_back(new_object);
			}
			else if (current_item[1].compare(L"CAMERA") == 0 && current_item.size() >= 2)
			{
				Object new_object(stoi(current_item[0]));
				new_object.InitializeCamera();
				objects.emplace_back(new_object);
			}
		}
		wstring debugText = L"Obj: " + current_item[0] + L"->" + to_wstring(current_item.size()) + L":" + current_item[1] + L"\n";
		OutputDebugStringW(debugText.c_str());
	}

	// setup light
	light = std::make_unique<Object>(-1);
	light.get()->InitializeLight();

	//setup light
	//m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	//m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light.setPosition(2.0f, 1.0f, 1.0f);
	//m_Light.setDirection(1.0f, -1.0f, 0.0f);
	lightDirection = Vector3(0.f, 0.0f, 0.0f);

	//setup camera
	m_Camera.setPosition(Vector3(0.0f, 0.0f, 0.0f));
	m_Camera.setRotation(Vector3(90.0f, 0.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 

    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);
	m_audio = std::make_unique<SoundEffect>(m_audEngine.get(), L"Assets/Audio/audio.wav");
	m_audioIns = m_audio->CreateInstance();
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game

    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	if (m_isPlaying)
	{
		m_gameTime += timer.GetElapsedSeconds();
		int beat = (m_gameTime - beat_offset) / seconds_per_beat;
		if (beat != last_beat_update)
		{
			last_beat_update = beat;
			last_beat_time = m_gameTime;
		}

		if (m_gameTime <= last_beat_time + seconds_per_beat)
		{
			float lerp_percent = (m_gameTime - last_beat_time) / seconds_per_beat;
			lightDiffuseColor = Vector4::Lerp(Vector4(0.65f, 1.f, 1.f, 1.f), Vector4(0.35f, 0.35f, 0.35f, 1.f), lerp_percent);
		}
	}
	else
	{
		auto m_outputSize = m_deviceResources->GetOutputSize();
		UINT backBufferWidth = std::max<UINT>(m_outputSize.right - m_outputSize.left, 1);
		UINT backBufferHeight = std::max<UINT>(m_outputSize.bottom - m_outputSize.top, 1);

		m_screenPos.x = backBufferWidth / 2.f;
		m_screenPos.y = backBufferHeight / 2.f;
		m_audioIns->Stop();
	}

	if (m_gameInputCommands.space)
	{
		m_isPlaying = !m_isPlaying;
		if (m_gameTime < 0.01f)
		{
			m_audioIns->Play();
			
		}
	}

	if (m_gameInputCommands.reload && debug_build)
	{

	}
	float deltaTime = float(timer.GetElapsedSeconds());
	if (m_gameInputCommands.rotLeft && debug_build)
	{
		Vector3 rotation = m_Camera.getRotation();
		rotation.y = rotation.y -= m_Camera.getRotationSpeed() * deltaTime;
		m_Camera.setRotation(rotation);
	}
	if (m_gameInputCommands.rotRight && debug_build)
	{
		Vector3 rotation = m_Camera.getRotation();
		rotation.y = rotation.y += m_Camera.getRotationSpeed() * deltaTime;
		m_Camera.setRotation(rotation);
	}
	if (m_gameInputCommands.rotUp && debug_build)
	{
		Vector3 rotation = m_Camera.getRotation();
		rotation.x = rotation.x -= m_Camera.getRotationSpeed() * deltaTime;
		m_Camera.setRotation(rotation);
	}
	if (m_gameInputCommands.rotDown && debug_build)
	{
		Vector3 rotation = m_Camera.getRotation();
		rotation.x = rotation.x += m_Camera.getRotationSpeed() * deltaTime;
		m_Camera.setRotation(rotation);
	}

	if (m_gameInputCommands.lightUp && debug_build)
	{
		lightDirection.x -= deltaTime;
	}
	if (m_gameInputCommands.lightDown && debug_build)
	{
		lightDirection.x += deltaTime;
	}

	if (m_gameInputCommands.lightLeft && debug_build)
	{
		lightDirection.z -= deltaTime;
	}
	if (m_gameInputCommands.lightRight && debug_build)
	{
		lightDirection.z += deltaTime;
	}

	if (m_gameInputCommands.forward && debug_build)
	{
		Vector3 position = m_Camera.getPosition(); //get the position
		position += (m_Camera.getForward() * m_Camera.getMoveSpeed() * deltaTime); //add the forward vector
		m_Camera.setPosition(position);
	}
	if (m_gameInputCommands.back && debug_build)
	{
		Vector3 position = m_Camera.getPosition(); //get the position
		position -= (m_Camera.getForward() * m_Camera.getMoveSpeed() * deltaTime); //add the forward vector
		m_Camera.setPosition(position);
	}
	if (m_gameInputCommands.goLeft && debug_build)
	{
		Vector3 position = m_Camera.getPosition(); //get the position
		Vector3 rightVector;
		m_Camera.getForward().Cross(DirectX::SimpleMath::Vector3::UnitY, rightVector);
		position -= (rightVector * m_Camera.getMoveSpeed() * deltaTime); //add the forward vector
		m_Camera.setPosition(position);
	}
	if (m_gameInputCommands.goRight && debug_build)
	{
		Vector3 position = m_Camera.getPosition(); //get the position
		Vector3 rightVector;
		m_Camera.getForward().Cross(DirectX::SimpleMath::Vector3::UnitY, rightVector);
		position += (rightVector * m_Camera.getMoveSpeed() * deltaTime); //add the forward vector
		m_Camera.setPosition(position);
	}
	if (m_gameInputCommands.fastForward && debug_build)
	{
		m_gameTime += 5;
	}

	if (m_gameInputCommands.fastBackward && debug_build)
	{
		if (m_gameTime - 5 <= 0)
		{
			m_gameTime = 0.f;
		}
		else
		{
			m_gameTime -= 5;
		}
	}



    if (m_input.Quit() || m_gameTime > 80.5f)
    {
        ExitGame();
    }
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

	std::wstring ws1 = std::to_wstring(m_timer.GetFramesPerSecond()) + L" FPS:";

	ws1 += std::to_wstring(m_gameTime);
	if (debug_build)
	{
		ws1 += L"\nCam:" + std::to_wstring(m_Camera.getPosition().x) + L"," + std::to_wstring(m_Camera.getPosition().y) + L"," + std::to_wstring(m_Camera.getPosition().z);
		ws1 += L"\nCamRot:" + std::to_wstring(m_Camera.getRotation().x) + L"," + std::to_wstring(m_Camera.getRotation().y) + L"," + std::to_wstring(m_Camera.getRotation().z);
		ws1 += L"\nObjs:" + std::to_wstring(objects.size());
		ws1 += L"\nLight:" + std::to_wstring(lightDirection.x) + L"," + std::to_wstring(lightDirection.y) + L"," + std::to_wstring(lightDirection.z);
	}
	const wchar_t* fps = ws1.c_str();

    // Draw sprite
    m_deviceResources->PIXBeginEvent(L"Draw sprite");
    m_sprites->Begin();
    m_font->DrawString(m_sprites.get(), fps, XMFLOAT2(10, 10), Colors::White);
    m_sprites->End();
    m_deviceResources->PIXEndEvent();

	m_Camera.Update();
	m_view = m_Camera.getCameraMatrix();
	m_world = Matrix::Identity;
	//Set Rendering states. 
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	//context->RSSetState(m_states->CullClockwise());
	context->RSSetState(m_raster.Get());

	// Turn our shaders on,  set parameters
	//m_BasicShaderPair.EnableShader(context);
	//m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get());

	//render our sphere
	//m_BasicModel.Render(context);

	//prepare transform for second object. 
	m_batchEffect->SetView(m_view);
	m_batchEffect->SetProjection(m_projection);

	if (m_isPlaying)
	{
		lightDirection = light.get()->GetLightDirection(m_gameTime);
	}
	else if (m_gameTime < 0.01f)
	{
		m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());

		m_spriteBatch->Draw(m_title.Get(), m_screenPos, nullptr, Colors::White,
			0.f, m_origin);

		m_spriteBatch->End();
	}

	m_guideModel->UpdateEffects([&](IEffect* effect)
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
	m_batchEffect->SetPerPixelLighting(true);
	m_batchEffect->SetLightDirection(0, lightDirection);
	m_batchEffect->SetLightDiffuseColor(0, lightDiffuseColor);

	for (vector<Object>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		Object current_object = *it;
		current_object.Render(m_gameTime, context, m_batchEffect.get(), m_batchInputLayout.Get(), m_states.get(), m_view, m_projection, lightDiffuseColor, lightDirection);
	}

	//m_world = SimpleMath::Matrix::Identity; //set world back to identity
	//Matrix newPosition2 = SimpleMath::Matrix::CreateTranslation(2.0f, -1.0f, 1.0f);
	//m_world = m_world * newPosition2;
	//m_guideModel->Draw(context, *m_states, m_world, m_view, m_projection, false);

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

	const float grey[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
    context->ClearRenderTargetView(renderTarget, grey);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
    m_audEngine->Suspend();
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();
    m_audEngine->Resume();
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

	CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);
	DX::ThrowIfFailed(device->CreateRasterizerState(&rastDesc,
		m_raster.ReleaseAndGetAddressOf()));

    m_states = std::make_unique<CommonStates>(device);

    m_fxFactory = std::make_unique<EffectFactory>(device);

    m_sprites = std::make_unique<SpriteBatch>(context);

    m_batchEffect = std::make_unique<BasicEffect>(device);
	m_batchEffect->SetTextureEnabled(true);
	m_batchEffect->SetPerPixelLighting(true);
	m_batchEffect->SetLightingEnabled(true);
	m_batchEffect->SetLightEnabled(0, true);
	//m_batchEffect->SetLightDiffuseColor(0, Colors::White);
	//m_batchEffect->SetLightEnabled(1, false);
	//m_batchEffect->SetLightEnabled(2, false);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionNormalColorTexture::InputElements,
				VertexPositionNormalColorTexture::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");

	//setup our test model
	//m_BasicModel.InitializeSphere(device);

	//m_BasicModel2.InitializeModel(device, "Assets/Models/drone.obj");
	//m_BasicModel3.InitializeBox(device, 10.0f, 0.1f, 10.0f);	//box includes dimensions
	//m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");
	m_fxFactory->SetDirectory(L"Assets/Textures");
	m_fxFactory->EnableNormalMapEffect(true);
	//m_model = Model::CreateFromCMO(device, L"Assets/Models/title.cmo", *m_fxFactory);
	m_guideModel = Model::CreateFromCMO(device, L"Assets/Models/untitled.cmo", *m_fxFactory);
	//m_model2 = GeometricPrimitive::CreateIcosahedron(context);
	//m_model2->CreateInputLayout(m_batchEffect.get(), m_batchInputLayout.ReleaseAndGetAddressOf());
	

	CreateDDSTextureFromFile(device, L"Assets/Textures/white.dds", nullptr, m_texture1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Assets/Textures/grey.dds", nullptr, m_texture2.ReleaseAndGetAddressOf());
	m_batchEffect->SetTexture(m_texture1.Get());

	m_spriteBatch = std::make_unique<SpriteBatch>(context);
	ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, L"Assets/Textures/title_sprite.png",
			resource.GetAddressOf(),
			m_title.ReleaseAndGetAddressOf()));

	ComPtr<ID3D11Texture2D> title;
	DX::ThrowIfFailed(resource.As(&title));

	CD3D11_TEXTURE2D_DESC titleDesc;
	title->GetDesc(&titleDesc);

	m_origin.x = float(titleDesc.Width / 2);
	m_origin.y = float(titleDesc.Height / 2);

	auto m_outputSize = m_deviceResources->GetOutputSize();
	UINT backBufferWidth = std::max<UINT>(m_outputSize.right - m_outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(m_outputSize.bottom - m_outputSize.top, 1);

	m_screenPos.x = backBufferWidth / 2.f;
	m_screenPos.y = backBufferHeight / 2.f;

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );

	m_batchEffect->SetView(m_view);
    m_batchEffect->SetProjection(m_projection);
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
  
    m_batchEffect.reset();
    m_font.reset();
   
    m_batchInputLayout.Reset();
	m_raster.Reset();
	m_title.Reset();
	//m_model.reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
