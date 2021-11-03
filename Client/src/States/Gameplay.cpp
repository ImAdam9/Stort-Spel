#include "Gameplay.h"

Gameplay::Gameplay(Aen::Window& window)
	:State(window), m_speed(10.f), m_fSpeed(0.15f), m_toggleFullScreen(true), m_hp(100.f),
	IFRAMEMAX(1.5f), m_iFrames(0.f) {}

Gameplay::~Gameplay() {
	Aen::GlobalSettings::RemoveMainCamera();
	Aen::EntityHandler::RemoveEntity(*m_dLight);
	Aen::EntityHandler::RemoveEntity(*m_plane);
	Aen::EntityHandler::RemoveEntity(*m_reimube);
	Aen::EntityHandler::RemoveEntity(*m_UI);
	for (auto& d : m_enemyQueue) {
		delete d;
	}
	//Aen::Resource::RemoveAllMaterials();
	//Aen::Resource::RemoveAllMeshes();
	//Aen::Resource::RemoveAllTextures();
}

void Gameplay::Initialize()
{
	State::SetLoad(false);
	// -----------------------------	UI	------------------------------- //
	m_UI = &Aen::EntityHandler::CreateEntity();
	m_UI->AddComponent<Aen::UIComponent>();
	m_UI->GetComponent<Aen::UIComponent>().AddPicture(L"../Resource/healthbar.png", 0);
	m_UI->GetComponent<Aen::UIComponent>().SetPicPos(220.f, 60.f, 0);
	m_UI->GetComponent<Aen::UIComponent>().SetPicSize(m_hp * 4.f, 150.f, 0);

	m_UI->GetComponent<Aen::UIComponent>().AddPicture(L"../Resource/GoalText.png", 1);
	m_UI->GetComponent<Aen::UIComponent>().SetPicPos(965.f, 100.f, 1);
	m_UI->GetComponent<Aen::UIComponent>().SetPicSize(1000.f, 100.f, 1);

	// ----------------------------- Setup Camera ------------------------------- //

	// ------------------------ Setup Directional Light ------------------------- //

	m_dLight = &Aen::EntityHandler::CreateEntity();
	m_dLight->AddComponent<Aen::DirectionalLight>();
	m_dLight->GetComponent<Aen::DirectionalLight>().SetColor(Aen::Color::White);
	m_dLight->GetComponent<Aen::DirectionalLight>().SetStrength(1.f);
	m_dLight->SetRot(45.f, -135.f, 0.f);

	// ----------------------------- Load Meshes -------------------------------- //

	Aen::Mesh& plane = Aen::Resource::CreateMesh("Plane");
	plane.Load(AEN_RESOURCE_DIR("Floor_Final.fbx"));
	Aen::Mesh& rimuru = Aen::Resource::CreateMesh("Rimuru");
	rimuru.Load(AEN_RESOURCE_DIR("Slime.fbx"));
	Aen::Mesh& reimube = Aen::Resource::CreateMesh("Reimube");
	reimube.Load(AEN_RESOURCE_DIR("Cube.fbx"));
	Aen::Mesh& wall = Aen::Resource::CreateMesh("Wall");
	wall.Load(AEN_RESOURCE_DIR("Wall_Final.fbx"));
	Aen::Mesh& wallDoor = Aen::Resource::CreateMesh("WallDoor");
	wallDoor.Load(AEN_RESOURCE_DIR("Wall_Door_Final.fbx"));

	// -------------------------- Setup Material -------------------------------- //

	Aen::Material& planeMat = Aen::Resource::CreateMaterial("PlaneMaterial");
	Aen::Material& enemyMat = Aen::Resource::CreateMaterial("EnemyMaterial");
	Aen::Material& reimubeMat = Aen::Resource::CreateMaterial("ReimubeMat");
	Aen::Material& wallMat = Aen::Resource::CreateMaterial("WallMat");

	enemyMat.LoadeAndSetDiffuseMap(AEN_RESOURCE_DIR("SlimeRimuruFace.png"));
	enemyMat["InnerEdgeColor"] = Aen::Color::Cyan;
	enemyMat["OuterEdgeColor"] = Aen::Color::Cyan;

	wallMat.LoadeAndSetDiffuseMap(AEN_RESOURCE_DIR("Brick_Diffuse.png"));
	wallMat["InnerEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);
	wallMat["OuterEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);

	reimubeMat.LoadeAndSetDiffuseMap(AEN_RESOURCE_DIR("greenMage.png"));
	reimubeMat["InnerEdgeColor"] = Aen::Color::Pink;
	reimubeMat["OuterEdgeColor"] = Aen::Color::Pink;

	planeMat.LoadeAndSetDiffuseMap(AEN_RESOURCE_DIR("Floor_Diffuse.png"));
	planeMat["InnerEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);
	planeMat["OuterEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);

	// -------------------------- Setup Entities -------------------------------- //

	m_wall = &Aen::EntityHandler::CreateEntity();
	m_wall->AddComponent<Aen::MeshInstance>();
	m_wall->GetComponent<Aen::MeshInstance>().SetMesh(wallDoor);
	m_wall->GetComponent<Aen::MeshInstance>().SetMaterial(wallMat);
	m_wall->SetPos(22.f, 0.f, 0.f);

	Aen::Entity* wallE = &Aen::EntityHandler::CreateEntity();
	wallE->AddComponent<Aen::MeshInstance>();
	wallE->GetComponent<Aen::MeshInstance>().SetMesh(wall);
	wallE->GetComponent<Aen::MeshInstance>().SetMaterial(wallMat);
	wallE->SetPos(-22.f, 0.f, 0.f);

	wallE = nullptr;

	wallE = &Aen::EntityHandler::CreateEntity();
	wallE->AddComponent<Aen::MeshInstance>();
	wallE->GetComponent<Aen::MeshInstance>().SetMesh(wall);
	wallE->GetComponent<Aen::MeshInstance>().SetMaterial(wallMat);
	wallE->SetPos(0.f, 0.f, 22.f);
	wallE->SetRot(0.f, 90.f, 0.f);

	wallE = nullptr;

	wallE = &Aen::EntityHandler::CreateEntity();
	wallE->AddComponent<Aen::MeshInstance>();
	wallE->GetComponent<Aen::MeshInstance>().SetMesh(wall);
	wallE->GetComponent<Aen::MeshInstance>().SetMaterial(wallMat);
	wallE->SetPos(0.f, 0.f, -22.f);
	wallE->SetRot(0.f, 90.f, 0.f);

	wallE = nullptr;

	wallE = &Aen::EntityHandler::CreateEntity();
	wallE->AddComponent<Aen::MeshInstance>();
	wallE->GetComponent<Aen::MeshInstance>().SetMesh(plane);
	wallE->GetComponent<Aen::MeshInstance>().SetMaterial(planeMat);
	wallE->SetPos(0.f, 22.f, 0.f);

	wallE = nullptr;

	m_plane = &Aen::EntityHandler::CreateEntity();
	m_plane->AddComponent<Aen::RigidBody>();
	m_plane->GetComponent<Aen::RigidBody>().SetOffset(0.f, -0.5f, 0.f);
	m_plane->GetComponent<Aen::RigidBody>().SetGeometry(Aen::GeometryType::CUBE, Aen::Vec3f(1.f, 44.f, 44.f));
	m_plane->AddComponent<Aen::MeshInstance>();
	m_plane->GetComponent<Aen::MeshInstance>().SetMesh(plane);
	m_plane->GetComponent<Aen::MeshInstance>().SetMaterial(planeMat);

	m_reimube = &Aen::EntityHandler::CreateEntity();
	//m_reimube->AddComponent<Aen::RigidBody>();
	//m_reimube->GetComponent<Aen::RigidBody>().SetGeometry(Aen::GeometryType::CUBE, Aen::Vec3f(2.f, 2.f, 2.f));
	//m_reimube->GetComponent<Aen::RigidBody>().SetRigidType(Aen::RigidType::STATIC);
	m_reimube->AddComponent<Aen::AABoundBox>();
	m_reimube->GetComponent<Aen::MeshInstance>().SetMesh(reimube);
	m_reimube->GetComponent<Aen::MeshInstance>().SetMaterial(reimubeMat);
	m_reimube->GetComponent<Aen::AABoundBox>().SetBoundsToMesh();
	m_reimube->SetPos(0.f, 1.f, -3.f);

	//m_attack->SetParent(*m_player);

	//printf("");

	// --------------------------- Setup Window --------------------------------- //

	m_Window.SetWindowSize(static_cast<UINT>(GetSystemMetrics(SM_CXSCREEN) * 0.4f), static_cast<UINT>(GetSystemMetrics(SM_CYSCREEN) * 0.4f));
	Aen::WindowDesc wDesc;
	wDesc.width = GetSystemMetrics(SM_CXSCREEN) + 4u;
	wDesc.height = GetSystemMetrics(SM_CYSCREEN) + 4u;
	wDesc.EXStyle = AEN_WS_EX_APPWINDOW;
	wDesc.style = AEN_WS_POPUPWINDOW | AEN_WS_VISIBLE;
	m_Window.LoadSettings(wDesc);

	Aen::Input::ToggleRawMouse(true);
	Aen::Input::SetMouseVisible(false);
	cout << "Press Enter To Continue\n";

	srand((UINT)time(NULL));
}

// ---------------------------------------------------------		Update		--------------------------------------------------------------- //

void Gameplay::Update(const float& deltaTime) {

	// Collision
	//m_UI->GetComponent<Aen::UIComponent>().SetPicSize(m_hp * 4.f, 150.f, 0);

	if (m_hp != m_player.GetHealth()) { //ers�tt collision med enemy i if satsen
		float hp = (m_hp - m_player.GetHealth());

		m_UI->GetComponent<Aen::UIComponent>().LessenPic(hp * 4, 0);
		m_hp = m_player.GetHealth();
	}

	/*for (auto& h : m_enemyQueue) {
		if (m_player.GetEntity()->GetComponent<Aen::AABoundBox>().Intersects(h->GetEntity()->GetComponent<Aen::AABoundBox>())){
			m_player.GetEntity()->GetComponent<Aen::AABoundBox>().ToggleActive(false);
			m_hp--;
			cout << "PLAYER HEALTH: " << m_hp << endl;;
		}
	}*/
	 //Invincible frames
	//if (m_invincible && m_iFrames <= IFRAMEMAX) {
	//	m_iFrames += deltaTime;
	//	//printf("Iframes: %f\n", m_iFrames);
	//}
	//else {
	//	m_player.GetEntity()->GetComponent<Aen::AABoundBox>().ToggleActive(true);
	//	m_iFrames = 0.f;
	//}


	if (m_toggleFullScreen)
		Aen::Input::SetMousePos((Aen::Vec2i)Aen::Vec2f(GetSystemMetrics(SM_CXSCREEN) * 0.5f, GetSystemMetrics(SM_CYSCREEN) * 0.5f));
	else
		Aen::Input::SetMousePos(m_Window.GetWindowPos() + (Aen::Vec2i)((Aen::Vec2f)m_Window.GetSize() * 0.5f));

	// ---------------------------------- Enemies --------------------------------------- //

	m_player.Update(m_enemyQueue, deltaTime);

	for(auto& i : m_enemyQueue)
		i->Update(deltaTime, m_player);

	m_player.UpdateAttack(m_enemyQueue, deltaTime);

	if(m_player.GetHealth() <= 0.f)
		State::SetState(States::Gameover);

	if(Aen::Input::KeyDown(Aen::Key::J))
		m_enemyQueue.emplace_back(AEN_NEW Rimuru());


	// ------------------------------ Toggle Fullscreen --------------------------------- //

	if (Aen::Input::KeyDown(Aen::Key::F1)) {
		m_toggleFullScreen = !m_toggleFullScreen;
		Aen::WindowDesc wDesc;

		if (m_toggleFullScreen) {
			wDesc.width = GetSystemMetrics(SM_CXSCREEN) + 4u;
			wDesc.height = GetSystemMetrics(SM_CYSCREEN) + 4u;
			wDesc.EXStyle = AEN_WS_EX_APPWINDOW;
			wDesc.style = AEN_WS_POPUPWINDOW | AEN_WS_VISIBLE;
			m_Window.LoadSettings(wDesc);
		}
		else {
			wDesc.width = static_cast<UINT>(GetSystemMetrics(SM_CXSCREEN) * 0.4f);
			wDesc.height = static_cast<UINT>(GetSystemMetrics(SM_CYSCREEN) * 0.4f);
			wDesc.EXStyle = AEN_WS_EX_APPWINDOW;
			wDesc.style = AEN_WS_OVERLAPPEDWINDOW | AEN_WS_VISIBLE;
			m_Window.LoadSettings(wDesc);
		}
	}

	// ------------------------------ Quick Exist Button -------------------------------- //

	if (Aen::Input::KeyDown(Aen::Key::ESCAPE))
		m_Window.Exit();

	// ------------------------------------- States -------------------------------------- //
	/*if (m_hp <= 0 && m_enemyQueue.size() == 0)
	{
		State::SetState(States::Gameover);
	}*/
}