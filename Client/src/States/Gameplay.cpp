#include "Gameplay.h"

Gameplay::Gameplay(Aen::Window& window)
	:State(window), m_speed(10.f), m_fSpeed(0.15f), m_toggleFullScreen(true), m_hp(200.f), m_timer(0),m_deathTimer(0),
	IFRAMEMAX(1.5f), m_iFrames(0.f) {}

Gameplay::~Gameplay() {
	//Aen::EntityHandler::RemoveEntity(*m_dLight);
	Aen::EntityHandler::RemoveEntity(*m_plane);
	Aen::EntityHandler::RemoveEntity(*m_reimube1);
	Aen::EntityHandler::RemoveEntity(*m_UI);
	//Aen::EntityHandler::RemoveEntity(*m_wall);
	
	for (auto& b : *m_levelImporter.GetEntityList()) {
		Aen::EntityHandler::RemoveEntity(*b);
	}

	for (auto& d : m_enemyQueue) {
		delete d;
	}

	Aen::Resource::RemoveAllMaterials();
	Aen::Resource::RemoveAllMeshes();
	Aen::Resource::RemoveAllTextures();
}

void Gameplay::Initialize()
{
	srand((UINT)time(NULL));
	State::SetLoad(false);

	m_UI->GetComponent<Aen::UIComponent>().AddText(L"Interact (F)", 60.f); //2
	m_UI->GetComponent<Aen::UIComponent>().SetTextPos(965.f, 800.f);
	m_UI->GetComponent<Aen::UIComponent>().SetTextSize(900.f, 300);
	m_UI->GetComponent<Aen::UIComponent>().SetColor(D2D1::ColorF::Aqua);

	// ----------------------------- Setup Camera ------------------------------- //

	// ------------------------ Setup Directional Light ------------------------- //

	/*m_dLight = &Aen::EntityHandler::CreateEntity();
	m_dLight->AddComponent<Aen::DirectionalLight>();
	m_dLight->GetComponent<Aen::DirectionalLight>().SetColor(Aen::Color::White);
	m_dLight->GetComponent<Aen::DirectionalLight>().SetStrength(1.f);
	m_dLight->SetRot(45.f, -135.f, 0.f);*/

	// ----------------------------- Load Meshes -------------------------------- //

	//Aen::Mesh& plane = Aen::Resource::CreateMesh("Plane");
	//plane.Load(AEN_RESOURCE_DIR("Floor_Final.fbx"));
	Aen::Mesh& rimuru = Aen::Resource::CreateMesh("Rimuru");
	rimuru.Load(AEN_RESOURCE_DIR("Slime.fbx"));
	Aen::Mesh& reimube = Aen::Resource::CreateMesh("Reimube");
	reimube.Load(AEN_RESOURCE_DIR("Cube.fbx"));
	//Aen::Mesh& wall = Aen::Resource::CreateMesh("Wall");
	//wall.Load(AEN_RESOURCE_DIR("Wall_Final.fbx"));
	//Aen::Mesh& wallDoor = Aen::Resource::CreateMesh("WallDoor");
	//wallDoor.Load(AEN_RESOURCE_DIR("Wall_Door_Final.fbx"));

	// -------------------------- Setup Material -------------------------------- //

	Aen::Material& planeMat = Aen::Resource::CreateMaterial("PlaneMaterial");
	Aen::Material& enemyMat = Aen::Resource::CreateMaterial("EnemyMaterial");
	Aen::Material& enemyMatHurt = Aen::Resource::CreateMaterial("EnemyMaterialHurt");
	Aen::Material& reimubeMat = Aen::Resource::CreateMaterial("ReimubeMat");
	//Aen::Material& wallMat = Aen::Resource::CreateMaterial("WallMat");

	enemyMat.LoadeAndSetDiffuseMap(AEN_RESOURCE_DIR("SlimeRimuruFace.png"));
	//enemyMat.LoadeAndSetOpacityMap(AEN_RESOURCE_DIR("SakuyaI.png"));
	enemyMat["InnerEdgeColor"] = Aen::Color::Cyan;
	enemyMat["OuterEdgeColor"] = Aen::Color::Cyan;
	enemyMat["BaseColor"] = Aen::Color::Cyan;

	// Material to switch to when enemy is hurt
	enemyMatHurt["BaseColor"] = Aen::Color::Red;
	/*wallMat.LoadeAndSetDiffuseMap(AEN_RESOURCE_DIR("Brick_Diffuse.png"));
	wallMat["InnerEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);
	wallMat["OuterEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);*/

	reimubeMat.LoadeAndSetDiffuseMap(AEN_RESOURCE_DIR("greenMage.png"));
	reimubeMat["InnerEdgeColor"] = Aen::Color::Pink;
	reimubeMat["OuterEdgeColor"] = Aen::Color::Pink;

	planeMat.LoadeAndSetDiffuseMap(AEN_RESOURCE_DIR("Floor_Diffuse.png"));
	planeMat["InnerEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);
	planeMat["OuterEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);

	//targetMat["InnerEdgeColor"] = Aen::Color::Red;
	//targetMat["OuterEdgeColor"] = Aen::Color::Green;

	// -------------------------- Setup Entities -------------------------------- //

	m_plane = &Aen::EntityHandler::CreateEntity();
	m_plane->AddComponent<Aen::StaticBody>();
	m_plane->GetComponent<Aen::StaticBody>().SetGeometry(Aen::StaticGeometryType::PLANE);


	//m_plane->GetComponent<Aen::StaticBody>().SetGeometry(Aen::StaticGeometryType::CUBE, Aen::Vec3f(1.f, 44.f, 44.f));
	//m_plane->AddComponent<Aen::MeshInstance>();
	//m_plane->GetComponent<Aen::MeshInstance>().SetMesh(plane);
	//m_plane->GetComponent<Aen::MeshInstance>().SetMaterial(planeMat);

	m_reimube1 = &Aen::EntityHandler::CreateEntity();
	m_reimube1->AddComponent<Aen::MeshInstance>();
	m_reimube1->GetComponent<Aen::MeshInstance>().SetMesh(reimube);
	m_reimube1->GetComponent<Aen::MeshInstance>().SetMaterial(enemyMat);
	//m_reimube1->AddComponent<Aen::StaticBody>();
	//m_reimube1->GetComponent<Aen::StaticBody>().SetBoundsToMesh(true);
	m_reimube1->SetPos(0.f, 1.f, 11.f);
	m_reimube1->SetRot(10, 1, 1);
	//m_reimube1->SetRenderLayer(1);

	// ------ Level Importer ------ //


	// ------------------- Procedural generation testing staging grounds ------- //
	m_levelGenerator.LoadMutipleRoomFiles();


	m_levelGenerator.AddLoadedToGeneration();

	m_levelGenerator.SetMapTheme(Aen::RoomTheme::GENERIC);

	//Match this value to the size of the rooms we are using
	m_levelGenerator.SetRoomDimension(43.f);
	mptr_map = m_levelGenerator.GenerateLevel();
	m_levelGenerator.CleanMap();

	//Use this value to set the start of the player / origin of the map
	Aen::Vec3f playerStartPos;
	Aen::Vec3f ChestPos;
	Aen::Vec3f EnemyPos;

	for (UINT y = 0; y < Aen::mapSize; y++) {
		for (UINT x = 0; x < Aen::mapSize; x++) {
			m_levelGenerator.SpawnRoom(Aen::Vec2i(x, y));

			if (mptr_map[y * Aen::mapSize + x].m_roomSpecial == Aen::SpecialRoom::ENTRANCE) {
				m_levelGenerator.GetRoomPos(x, y, &playerStartPos.x, &playerStartPos.z);
			}
			mptr_map[x + y * Aen::mapSize].mptr_parent;

			if (mptr_map[y * Aen::mapSize + x].m_roomSpecial == Aen::SpecialRoom::ITEM) {

				for (int i = 0; i < 2; i++) {
					m_levelGenerator.GetRoomPos(x, y, &EnemyPos.x, &EnemyPos.z);
					m_enemyQueue.emplace_back(AEN_NEW Rimuru(EnemyPos));
					m_enemyQueue.at(i)->GetEntity()->SetTag("ItemEnemy");
				}
				m_levelGenerator.GetRoomPos(x, y, &ChestPos.x, &ChestPos.z);
			}

			if (mptr_map[y * Aen::mapSize + x].m_roomSpecial == Aen::SpecialRoom::NONE) {
				m_levelGenerator.GetRoomPos(x, y, &EnemyPos.x, &EnemyPos.z);
				m_enemyQueue.emplace_back(AEN_NEW Rimuru(EnemyPos));
			}

			if (mptr_map[y * Aen::mapSize + x].m_roomSpecial == Aen::SpecialRoom::BOSS) {

				int index = m_enemyQueue.size();
				m_levelGenerator.GetRoomPos(x, y, &EnemyPos.x, &EnemyPos.z);
				m_enemyQueue.emplace_back(AEN_NEW Rimuru(EnemyPos));
				m_enemyQueue.at(index)->GetEntity()->SetScale(2.f);
			}
		}
	}
	m_chest.GetObjectEntity()->SetPos(ChestPos);
	m_player.GetEntity()->SetPos(ChestPos.x + 10.f, ChestPos.y, ChestPos.z);
	m_chest.SetType(Type::Locked);

	//m_attack->SetParent(*m_player);

	//printf("");

	//std::vector<Aen::Vec3f> tempEnemies = m_levelGenerator.GetHandlerPtr()->GetEnemyPos();
	//for (size_t i = 0; i < m_levelGenerator.GetHandlerPtr()->GetEnemyPos().size(); i++)
	//{
	//	m_enemyQueue.emplace_back(AEN_NEW Rimuru(tempEnemies[i]));
	//}
	//m_enemyQueue.emplace_back(AEN_NEW Rimuru(Aen::Vec3f(0,0,0)));


	// --------------------------- Setup Window --------------------------------- //

	m_Window.SetWindowSize(static_cast<UINT>(GetSystemMetrics(SM_CXSCREEN) * 0.4f), static_cast<UINT>(GetSystemMetrics(SM_CYSCREEN) * 0.4f));
	Aen::WindowDesc wDesc;
	wDesc.width = GetSystemMetrics(SM_CXSCREEN) + 4u;
	wDesc.height = GetSystemMetrics(SM_CYSCREEN) + 4u;
	wDesc.EXStyle = AEN_WS_EX_APPWINDOW;
	wDesc.style = AEN_WS_POPUPWINDOW | AEN_WS_VISIBLE;
	m_Window.LoadSettings(wDesc);
	screenWidth = wDesc.width;

	// -----------------------------	UI	------------------------------- //
	m_UI = &Aen::EntityHandler::CreateEntity();
	m_UI->AddComponent<Aen::UIComponent>();
	m_UI->GetComponent<Aen::UIComponent>().AddPicture(AEN_RESOURCE_DIR_W(L"healthbar.png")); //0
	m_UI->GetComponent<Aen::UIComponent>().SetPicPos((350.f / 1920)* wDesc.width, (100.f / 1024) * wDesc.height);
	m_UI->GetComponent<Aen::UIComponent>().SetPicSize(((m_hp * 2.f) / 1920) * wDesc.width, (150.f / 1024) * wDesc.height);

	m_UI->GetComponent<Aen::UIComponent>().AddPicture(AEN_RESOURCE_DIR_W(L"bar.png")); //1
	m_UI->GetComponent<Aen::UIComponent>().SetPicPos((350.f / 1920) * wDesc.width, (100.f / 1024) * wDesc.height);
	m_UI->GetComponent<Aen::UIComponent>().SetPicSize(((m_hp * 2.f) / 1920) * wDesc.width, (150.f / 1024) * wDesc.height);

	m_UI->GetComponent<Aen::UIComponent>().AddPicture(AEN_RESOURCE_DIR_W(L"potion.png")); //2
	m_UI->GetComponent<Aen::UIComponent>().SetPicPos((125.f / 1920) * wDesc.width, (100.f / 1024) * wDesc.height);
	m_UI->GetComponent<Aen::UIComponent>().SetPicSize((150.f / 1920) * wDesc.width, (150.f / 1024) * wDesc.height);

	//m_UI->GetComponent<Aen::UIComponent>().AddPicture(AEN_RESOURCE_DIR_W(L"GoalText.png"), 1);
	//m_UI->GetComponent<Aen::UIComponent>().SetPicPos(965.f, 100.f, 1);
	//m_UI->GetComponent<Aen::UIComponent>().SetPicSize(600.f, 100.f, 1);

	m_UI->GetComponent<Aen::UIComponent>().AddText(L"Kill All Enemies", 72.f); //0
	m_UI->GetComponent<Aen::UIComponent>().SetTextPos((965.f / 1920) * wDesc.width, (100.f / 1024) * wDesc.height);
	m_UI->GetComponent<Aen::UIComponent>().SetTextSize((900.f / 1920) * wDesc.width, (300 / 1024) * wDesc.height);

		m_UI->GetComponent<Aen::UIComponent>().AddText(L"5", 50.f); //1 - Amount of potion
	m_UI->GetComponent<Aen::UIComponent>().SetTextPos((120.f / 1920) * wDesc.width, (110.f / 1024) * wDesc.height);
	m_UI->GetComponent<Aen::UIComponent>().SetTextSize((150.f / 1920) * wDesc.width, (150.f / 1024) * wDesc.height);
	m_UI->GetComponent<Aen::UIComponent>().SetColor(D2D1::ColorF::Black);

	Aen::Input::ToggleRawMouse(true);
	Aen::Input::SetMouseVisible(false);
#ifdef _DEBUG
	cout << "Press Enter To Continue\n";
#endif // _DEBUG

}

// ---------------------------------------------------------		Update		--------------------------------------------------------------- //

void Gameplay::Update(const float& deltaTime) {
	
	wstringstream potionNr;
	potionNr << m_player.GetPotionNr();
	m_player.PotionUpdate();
	if (m_hp != m_player.GetHealth()) {
		float hp = (m_hp - m_player.GetHealth());
		potionNr << m_player.GetPotionNr();

		m_UI->GetComponent<Aen::UIComponent>().UpdatePicture((hp * 2.f) * (1.f/1920.f) * screenWidth, 0);
		m_UI->GetComponent<Aen::UIComponent>().TextNr(1, potionNr.str().c_str());
		m_hp = m_player.GetHealth();
	}
	m_UI->GetComponent<Aen::UIComponent>().TextNr(1, potionNr.str().c_str());
	//cout << "hp: " << m_hp << "		player: " << m_player.GetHealth() << endl;

	if (m_toggleFullScreen)
		Aen::Input::SetMousePos((Aen::Vec2i)Aen::Vec2f(GetSystemMetrics(SM_CXSCREEN) * 0.5f, GetSystemMetrics(SM_CYSCREEN) * 0.5f));
	else
		Aen::Input::SetMousePos(m_Window.GetWindowPos() + (Aen::Vec2i)((Aen::Vec2f)m_Window.GetSize() * 0.5f));

	// ---------------------------------- Enemies --------------------------------------- //

	m_player.Update(m_enemyQueue, deltaTime);

	m_chest.Update(deltaTime, m_player.GetEntity());
	if (m_chest.GetNear()) {
		m_UI->GetComponent<Aen::UIComponent>().SetTextPos(965.f, 800.f, 2);
		m_UI->GetComponent<Aen::UIComponent>().SetTextSize(900.f, 300.f, 2);

		//if (m_enemyQueue.empty())
		//	m_chest.SetType(Type::Closed);

		if (Aen::Input::KeyDown(Aen::Key::F) && m_chest.GetType() == Type::Open) {
			m_player.IncreaseHealthCap();
			m_chest.SetType(Type::Closed);
		}
	}
	else {
		m_UI->GetComponent<Aen::UIComponent>().SetTextPos(-100.f, 0.f, 2);
	}


	for (auto& i : m_enemyQueue) {
		i->Update(deltaTime, m_player);
		m_chest.Update(deltaTime, i->GetEntity());
	}

	m_player.UpdateAttack(m_enemyQueue, deltaTime);


	if (m_hp <= 0.f) {
		SetWin(false);
		m_UI->GetComponent<Aen::UIComponent>().SetPicPos(0.f, 0.f, 0);
		m_deathTimer += deltaTime;

		if (m_deathTimer > 0.1f) {
			State::SetState(States::Gameover);
		}
	}

	//if (m_enemyQueue.empty()) {
	//	SetWin(true);
	//	State::SetState(States::Gameover);
	//}

	#ifdef _DEBUG
		if(Aen::Input::KeyDown(Aen::Key::J))
			m_enemyQueue.emplace_back(AEN_NEW Rimuru());
	#endif


	//if (Aen::Input::KeyDown(Aen::Key::O)) {
	//	delete m_enemyQueue.front();
	//	m_enemyQueue.pop_front();
	//}

	// ------------------------------ Toggle Fullscreen --------------------------------- //

	if (Aen::Input::KeyDown(Aen::Key::F1)) {
		m_toggleFullScreen = !m_toggleFullScreen;
		Aen::WindowDesc wDesc;

		if (m_toggleFullScreen) {
			wDesc.width = GetSystemMetrics(SM_CXSCREEN) + 4u;
			screenWidth = wDesc.width;
			wDesc.height = GetSystemMetrics(SM_CYSCREEN) + 4u;
			wDesc.EXStyle = AEN_WS_EX_APPWINDOW;
			wDesc.style = AEN_WS_POPUPWINDOW | AEN_WS_VISIBLE;
			m_Window.LoadSettings(wDesc);
		}
		else {
			wDesc.width = static_cast<UINT>(GetSystemMetrics(SM_CXSCREEN) * 0.4f);
			screenWidth = wDesc.width;
			wDesc.height = static_cast<UINT>(GetSystemMetrics(SM_CYSCREEN) * 0.4f);
			wDesc.EXStyle = AEN_WS_EX_APPWINDOW;
			wDesc.style = AEN_WS_OVERLAPPEDWINDOW | AEN_WS_VISIBLE;
			m_Window.LoadSettings(wDesc);
		}
	}

	// ------------------------------ Quick Exit Button -------------------------------- //

	if (Aen::Input::KeyDown(Aen::Key::ESCAPE)) {
		//State::SetState(States::Gameover);
		m_Window.Exit();
	}
	// ------------------------------------- States -------------------------------------- //
	//if (m_hp <= 0 && m_enemyQueue.size() == 0)
	//{
	//	State::SetState(States::Gameover);
	//}
}