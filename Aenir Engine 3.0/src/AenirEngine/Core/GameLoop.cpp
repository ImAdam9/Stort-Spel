#include"PCH.h"
#include"GameLoop.h"

namespace Aen {

	GameLoop::GameLoop()
		:m_app(nullptr), m_start(), m_end(), m_frameTime(), m_deltaTime(), m_renderer(nullptr) {}

	void GameLoop::Initialize() {
		//m_app = CreateApp();                       fix this
		int ft = (int)(((double)1 / (double)60) * (double)pow(10, 9));
		m_frameTime = std::chrono::nanoseconds{ft};

		if(!Input::Initialize())
			exit(-1);

		if(!GCore::Concealed::Initialize(m_app->m_window))
			exit(-1);

		GlobalSettings::Initialize(m_app->m_window);


		m_renderer = AEN_NEW Renderer(m_app->m_window);
		m_renderer->Initialize();

		FMOD::System_Create(&m_fmSys);
		FMOD_RESULT fr = m_fmSys->init(32, FMOD_INIT_NORMAL, nullptr);
		if (fr != FMOD_OK) throw;
		m_fmSys->createSound((AEN_RESOURCE_DIR("Sounds/boom.wav")).c_str(), 0, nullptr, &m_fmtestSound);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplDX11_Init(GCore::m_device.Get(), GCore::m_dContext.Get());
		ImGui_ImplWin32_Init(m_app->m_window.GetWHND());
		ImGui::StyleColorsDark();

		m_app->Start();
	}

	void GameLoop::Run() {
		
		m_start = m_end = ResClock::now();
		while(Aen::WindowHandle::HandleMsg()) {
		
			m_end = ResClock::now();
			while(std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_start) > m_frameTime) {
				m_deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_start);
				m_start = ResClock::now();

				if (Aen::Input::KeyDown(Aen::Key::J))
				{
					m_fmSys->playSound(m_fmtestSound, 0, false, &m_fmChannel);
					m_fmChannel->setVolume(.5f);
				}

				if(m_app->m_window.IsActive()) {
					Input::Update();
					m_app->Update(static_cast<float>(m_deltaTime.count()));
				}

				m_renderer->Render(); // VSync
			}
		}

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		Resource::Destroy();
		GCore::Concealed::Release();
		delete m_app;
		delete m_renderer;
	}
}