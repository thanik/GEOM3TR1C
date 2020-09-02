#pragma once
struct InputCommands
{
	bool reload;
	bool space;

	bool forward;
	bool back;
	bool goLeft;
	bool goRight;
	bool rotUp;
	bool rotDown;
	bool rotRight;
	bool rotLeft;
	bool lightLeft;
	bool lightRight;
	bool lightUp;
	bool lightDown;
	bool fastForward;
	bool fastBackward;
};


class Input
{
public:
	Input();
	~Input();
	void Initialise(HWND window);
	void Update();
	bool Quit();
	InputCommands getGameInput();


private:
	bool m_quitApp;
	std::unique_ptr<DirectX::Keyboard>	m_keyboard;
	std::unique_ptr<DirectX::Mouse>		m_mouse;
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;
	DirectX::Mouse::ButtonStateTracker		m_MouseTracker;
	InputCommands							m_GameInput;
};