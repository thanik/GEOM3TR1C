//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"
#include "ModelObject.h"
#include "pch.h"
#include "Object.h"

using namespace std;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

	double m_gameTime;
	bool m_isPlaying;
	double bpm;
	double beat_offset;
	double seconds_per_beat;
	double last_beat_time;
	int last_beat_update;
	bool debug_build;
    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    // Input devices.
	Input									m_input;
	InputCommands							m_gameInputCommands;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
	std::unique_ptr<DirectX::SoundEffect>									m_audio;
	std::unique_ptr<DirectX::SoundEffectInstance>							m_audioIns;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

	//Light																	m_Light;
	Camera																	m_Camera;
	//ModelObject																m_BasicModel;
	//ModelObject																m_BasicModel2;
	//ModelObject																m_BasicModel3;
	//std::unique_ptr<DirectX::Model> m_model;
	std::unique_ptr<DirectX::Model> m_guideModel;
	//std::unique_ptr<DirectX::GeometricPrimitive> m_model2;

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>						m_title;
	vector<Object> objects;
	Vector3 lightDirection;
	Vector4 lightDiffuseColor;
	std::unique_ptr <Object> light;

	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	DirectX::SimpleMath::Vector2 m_screenPos;
	DirectX::SimpleMath::Vector2 m_origin;
};