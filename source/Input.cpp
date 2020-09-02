#include "pch.h"
#include "Input.h"


Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_quitApp = false;

	m_GameInput.reload = false;
	m_GameInput.space = false;

	m_GameInput.forward = false;
	m_GameInput.back = false;
	m_GameInput.goLeft = false;
	m_GameInput.goRight = false;
	m_GameInput.rotRight = false;
	m_GameInput.rotLeft = false;
	m_GameInput.lightLeft = false;
	m_GameInput.lightRight = false;
	m_GameInput.lightUp = false;
	m_GameInput.lightDown = false;
	m_GameInput.fastForward = false;
	m_GameInput.fastBackward = false;
}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	if (kb.R)	m_GameInput.reload = true;
	else		m_GameInput.reload = false;

	if (m_KeyboardTracker.pressed.Space)	m_GameInput.space = true;
	else		m_GameInput.space = false;

	if (kb.Up)	m_GameInput.rotUp = true;
	else		m_GameInput.rotUp = false;

	if (kb.Down)	m_GameInput.rotDown = true;
	else		m_GameInput.rotDown = false;

	if (kb.Left)	m_GameInput.rotLeft = true;
	else        m_GameInput.rotLeft = false;

	if (kb.Right)	m_GameInput.rotRight = true;
	else        m_GameInput.rotRight = false;

	if (kb.W)	m_GameInput.forward = true;
	else        m_GameInput.forward = false;

	if (kb.S)	m_GameInput.back = true;
	else        m_GameInput.back = false;

	if (kb.A)	m_GameInput.goLeft = true;
	else        m_GameInput.goLeft = false;

	if (kb.D)	m_GameInput.goRight = true;
	else        m_GameInput.goRight = false;

	if (kb.I)	m_GameInput.lightUp = true;
	else        m_GameInput.lightUp = false;

	if (kb.J)	m_GameInput.lightLeft = true;
	else        m_GameInput.lightLeft = false;

	if (kb.K)	m_GameInput.lightDown = true;
	else        m_GameInput.lightDown = false;

	if (kb.L)	m_GameInput.lightRight = true;
	else        m_GameInput.lightRight = false;

	if (m_KeyboardTracker.pressed.D1)	m_GameInput.fastBackward = true;
	else        m_GameInput.fastBackward = false;

	if (m_KeyboardTracker.pressed.D2)	m_GameInput.fastForward = true;
	else        m_GameInput.fastForward = false;
}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
