#include "Menu.h"

MainMenu::MainMenu(Aen::Window& window)
	:State(window)
{

}

MainMenu::~MainMenu()
{
}

void MainMenu::Update(const float& deltaTime)
{
	if (Aen::Input::KeyDown(Aen::Key::NUMPAD0))
	{
		State::SetState(States::Loadscreen);
	}
	cout << "Menu\n";
}

void MainMenu::Initialize()
{
	m_Window.SetWindowSize(static_cast<UINT>(GetSystemMetrics(SM_CXSCREEN) * 0.4f), static_cast<UINT>(GetSystemMetrics(SM_CYSCREEN) * 0.4f));
}
