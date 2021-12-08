#include "Gameplay.h"

Gameplay::Gameplay(Aen::Window& window)
	:State(window), m_speed(10.f), m_fSpeed(0.15f), m_toggleFullScreen(true), m_hp(200.f), m_timer(0),m_deathTimer(0),
	IFRAMEMAX(1.5f), m_iFrames(0.f) {}

Gameplay::~Gameplay() {
	//Aen::EntityHandler::RemoveEntity(*m_dLight);
	Aen::EntityHandler::RemoveEntity(*m_plane);
	mp_uiComp = nullptr;
	Aen::EntityHandler::RemoveEntity(*m_UI);
	Aen::EntityHandler::RemoveEntity(*m_PS);
	
	for (auto& d : m_enemyQueue) {
		delete d;
	}
	m_pSkeleBoss, m_plane, m_UI = nullptr;

	Aen::Resource::RemoveAllMaterials();
	Aen::Resource::RemoveAllMeshes();
	Aen::Resource::RemoveAllTextures();
}

void Gameplay::Initialize()
{
	srand((UINT)time(NULL));
	State::SetLoad(false);

	// ----------------------------- Setup Camera ------------------------------- //

	// ------------------------ Setup Directional Light ------------------------- //

	/*m_dLight = &Aen::EntityHandler::CreateEntity();
	m_dLight->AddComponent<Aen::DirectionalLight>();
	m_dLight->GetComponent<Aen::DirectionalLight>().SetColor(Aen::Color::White);
	m_dLight->GetComponent<Aen::DirectionalLight>().SetStrength(1.f);
	m_dLight->SetRot(45.f, -135.f, 0.f);*/

	// ----------------------------- Animations -------------------------------- //
	
	Aen::Animation& skelIdle = Aen::Resource::CreateAnimation("Skel_Idle");
	skelIdle.LoadAnimation(AEN_MODEL_DIR("Idle_skelTest2.fbx"));
	Aen::Animation& skelWalk = Aen::Resource::CreateAnimation("Skel_Walk");
	skelWalk.LoadAnimation(AEN_MODEL_DIR("Walk_skelTest3.fbx"));
	Aen::Animation& skelAttack = Aen::Resource::CreateAnimation("Skel_Attack");
	skelAttack.LoadAnimation(AEN_MODEL_DIR("Attack_skelTest3.fbx"));

	// ----------------------------- Load Meshes -------------------------------- //

	Aen::Mesh& rimuru = Aen::Resource::CreateMesh("Rimuru");
	rimuru.Load(AEN_MODEL_DIR("Slime.fbx"));
	Aen::Mesh& skeleLight = Aen::Resource::CreateMesh("SkeletonLight");
	skeleLight.Load(AEN_MODEL_DIR("Skel_Meshtest.fbx"));

	// -------------------------- Setup Material -------------------------------- //

	Aen::Material& planeMat = Aen::Resource::CreateMaterial("PlaneMaterial");
	Aen::Material& slimeMat = Aen::Resource::CreateMaterial("SlimeMaterial");
	Aen::Material& skeleLightMat = Aen::Resource::CreateMaterial("SkeleLightMaterial");
	Aen::Material& enemyMatHurt = Aen::Resource::CreateMaterial("EnemyMaterialHurt");

	Aen::Material& psMat = Aen::Resource::CreateMaterial("PSMaterial");
	psMat.LoadeAndSetDiffuseMap(AEN_TEXTURE_DIR("Flames.png"));
	psMat.LoadeAndSetOpacityMap(AEN_TEXTURE_DIR("FlamesOppacity1.png"));

	/*enemyMat.LoadeAndSetDiffuseMap(AEN_TEXTURE_DIR("SlimeRimuruFace.png"));
	enemyMat["InnerEdgeColor"] = Aen::Color::Cyan;
	enemyMat["OuterEdgeColor"] = Aen::Color::Cyan;
	enemyMat["BaseColor"] = Aen::Color::Cyan;*/
	slimeMat.LoadeAndSetDiffuseMap(AEN_TEXTURE_DIR("SlimeRimuruFace.png"));
	slimeMat["InnerEdgeColor"] = Aen::Color::Cyan;
	slimeMat["OuterEdgeColor"] = Aen::Color::Cyan;
	slimeMat["BaseColor"] = Aen::Color::Cyan;

	skeleLightMat["InnerEdgeColor"] = Aen::Color::Black;
	skeleLightMat["OuterEdgeColor"] = Aen::Color::Black;
	skeleLightMat["BaseColor"] = Aen::Color::White;

	// Material to switch to when enemy is hurt
	enemyMatHurt.LoadeAndSetEmissionMap(AEN_TEXTURE_DIR("White.png"));
	enemyMatHurt["BaseColor"] = Aen::Color::Red;
	enemyMatHurt["GlowColor"] = Aen::Color::Red;

	planeMat.LoadeAndSetDiffuseMap(AEN_TEXTURE_DIR("Floor_Diffuse.png"));
	planeMat["InnerEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);
	planeMat["OuterEdgeColor"] = Aen::Color(0.2f, 0.26f, 0.37f, 1.f);
	// -------------------------- Setup Entities -------------------------------- //
	// 
	// -------------------------- Particle System -------------------------------- //

	//Comment if you want the engine to work, big problem here

	//m_PS = &Aen::EntityHandler::CreateEntity();
	//m_PS->AddComponent<Aen::PSSystemcomponent>();
	//m_PS->GetComponent<Aen::PSSystemcomponent>().Initialize();
	///*m_PS->GetComponent<Aen::PSSystemcomponent>().SetRespawnHeight(10);*/
	//m_PS->GetComponent<Aen::PSSystemcomponent>().SetEmitPos(0,0,0);
	//m_PS->GetComponent<Aen::PSSystemcomponent>().SetNrOfPS(5);
	//m_PS->GetComponent<Aen::PSSystemcomponent>().SetMaterial(psMat);


	m_plane = &Aen::EntityHandler::CreateEntity();
	m_plane->AddComponent<Aen::StaticBody>();
	m_plane->GetComponent<Aen::StaticBody>().SetGeometry(Aen::StaticGeometryType::PLANE);

	// ------------------- Procedural generation testing staging grounds ------- //
	m_levelGenerator.LoadMutipleRoomFiles();

	m_levelGenerator.AddLoadedToGeneration();

	m_levelGenerator.SetMapTheme(Aen::RoomTheme::GENERIC);

	//Match this value to the size of the rooms we are using
	m_levelGenerator.SetRoomDimension(43.f);
	mptr_map = m_levelGenerator.GenerateLevel();
	m_levelGenerator.GenerationTestingFunction();
	m_levelGenerator.CleanMap();

	//Use this value to set the start of the player / origin of the map
	Aen::Vec3f playerStartPos(0.f, 0.f, 0.f);
	Aen::Vec3f ChestPos;
	Aen::Vec3f DoorPos;
	Aen::Vec3f EnemyPos;
	int roomNormal = 0;
	int itemNormal = 0;

	for (UINT y = 0; y < Aen::mapSize; y++) {
		for (UINT x = 0; x < Aen::mapSize; x++) {
			m_levelGenerator.SpawnRoom(Aen::Vec2i(x, y));

			if (mptr_map[y * Aen::mapSize + x].m_roomSpecial == Aen::SpecialRoom::ENTRANCE) {
				m_levelGenerator.GetRoomPos(x, y, &playerStartPos.x, &playerStartPos.z);

			}
			mptr_map[x + y * Aen::mapSize].mptr_parent;

			if (mptr_map[y * Aen::mapSize + x].m_roomSpecial == Aen::SpecialRoom::BOSS) 
			{
				m_levelGenerator.GetRoomPos(x, y, &m_bossPos.x, &m_bossPos.z);
				//m_levelGenerator.GetRoomPos(x, y, &playerStartPos.x, &playerStartPos.z);
			}
			mptr_map[x + y * Aen::mapSize].mptr_parent;

			if (mptr_map[y * Aen::mapSize + x].m_roomSpecial == Aen::SpecialRoom::ITEM) {
				itemNormal = mptr_map[y * Aen::mapSize + x].connectionDirections;
				m_levelGenerator.GetRoomPos(x, y, &ChestPos.x, &ChestPos.z);
			}

			if (mptr_map[y * Aen::mapSize + x].m_roomSpecial == Aen::SpecialRoom::BOSS) {

				m_levelGenerator.GetRoomPos(x, y, &DoorPos.x, &DoorPos.z);
				roomNormal = mptr_map[y * Aen::mapSize + x].connectionDirections;
				for (int i = 0; i < 10; i++) {
					m_enemyQueue.emplace_back(AEN_NEW Rimuru(EnemyPos));
				}
			}
		}
	}
	m_chest.GetEntity()->SetPos(ChestPos);
	for (size_t i = 0; i < m_levelGenerator.GetHandlerPtr()->GetParticleList().size(); i++)
	{
		AenIF::Particle* particle = &m_levelGenerator.GetHandlerPtr()->GetParticleList()[i];

		m_PS = &Aen::EntityHandler::CreateEntity();
		m_PS->AddComponent<Aen::PSSystemcomponent>();
		m_PS->GetComponent<Aen::PSSystemcomponent>().Initialize();
		m_PS->GetComponent<Aen::PSSystemcomponent>().SetHeightLimit(6.0f);
		m_PS->GetComponent<Aen::PSSystemcomponent>().SetEmitPos(particle->translation[0], particle->translation[1], particle->translation[2]);
		m_PS->GetComponent<Aen::PSSystemcomponent>().SetVelocity(0,0,0);
		m_PS->GetComponent<Aen::PSSystemcomponent>().SetNrOfPS(1);
		m_PS->GetComponent<Aen::PSSystemcomponent>().SetMaterial(psMat);
	}
	
	

	//PS system pos
	//m_PS->SetPos(ChestPos.x + 10.f, ChestPos.y + 5.f, ChestPos.z);
	//m_PS->GetComponent<Aen::PSSystemcomponent>().SetHeightLimit(ChestPos.y + 10.f);
	//m_PS->GetComponent<Aen::PSSystemcomponent>().SetEmitPos(ChestPos.x, ChestPos.y, ChestPos.z);

	m_chest.SetType(Type::Open);

	//m_door.SetType(Type::Open);
	//m_player.GetEntity()->SetPos(m_bossPos.x, m_bossPos.y + 5.f, m_bossPos.z);
	//m_player.GetEntity()->SetPos(ChestPos.x + 10.f, ChestPos.y + 5.f, ChestPos.z);

	m_player.GetEntity()->SetPos(playerStartPos.x, playerStartPos.y + 5.f, playerStartPos.z);

	m_chest.SetType(Type::Open);
	m_door.SetType(Type::Closed);

	if (roomNormal == 1) { //north
		m_door.GetEntity()->SetRot(0, 180, 0);
	}
	else if (roomNormal == 10) {//east
		m_door.GetEntity()->SetRot(0, 90, 0);
	}
	else if (roomNormal == 100) {//south
		m_door.GetEntity()->SetRot(0, 0, 0);
	}
	else if (roomNormal == 1000) {//west
		m_door.GetEntity()->SetRot(0, -90, 0);
	}

	if (itemNormal == 1) { //north
		m_chest.GetEntity()->SetRot(0, 0, 0);
	}
	else if (itemNormal == 10) {//east
		m_chest.GetEntity()->SetRot(0, -90, 0);
	}
	else if (itemNormal == 100) {//south
		m_chest.GetEntity()->SetRot(0, 180, 0);
	}
	else if (itemNormal == 1000) {//west
		m_chest.GetEntity()->SetRot(0, 90, 0);
	}
	m_door.GetEntity()->SetPos(DoorPos.x, 3.2f, DoorPos.z);
	m_door.GetEntity()->MoveRelative(0.f, 0, 21.5f);

	//m_attack->SetParent(*m_player);
	//printf("");
	// 
	//---------ENEMIES----------//
	// ALWAYS SPAWN BOSS BEFORE OTHER ENEMIES!!!!!

	m_enemyQueue.emplace_back(AEN_NEW Boss(m_bossPos, 200.f));
	m_pSkeleBoss = dynamic_cast<Boss*>(m_enemyQueue[m_enemyQueue.size() - 1]);
	m_player.AddBossesAlive(1);
	m_pSkeleBoss->GetEntity()->SetPos(m_bossPos);
	m_player.SetBossP(m_pSkeleBoss);

	std::vector<Aen::Vec3f> tempSlimes = m_levelGenerator.GetHandlerPtr()->GetEnemyPos();
	std::vector<Aen::Vec3f> tempLskels = m_levelGenerator.GetHandlerPtr()->GetLskelPos();
	for (size_t i = 0; i < m_levelGenerator.GetHandlerPtr()->GetEnemyPos().size(); i++)
	{
		m_enemyQueue.emplace_back(AEN_NEW Rimuru(tempSlimes[i]));
	}
	for (size_t i = 0; i < m_levelGenerator.GetHandlerPtr()->GetLskelPos().size(); i++)
	{
		m_enemyQueue.emplace_back(AEN_NEW SkeleLight(tempLskels[i]));
	}


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
	screenSize.x = wDesc.width;
	screenSize.y = wDesc.height;

	// -----------------------------	UI	------------------------------- //
	m_UI = &Aen::EntityHandler::CreateEntity();
	m_UI->AddComponent<Aen::UIComponent>();
	mp_uiComp = &m_UI->GetComponent<Aen::UIComponent>();
	mp_uiComp->AddPicture(AEN_TEXTURE_DIR_W(L"healthbar.png")); //0
	mp_uiComp->SetPicPos((350.f / 1920)* wDesc.width, (100.f / 1024) * wDesc.height);
	mp_uiComp->SetPicSize(((m_hp * 2.f) / 1920) * wDesc.width, (150.f / 1024) * wDesc.height);

	mp_uiComp->AddPicture(AEN_TEXTURE_DIR_W(L"bar.png")); //1
	mp_uiComp->SetPicPos((350.f / 1920) * wDesc.width, (100.f / 1024) * wDesc.height);
	mp_uiComp->SetPicSize(((m_hp * 2.f) / 1920) * wDesc.width, (150.f / 1024) * wDesc.height);

	mp_uiComp->AddPicture(AEN_TEXTURE_DIR_W(L"potion.png")); //2
	mp_uiComp->SetPicPos((125.f / 1920) * wDesc.width, (100.f / 1024) * wDesc.height);
	mp_uiComp->SetPicSize((150.f / 1920) * wDesc.width, (150.f / 1024) * wDesc.height);

	//m_UI->GetComponent<Aen::UIComponent>().AddPicture(AEN_RESOURCE_DIR_W(L"GoalText.png"), 1);
	//m_UI->GetComponent<Aen::UIComponent>().SetPicPos(965.f, 100.f, 1);
	//m_UI->GetComponent<Aen::UIComponent>().SetPicSize(600.f, 100.f, 1);

	mp_uiComp->AddText(L"- Find the boss", 30.f); //0
	mp_uiComp->SetTextPos((175.f / 1920) * wDesc.width, (300.f / 1024) * wDesc.height);
	mp_uiComp->SetTextSize((900.f / 1920) * wDesc.width, (300 / 1024) * wDesc.height);

	mp_uiComp->AddText(L"3", 50.f); //1 - Amount of potion
	mp_uiComp->SetTextPos((120.f / 1920) * wDesc.width, (110.f / 1024) * wDesc.height);
	mp_uiComp->SetTextSize((150.f / 1920) * wDesc.width, (150.f / 1024) * wDesc.height);
	mp_uiComp->SetColor(D2D1::ColorF::Black);

	mp_uiComp->AddText(L"Interact (F)", 60.f); //2
	mp_uiComp->SetTextPos((965.f / 1920.f) * wDesc.width, (800.f / 1024.f) * wDesc.height);
	mp_uiComp->SetTextSize((900.f / 1920.f) * wDesc.width, (300.f / 1024.f) * wDesc.height);
	mp_uiComp->SetColor(D2D1::ColorF::Aqua);

	mp_uiComp->AddText(L"- Find Item Room (Optional)", 30.f); //3
	mp_uiComp->SetTextPos((200.f / 1920) * wDesc.width, (350.f / 1024) * wDesc.height);
	mp_uiComp->SetTextSize((900.f / 1920) * wDesc.width, (300 / 1024) * wDesc.height);

	//Pause menu UI
	mp_uiComp->AddPicture(AEN_TEXTURE_DIR_W(L"PauseOverlay.png")); //3
	mp_uiComp->SetPicPos((1.f / 2.f)* wDesc.width, (1.f / 2.f)* wDesc.height);
	mp_uiComp->SetPicSize(wDesc.width, wDesc.height);

	mp_uiComp->AddButton(AEN_TEXTURE_DIR_W(L"Continue.png")); //0
	mp_uiComp->SetButtonPos((1.f / 6.f)* wDesc.width, (800.f / 1024.f)* wDesc.height);
	mp_uiComp->SetButtonSize((200.f / 1920.f) * wDesc.width, (75.f / 1024.f) * wDesc.height);

	mp_uiComp->AddButton(AEN_TEXTURE_DIR_W(L"Quit.png")); //1
	mp_uiComp->SetButtonPos((1.f / 6.f)* wDesc.width, (900 / 1024.f)* wDesc.height);
	mp_uiComp->SetButtonSize((200.f / 1920.f)* wDesc.width, (75.f / 1024.f)* wDesc.height);

	mp_uiComp->SaveButtonData();

	mp_uiComp->SetPicSize(0, 0, 3);
	mp_uiComp->SetButtonSize(0, 0, 0);
	mp_uiComp->SetButtonSize(0, 0, 1);


	Aen::Input::ToggleRawMouse(true);
	Aen::Input::SetMouseVisible(false);
}

// ---------------------------------------------------------		Update		--------------------------------------------------------------- //

void Gameplay::Update(const float& deltaTime) {

	if (Aen::Input::KeyDown(Aen::Key::ESCAPE)) {
		m_paused = !m_paused;
		Aen::Input::SetMouseVisible(m_paused);
		if (m_paused) {
			mp_uiComp->SetPicSize(screenSize.x, screenSize.y, 3);
			mp_uiComp->SetTextPos(-100, 0, 0);
			mp_uiComp->SetTextPos(-100, 0, 1);
			mp_uiComp->SetTextPos(-100, 0, 3);
		}
		else{
			mp_uiComp->SetPicSize(0, 0, 3);
			mp_uiComp->SetButtonSize(0, 0, 0);
			mp_uiComp->SetButtonSize(0, 0, 1);
			mp_uiComp->SetTextPos((175.f / 1920) * screenSize.x, (300.f / 1024) * screenSize.y, 0);
			mp_uiComp->SetTextSize((900.f / 1920) * screenSize.x, (300 / 1024) * screenSize.y, 0);
			mp_uiComp->SetTextPos((120.f / 1920) * screenSize.x, (110.f / 1024) * screenSize.y, 1);
			mp_uiComp->SetTextSize((150.f / 1920) * screenSize.x, (150.f / 1024) * screenSize.y, 1);
			mp_uiComp->SetTextPos((200.f / 1920) * screenSize.x, (350.f / 1024) * screenSize.y, 3);
			mp_uiComp->SetTextSize((900.f / 1920) * screenSize.x, (300 / 1024) * screenSize.y, 3);

		}
		//State::SetState(States::Loadscreen);
		//m_Window.Exit();
	}

	if (m_paused) {
		mp_uiComp->Update();
		if (Aen::Input::KeyDown(Aen::Key::LMOUSE) && mp_uiComp->Intersects(0))
		{
			m_paused = false;
			Aen::Input::SetMouseVisible(false);
			mp_uiComp->SetPicSize(0, 0, 3);
			mp_uiComp->SetButtonSize(0, 0, 0);
			mp_uiComp->SetButtonSize(0, 0, 1);
			mp_uiComp->SetTextPos((175.f / 1920) * screenSize.x, (300.f / 1024) * screenSize.y, 0);
			mp_uiComp->SetTextSize((900.f / 1920) * screenSize.x, (300 / 1024) * screenSize.y, 0);
			mp_uiComp->SetTextPos((120.f / 1920) * screenSize.x, (110.f / 1024) * screenSize.y, 1);
			mp_uiComp->SetTextSize((150.f / 1920) * screenSize.x, (150.f / 1024) * screenSize.y, 1);
			mp_uiComp->SetTextPos((200.f / 1920) * screenSize.x, (350.f / 1024) * screenSize.y, 3);
			mp_uiComp->SetTextSize((900.f / 1920) * screenSize.x, (300 / 1024) * screenSize.y, 3);
		}
		if (Aen::Input::KeyDown(Aen::Key::LMOUSE) && mp_uiComp->Intersects(1))
		{
			State::SetState(States::Gameover);
		}
		return;
	}

	if (m_hp != m_player.GetHealth()) { //ers�tt collision med enemy i if satsen
		mp_uiComp->UpdatePicture(((m_hp - m_player.GetHealth()) * 2.f) * (1.f/1920.f) * screenSize.x, 0);
		m_hp = m_player.GetHealth();
	}
	static int pots;
	if (pots != m_player.GetPotionNr()) {
		mp_uiComp->ChangeText(1, std::to_wstring(m_player.GetPotionNr()).c_str());
		pots = m_player.GetPotionNr();
	}
	//cout << "hp: " << m_hp << "		player: " << m_player.GetHealth() << endl;

	m_player.PotionUpdate();

	if (m_toggleFullScreen)
		Aen::Input::SetMousePos((Aen::Vec2i)Aen::Vec2f(GetSystemMetrics(SM_CXSCREEN) * 0.5f, GetSystemMetrics(SM_CYSCREEN) * 0.5f));
	else
		Aen::Input::SetMousePos(m_Window.GetWindowPos() + (Aen::Vec2i)((Aen::Vec2f)m_Window.GetSize() * 0.5f));

	// ---------------------------------- Enemies --------------------------------------- //

	m_player.Update(m_enemyQueue, deltaTime);

	m_chest.Update(deltaTime, m_player.GetEntity());
	m_door.Update(deltaTime, m_player.GetEntity());
	if (m_chest.GetNear() || m_door.GetNear()) {
		mp_uiComp->SetTextPos((965.f / 1920.f) * screenSize.x, (800.f / 1024.f) * screenSize.y, 2);
		mp_uiComp->SetTextSize((900.f / 1920.f) * screenSize.x, (300.f / 1024.f) * screenSize.y, 2);

		if (Aen::Input::KeyDown(Aen::Key::F) && m_chest.GetType() == Type::Open && m_chest.GetNear()) {
			m_player.IncreaseHealthCap();
			m_chest.SetType(Type::Locked);
			mp_uiComp->ChangeText(2, L"Health Potions Restored");
		}
		else if (Aen::Input::KeyDown(Aen::Key::F) && m_chest.GetType() == Type::Locked && m_chest.GetNear()) {
			mp_uiComp->ChangeText(2, L"Can't Get More health potions");
		}

		if (m_door.GetNear())
			mp_uiComp->ChangeText(2, L"Interact(F)");

		if (Aen::Input::KeyDown(Aen::Key::F) && m_door.GetType() == Type::Closed && m_door.GetNear()) {
			m_door.SetType(Type::Opening);
		}
	}
	else {
		mp_uiComp->SetTextPos(-100.f, -100.f, 2);
	}

	for (auto& i : m_enemyQueue) {
		i->Update(deltaTime, m_player);
	}

	m_player.UpdateAttack(m_enemyQueue, deltaTime);

	int enemiesToSummon = 0;
	if (m_player.GetBossesAlive() > 0)
	{
		if (m_pSkeleBoss->GetBS() != BossState::STATIONARY && m_door.GetType() == Type::Open) {
			mp_uiComp->ChangeText(0, L"- Kill the Boss");
			m_door.SetType(Type::Locking);
		}

		Aen::Vec3f minionOffset(-8.f,0,8.f);
		enemiesToSummon = m_pSkeleBoss->GetEnemiesToSummon();
		for (int i = 0; i < enemiesToSummon; i++)
		{
			minionOffset.z *= -1;
			Rimuru* bossMinion = AEN_NEW Rimuru(m_bossPos + minionOffset, EnemyType::MINION);
			minionOffset += Aen::Vec3f(3.f, 0, 0);
			m_pSkeleBoss->EmplaceMinion(bossMinion);
			m_enemyQueue.emplace_back(bossMinion);
		}

	}
	else enemiesToSummon = 0;


	if (m_hp <= 0.f) {
		SetWin(false);
		mp_uiComp->SetPicPos(0.f, 0.f, 0);
		m_deathTimer += deltaTime;

		if (m_deathTimer > 0.2f) {
			State::SetState(States::Gameover);
		}
	}
	if (m_player.GetBossesAlive() <= 0.f)
	{
		SetWin(true);
		State::SetState(States::Gameover);
	}
	// ------------------------------ Toggle Fullscreen --------------------------------- //

	if (Aen::Input::KeyDown(Aen::Key::F1)) {
		m_toggleFullScreen = !m_toggleFullScreen;
		Aen::WindowDesc wDesc;

		if (m_toggleFullScreen) {
			wDesc.width = GetSystemMetrics(SM_CXSCREEN) + 4u;
			wDesc.height = GetSystemMetrics(SM_CYSCREEN) + 4u;
			screenSize.x = wDesc.width;
			screenSize.y = wDesc.height;
			wDesc.EXStyle = AEN_WS_EX_APPWINDOW;
			wDesc.style = AEN_WS_POPUPWINDOW | AEN_WS_VISIBLE;
			m_Window.LoadSettings(wDesc);
		}
		else {
			wDesc.width = static_cast<UINT>(GetSystemMetrics(SM_CXSCREEN) * 0.4f);
			wDesc.height = static_cast<UINT>(GetSystemMetrics(SM_CYSCREEN) * 0.4f);
			screenSize.x = wDesc.width;
			screenSize.y = wDesc.height;
			wDesc.EXStyle = AEN_WS_EX_APPWINDOW;
			wDesc.style = AEN_WS_OVERLAPPEDWINDOW | AEN_WS_VISIBLE;
			m_Window.LoadSettings(wDesc);
		}
	}

	// ------------------------------ Quick Exit Button -------------------------------- //

	// ------------------------------------- States -------------------------------------- //
	//if (m_hp <= 0 && m_enemyQueue.size() == 0)
	//{
	//	State::SetState(States::Gameover);
	//}
}