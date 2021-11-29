#include"Player.h"
#include "Enemy/Enemy.h"

Player::Player()
	:m_player(&Aen::EntityHandler::CreateEntity()), m_camera(&Aen::EntityHandler::CreateEntity()),
	m_hurtbox(&Aen::EntityHandler::CreateEntity()), m_health(200.f), m_potion(15.f), m_nrPotion(5),
	//m_sword(&Aen::EntityHandler::CreateEntity()),
	m_mouseSense(5.f), m_movementSpeed(8.f), m_finalDir(0.f, 0.f, -1.f),
	m_LIGHTATTACKTIME(.3f), m_HEAVYATTACKTIME(1.f), m_attackTimer(0.f),
	m_LIGHTCHARGETIME(0.f), m_HEAVYCHARGETIME(.5f),
	m_LIGHTATTACKSPEED(6.0f), m_HEAVYATTACKSPEED(2.54f)
{
	m_camera = &Aen::EntityHandler::CreateEntity();
	m_camera->AddComponent<Aen::Camera>();
	m_camera->GetComponent<Aen::Camera>().SetCameraPerspective(70.f, Aen::GlobalSettings::GetWindow()->GetAspectRatio(), 0.01f, 100.f);

	Aen::GlobalSettings::SetMainCamera(*m_camera);

	Aen::Mesh& sword = Aen::Resource::CreateMesh("Sword");
	sword.Load(AEN_MODEL_DIR("SwordOffset.fbx"));

	Aen::Mesh& capsule = Aen::Resource::CreateMesh("Capsule");
	capsule.Load(AEN_MODEL_DIR("Player.fbx"));

	Aen::Mesh* protag = &Aen::Resource::CreateMesh("Protag");
	protag->Load(AEN_MODEL_DIR("Protagonist.fbx"));

	Aen::Material& skin = Aen::Resource::CreateMaterial("Skin");
	skin["InnerEdgeColor"] = Aen::Color(0.3f, 0.1f, 0.08f, 1.f);
	skin["OuterEdgeColor"] = Aen::Color(0.13f, 0.1f, 0.08f, 1.f);
	skin["InnerEdgeThickness"] = 1;
	skin["OuterEdgeThickness"] = 2;
	skin["BaseColor"] = Aen::Color(0.8f, 0.67f, 0.6f, 1.f);
	skin["SpecularColor"] = Aen::Color::Red;
	skin["RimLightColor"] = Aen::Color(0.5f, 0.f, 0.f, 1.f);
	skin["RimLightIntensity"] = 0.8f;
	skin["RimLightSize"] = 0.4f;

	Aen::Material& shirt = Aen::Resource::CreateMaterial("Shirt");
	shirt["InnerEdgeColor"] = Aen::Color(0.1f, 0.08f, 0.05f, 1.f);
	shirt["OuterEdgeColor"] = Aen::Color(0.1f, 0.08f, 0.05f, 1.f);
	shirt["InnerEdgeThickness"] = 1;
	shirt["OuterEdgeThickness"] = 2;
	shirt["BaseColor"] = Aen::Color(0.78f, 0.77f, 0.69f, 1.f);
	shirt["RimLightColor"] = Aen::Color(0.5f, 0.f, 0.f, 1.f);
	shirt["RimLightIntensity"] = 0.8f;
	shirt["RimLightSize"] = 0.3f;

	Aen::Material& brown = Aen::Resource::CreateMaterial("Brown");
	brown["InnerEdgeColor"] = Aen::Color(0.13f, 0.014f, 0.012f, 1.f);
	brown["OuterEdgeColor"] = Aen::Color(0.032f, 0.017f, 0.012f, 1.f);
	brown["InnerEdgeThickness"] = 1;
	brown["OuterEdgeThickness"] = 2;
	brown["BaseColor"] = Aen::Color(0.23f, 0.17f, 0.16f, 1.f);
	brown["RimLightColor"] = Aen::Color(0.8f, 0.2f, 0.1f, 1.f);
	brown["RimLightIntensity"] = 1.f;
	brown["RimLightSize"] = 0.6f;

	Aen::Material& pants = Aen::Resource::CreateMaterial("Pants");
	pants["InnerEdgeColor"] = Aen::Color(0.06f, 0.07f, 0.07f, 1.f);
	pants["OuterEdgeColor"] = Aen::Color(0.06f, 0.07f, 0.07f, 1.f);
	pants["InnerEdgeThickness"] = 1;
	pants["OuterEdgeThickness"] = 2;
	pants["BaseColor"] = Aen::Color(0.44f, 0.41f, 0.34f, 1.f);

	Aen::Material& metal = Aen::Resource::CreateMaterial("Metal");
	metal["InnerEdgeColor"] = Aen::Color(0.04f, 0.04f, 0.07f, 1.f);
	metal["OuterEdgeColor"] = Aen::Color(0.04f, 0.04f, 0.07f, 1.f);
	metal["InnerEdgeThickness"] = 1;
	metal["OuterEdgeThickness"] = 2;
	metal["BaseColor"] = Aen::Color(0.32f, 0.36f, 0.4f, 1.f);
	metal["Roughness"] = 0.f;

	Aen::Material& shadow = Aen::Resource::CreateMaterial("Shadow");
	shadow["InnerEdgeColor"] = Aen::Color(0.1f, 0.03f, 0.01f, 1.f);
	shadow["OuterEdgeColor"] = Aen::Color(0.13f, 0.1f, 0.08f, 1.f);
	shadow["InnerEdgeThickness"] = 1;
	shadow["OuterEdgeThickness"] = 1;
	shadow["BaseColor"] = Aen::Color(0.8f, 0.67f, 0.6f, 1.f);
	shadow["ShadowColor"] = Aen::Color(0.3f, 0.2f, 0.2f, 1.f);
	shadow["ShadowOffset"] = 1.f;
	shadow["SpecularColor"] = Aen::Color::Red;

	Aen::Material& playerMat = Aen::Resource::CreateMaterial("PlayerMaterial");
	//Aen::Material& swordMat = Aen::Resource::CreateMaterial("SwordMaterial");

	m_player->AddComponent<Aen::CharacterController>();
	m_player->GetComponent<Aen::CharacterController>().Resize(2.3f);
	m_player->AddComponent<Aen::MeshInstance>();
	m_player->GetComponent<Aen::MeshInstance>().SetMesh(*protag);
	m_player->GetComponent<Aen::MeshInstance>().SetMaterial("Skin1", skin);
	m_player->GetComponent<Aen::MeshInstance>().SetMaterial("Shirt1", shirt);
	m_player->GetComponent<Aen::MeshInstance>().SetMaterial("Brown1", brown);
	m_player->GetComponent<Aen::MeshInstance>().SetMaterial("Pants1", pants);
	m_player->GetComponent<Aen::MeshInstance>().SetMaterial("Metal1", metal);
	m_player->GetComponent<Aen::MeshInstance>().SetMaterial("Shadow1", shadow);
	m_player->AddComponent<Aen::AABoundBox>();
	m_player->GetComponent<Aen::AABoundBox>().SetBoundsToMesh();

	/*m_sword->AddComponent<Aen::MeshInstance>();
	m_sword->GetComponent<Aen::MeshInstance>().SetMesh(sword);
	m_sword->GetComponent<Aen::MeshInstance>().SetMaterial(swordMat);
	m_sword->SetParent(*m_player);*/

	m_hurtbox->AddComponent<Aen::OBBox>();
	m_hurtbox->GetComponent<Aen::OBBox>().SetBoundingBox(1.f, 1.f, 1.0);
	m_hurtbox->GetComponent<Aen::OBBox>().SetOffset(0.f, 0.f, 0.f);
	m_hurtbox->GetComponent<Aen::OBBox>().ToggleActive(false);


	Aen::Mesh& eBar = Aen::Resource::CreateMesh("eBar");
	eBar.Load(AEN_MODEL_DIR("bar.fbx"));

	Aen::Material& barMat = Aen::Resource::CreateMaterial("barMat");
	Aen::Material& targetMat = Aen::Resource::CreateMaterial("targetMat");

	targetMat.LoadeAndSetDiffuseMap(AEN_TEXTURE_DIR("target1.png"));
	targetMat.LoadeAndSetOpacityMap(AEN_TEXTURE_DIR("target1.png"));
	targetMat.LoadeAndSetEmissionMap(AEN_TEXTURE_DIR("target1.png"));
	targetMat["InnerEdgeThickness"] = 0;
	targetMat["OuterEdgeColor"] = Aen::Color::Red;
	targetMat["GlowColor"] = Aen::Color::Red;

	barMat.LoadeAndSetDiffuseMap(AEN_TEXTURE_DIR("enemybar.png"));
	barMat.LoadeAndSetOpacityMap(AEN_TEXTURE_DIR("opacBar.png"));
	barMat.LoadeAndSetEmissionMap(AEN_TEXTURE_DIR("enemybar.png"));
	//barMat["InnerEdgeThickness"] = 0;
	barMat["GlowColor"] = Aen::Color::Red;
	barMat["InnerEdgeColor"] = Aen::Color::Red;
	barMat["OuterEdgeColor"] = Aen::Color::Red;

	m_targetUI = &Aen::EntityHandler::CreateEntity();
	m_targetUI->AddComponent<Aen::MeshInstance>();
	m_targetUI->GetComponent<Aen::MeshInstance>().SetMesh("eBar");
	m_targetUI->GetComponent<Aen::MeshInstance>().SetMaterial("targetMat");
	m_targetUI->SetScale(0, 0, 0);
	m_targetUI->SetRenderLayer(2);
}

Player::~Player() {
	Aen::GlobalSettings::RemoveMainCamera();
	Aen::EntityHandler::RemoveEntity(*m_player);
	Aen::EntityHandler::RemoveEntity(*m_camera);
	//m_sword->RemoveParent();
	//Aen::EntityHandler::RemoveEntity(*m_sword);
	Aen::EntityHandler::RemoveEntity(*m_hurtbox);
	Aen::EntityHandler::RemoveEntity(*m_targetUI);
}

void Player::Update(std::deque<Enemy*>& e, const float& deltaTime) {

	static Aen::Vec3f axis;
	static Aen::Vec3f targetDir(0.f, 0.f, -1.f);
	static bool lockedOn = false;

	static Aen::Vec2f dir;
	static Aen::Vec3f camDir;
	static Aen::Vec2f side;
	if (lockedOn)
		side.x = Aen::Lerp(side.x, axis.x, 0.05f);
	else
		side.x = Aen::Lerp(side.x, axis.x * 0.3f, 0.05f);
	side.y = Aen::Lerp(side.y, axis.z, 0.15f);



	// --------------------------- Raw Mouse and scroll Input --------------------------- //

	while (!Aen::Input::MouseBufferIsEmbty())
	{
		Aen::MouseEvent me = Aen::Input::ReadEvent();

		if (me.getInputType() == Aen::MouseEvent::MouseInput::RAW_MOVE)
		{
			if (!Aen::Input::GPGetActive(0u) && !lockedOn) {
				m_camera->Rotate(
					-(float)me.GetPos().y * m_mouseSense * deltaTime,
					(float)me.GetPos().x * m_mouseSense * deltaTime, 0.f);
			}
		}
		if (me.getInputType() == Aen::MouseEvent::MouseInput::SCROLL_UP) {
			printf("scroll up\n");

		}
		else if (me.getInputType() == Aen::MouseEvent::MouseInput::SCROLL_DOWN) {
			printf("scroll down\n");

		}
	}
	// ------------------------------		Health potion		---------------------------------- //
	
	if (Aen::Input::KeyDown(Aen::Key::NUM1) && m_nrPotion > 0 && m_health < 200.f) {
	
		m_health += m_potion;
		m_nrPotion--;

		if (m_health > 200.f) // cap
			m_health = 200.f;
	}

	// ------------------------------ Player Controler ---------------------------------- //

	if (Aen::Input::GPGetActive(0u)) {
		axis.x = Aen::Input::GPGetAnalog(0u, Aen::Analog::LTHUMB).x;
		axis.z = Aen::Input::GPGetAnalog(0u, Aen::Analog::LTHUMB).y;

		m_camera->Rotate(
			Aen::Input::GPGetAnalog(0u, Aen::Analog::RTHUMB).y * m_mouseSense * 18.f * deltaTime,
			Aen::Input::GPGetAnalog(0u, Aen::Analog::RTHUMB).x * m_mouseSense * 28.f * deltaTime, 0.f);

		// Dash/Dodge

		static bool toggle = false;
		static bool lTriggerPressed = false;

		if (Aen::Input::GPGetAnalog(0u, Aen::Analog::TRIGGER).x > 0.f && !toggle) {
			lTriggerPressed = true;
			toggle = true;
		}
		else if (Aen::Input::GPGetAnalog(0u, Aen::Analog::TRIGGER).x <= 0.f)
			toggle = false;

		if (lTriggerPressed) {
			lTriggerPressed = false;
			EventData data;
			data.accell = 20.f;
			data.duration = 0.3f;
			data.type = EventType::Dash;
			data.function = [&](float& accell, const float& attackDuration) {
				m_player->GetComponent<Aen::CharacterController>().Move(m_finalDir * accell * deltaTime, deltaTime);
				accell -= 25.f * deltaTime;
			};

			AddEvent(data);
		}

		// Attack

		if (Aen::Input::GPKeyDown(0u, Aen::GP::A)) {
			EventData data;
			data.accell = 6.f;
			data.duration = 0.2f;
			data.type = EventType::Attack;
			data.damage = 20.f;
			data.function = [&](float& accell, const float& attackDuration) {
				if (lockedOn) {
					Aen::Vec2f d2(Aen::Vec2f(camDir.x, camDir.z).Normalized());
					Aen::Vec3f d(d2.x, 0.f, d2.y);
					m_finalDir = Aen::Lerp(m_finalDir, d, 0.6f);
				}

				m_player->GetComponent<Aen::CharacterController>().Move(m_finalDir * accell * deltaTime, deltaTime);
				accell -= 12.f * deltaTime;
			};

			AddEvent(data);
		}

		// Lock On Target

		if(Aen::Input::GPKeyDown(0u, Aen::GP::DPAD_LEFT))
			if(m_targets.size() > 1u && lockedOn) {
				m_targets.front().target->SetISTargeted(false);
				TargetData temp = m_targets.front();
				m_targets.pop_front();
				m_targets.front().target->SetISTargeted(true);
				m_targets.emplace_back(temp);
			}

		if(Aen::Input::GPKeyDown(0u, Aen::GP::DPAD_RIGHT))
			if(m_targets.size() > 1u && lockedOn) {
				TargetData temp = m_targets.back();
				m_targets.pop_back();
				m_targets.front().target->SetISTargeted(false);
				m_targets.emplace_front(temp);
				m_targets.front().target->SetISTargeted(true);
			}

		if (Aen::Input::GPKeyDown(0u, Aen::GP::LSHOULDER)) {
			lockedOn = !lockedOn;

			if (lockedOn) {
				for(auto i : e)
					i->SetISTargeted(false);

				m_targets.clear();
				for (auto i : e) {
					TargetData data;
					data.target = i;
					Aen::Vec3f eDir = m_player->GetPos() - data.target->GetEntity()->GetPos();
					data.distance = eDir.Magnitude();

					if(data.distance < 20.f)
						m_targets.emplace_back(data);
				}

				for(uint32_t i = 0u; i < m_targets.size(); i++) {

					uint32_t t(i);

					for(uint32_t k = i + 1u; k < m_targets.size(); k++)
						if(m_targets[t].distance > m_targets[k].distance)
							t = k;

					TargetData temp = m_targets[i];
					m_targets[i] = m_targets[t];
					m_targets[t] = temp;
				}

				if(!m_targets.empty())
					m_targets.front().target->SetISTargeted(true);
			}
		}

	}
	else {
		axis.x = (float)Aen::Input::KeyPress(Aen::Key::D) - (float)Aen::Input::KeyPress(Aen::Key::A);
		axis.z = (float)Aen::Input::KeyPress(Aen::Key::W) - (float)Aen::Input::KeyPress(Aen::Key::S);

		// Dash/Dodge

		if (Aen::Input::KeyDown(Aen::Key::SPACE)) {

			if(m_eventQueue.empty())
				if (axis.Magnitude() > 0.f) 
					m_finalDir = Aen::Vec3f(dir.Normalized().x, 0.f, dir.Normalized().y);
				else {
					Aen::Vec2f dir(camDir.x, camDir.z);
					m_finalDir = Aen::Vec3f(dir.Normalized().x, 0.f, dir.Normalized().y);
				}

			EventData data;
			data.accell = 20.f;
			data.duration = 0.4f;
			data.type = EventType::Dash;
			data.function = [&](float& accell, const float& attackDuration) {
				m_player->GetComponent<Aen::CharacterController>().Move(m_finalDir * accell * deltaTime, deltaTime);
				m_player->GetComponent<Aen::AABoundBox>().ToggleActive(false);
				accell -= 25.f * deltaTime;
			};

			AddEvent(data);
		}

		// Attack

		if (Aen::Input::KeyDown(Aen::Key::LMOUSE)) {

			if(m_eventQueue.empty()) {
				Aen::Vec2f dir(camDir.x, camDir.z);
				m_finalDir = Aen::Vec3f(dir.Normalized().x, 0.f, dir.Normalized().y);
			}

			EventData data;
			data.accell = m_LIGHTATTACKSPEED;
			data.duration = m_LIGHTATTACKTIME;
			data.type = EventType::Attack;
			data.damage = 20.f;
			data.function = [&](float& accell, const float& attackDuration) {
				m_hurtbox->GetComponent<Aen::OBBox>().ToggleActive(true);
				//SwordSwing(10.f, m_LIGHTATTACKTIME, deltaTime);
				if (lockedOn) {
					Aen::Vec2f d2(Aen::Vec2f(camDir.x, camDir.z).Normalized());
					Aen::Vec3f d(d2.x, 0.f, d2.y);
					m_finalDir = Aen::Lerp(m_finalDir, d, 0.6f);
				}
				m_player->GetComponent<Aen::CharacterController>().Move(m_finalDir * accell * deltaTime, deltaTime);
				accell -= 12.f * deltaTime;
			};

			AddEvent(data);
		}
		if (Aen::Input::KeyDown(Aen::Key::RMOUSE)) {
			EventData data;
			data.accell = m_HEAVYATTACKSPEED;
			data.duration = m_HEAVYATTACKTIME;
			data.type = EventType::Attack;
			data.damage = 40.f;
			data.function = [&](float& accell, const float& attackDuration)
			{

				if (lockedOn) {
					Aen::Vec2f d2(Aen::Vec2f(camDir.x, camDir.z).Normalized());
					Aen::Vec3f d(d2.x, 0.f, d2.y);
					m_finalDir = Aen::Lerp(m_finalDir, d, 0.6f);
				}

				m_player->GetComponent<Aen::CharacterController>().Move(m_finalDir * accell * deltaTime, deltaTime);
				accell -= deltaTime * 2;
				if (attackDuration < m_HEAVYCHARGETIME)
				{
					m_hurtbox->GetComponent<Aen::OBBox>().ToggleActive(true);
					//SwordSwing(5.f, m_HEAVYATTACKTIME, deltaTime);
				}
				else
					m_hurtbox->GetComponent<Aen::OBBox>().ToggleActive(false);
			};

			AddEvent(data);
		}

		// Lock On Target

		if(Aen::Input::KeyDown(Aen::Key::TAB))
			if(m_targets.size() > 1u && lockedOn) {
				m_targets.front().target->SetISTargeted(false);
				TargetData temp = m_targets.front();
				m_targets.pop_front();
				m_targets.front().target->SetISTargeted(true);
				m_targets.emplace_back(temp);
			}

		if (Aen::Input::KeyDown(Aen::Key::E)) {
			lockedOn = !lockedOn;

			if (lockedOn) {
				for(auto i : e)
					i->SetISTargeted(false);

				m_targets.clear();
				for (auto i : e) {
					TargetData data;
					data.target = i;
					Aen::Vec3f eDir = m_player->GetPos() - data.target->GetEntity()->GetPos();
					data.distance = eDir.Magnitude();
					
					if(data.distance < 20.f)
						m_targets.emplace_back(data);
				}

				for(uint32_t i = 0u; i < m_targets.size(); i++) {

					uint32_t t(i);

					for(uint32_t k = i + 1u; k < m_targets.size(); k++)
						if(m_targets[t].distance > m_targets[k].distance)
							t = k;

					TargetData temp = m_targets[i];
					m_targets[i] = m_targets[t];
					m_targets[t] = temp;
				}

				

				if(!m_targets.empty())
					m_targets.front().target->SetISTargeted(true);
			}
		}
	}

	if (!m_targets.empty() && lockedOn) { 
		Aen::Vec3f tDir = ((m_player->GetPos() + Aen::Vec3f(0.f, 1.f, 0.f)) - m_targets.front().target->GetEntity()->GetPos() + (camDir % Aen::Vec3f(0.f, 1.f, 0.f)).Normalized() * side.x).Normalized();
		float yaw = Aen::RadToDeg(std::atan2(tDir.x, tDir.z));
		float pitch = Aen::RadToDeg(std::acos(tDir * Aen::Vec3f(0.f, 1.f, 0.f))) - 90.f;
		//Create UI
		m_targetUI->SetPos(m_targets.front().target->GetEntity()->GetPos());
		m_targetUI->SetScale(5.f, 1.f,15.f);
		m_targetUI->SetRot(-GetCamera()->GetRot().x - 90.f, GetCamera()->GetRot().y + 180.f, 0);

		m_camera->SetRot(pitch, yaw, 0.f);
		Aen::Vec3f eDir = m_player->GetPos() - m_targets.front().target->GetEntity()->GetPos();
		if (eDir.Magnitude() > 20.f) lockedOn = false;
	} else {
		m_targetUI->SetScale(0, 0, 0);
		lockedOn = false;
		for(auto i : e)
			i->SetISTargeted(false);
	}
	
	float r = Aen::Clamp(m_camera->GetRot().x, -45.f, 45.f);
	m_camera->SetRot(r, m_camera->GetRot().y, m_camera->GetRot().z);
	camDir = Aen::Lerp(camDir, Aen::Transform(m_camera->GetComponent<Aen::Rotation>().GetTranform(), targetDir).Normalized(), 0.6f).Normalized();

	m_ray.SetOrigin(m_player->GetPos() - camDir * 0.8f);
	m_ray.SetDirection(-camDir);
	m_ray.SetMaxDist(5.f);
	m_ray.Update();

	m_camera->SetPos(Aen::Lerp(m_camera->GetPos(), m_player->GetPos() + Aen::Vec3f(0.f, 0.8f, 0.f) + camDir * (-m_ray.GetDistance() - side.y) + (camDir % Aen::Vec3f(0.f, 1.f, 0.f)).Normalized() * 1.25f * side.x, 0.6f));
	m_camera->GetComponent<Aen::Camera>().LookTowards(camDir);

	Aen::Vec3f playerDir = m_camera->GetComponent<Aen::Camera>().GetForward() * axis.Normalized().z + m_camera->GetComponent<Aen::Camera>().GetRight() * axis.Normalized().x;
	dir = Aen::Vec2f(playerDir.x, playerDir.z);

	Aen::Vec3f attackPos = m_player->GetPos() + m_finalDir * 2.f;

	m_hurtbox->SetPos(attackPos);

	float yaw = std::atan2(m_finalDir.x, m_finalDir.z);
	float swordYaw = std::atan2(playerDir.x, playerDir.z);

	m_hurtbox->GetComponent<Aen::OBBox>().SetRotation(0.f, yaw, 0.f);
	m_player->SetRot(0.f, Aen::RadToDeg(yaw) + 180.f, 0.f);

	if (!m_eventQueue.empty())
		if (m_eventQueue.front().duration > 0.f) {
			m_eventQueue.front().function(m_eventQueue.front().accell, m_eventQueue.front().duration);
			m_eventQueue.front().duration -= deltaTime;
		}
		else {
			if(axis.Magnitude() > 0.f)
				m_finalDir = Aen::Vec3f(dir.Normalized().x, 0.f, dir.Normalized().y);
			else if(m_eventQueue.size() > 1) {
				Aen::Vec2f dir(camDir.x, camDir.z);
				m_finalDir = Aen::Vec3f(dir.Normalized().x, 0.f, dir.Normalized().y);
			}

			m_eventQueue.pop_front();

			if(!m_eventQueue.empty()) {
				if(m_eventQueue.front().type == EventType::Attack) {
					Aen::Vec2f dir(camDir.x, camDir.z);
					m_finalDir = Aen::Vec3f(dir.Normalized().x, 0.f, dir.Normalized().y);
				}
			}
		}

	if (m_eventQueue.empty()) {
		if (axis.Magnitude() > 0.f) {
			m_finalDir = Aen::Vec3f(dir.Normalized().x, 0.f, dir.Normalized().y);
			m_player->GetComponent<Aen::CharacterController>().Move(m_finalDir * m_movementSpeed * deltaTime, deltaTime);
		}
	}

	if(!m_eventQueue.empty() && m_eventQueue.front().type == EventType::Dash)
		m_player->GetComponent<Aen::AABoundBox>().ToggleActive(false);
	else
		m_player->GetComponent<Aen::AABoundBox>().ToggleActive(true);

	m_v += Aen::Vec3f(-m_v.x * 1.8f, -30.f, -m_v.z * 1.8f) * deltaTime;
	m_v = Aen::Clamp(m_v, -Aen::Vec3f(20.f, 20.f, 20.f), Aen::Vec3f(20.f, 20.f, 20.f));
	m_player->GetComponent<Aen::CharacterController>().Move(m_v * deltaTime, deltaTime);
}

Aen::Entity*& Player::GetEntity() {
	return m_player;
}

Aen::Entity*& Player::GetHurtBox() {
	return m_hurtbox;
}

Aen::Entity*& Player::GetCamera()
{
	return m_camera;
}

void Player::UpdateAttack(std::deque<Enemy*>& e, const float& deltaTime) {
	// Attacking -------------------------------------------------------------------------------------

	if (!m_eventQueue.empty() && m_eventQueue.front().type == EventType::Attack) {
		/*m_attackTimer += deltaTime;*/

		for (int i = 0; i < e.size(); i++) {
			if (e[i]->GetEntity()->GetComponent<Aen::AABoundBox>().Intersects(m_hurtbox->GetComponent<Aen::OBBox>()) && !e[i]->IsHurt()) {

				e[i]->Hurt(true);

				e[i]->SubtractHealth(m_eventQueue.front().damage);
				Aen::Vec3f dir = Aen::Vec3f(0.f, 0.3f, 0.f) + (e[i]->GetEntity()->GetPos() - m_player->GetPos()).Normalized();
				e[i]->Move(dir.Normalized() * m_eventQueue.front().damage);
				
				if(e[i]->GetHealth() <= 0.f) {
					for(uint32_t k = 0u; k < m_targets.size(); k++)
						if(m_targets[k].target->GetEntity()->GetID() == e[i]->GetEntity()->GetID()) {
							m_targets.erase(m_targets.begin() + k);
							break;
						}

					delete e[i];
					e.erase(e.begin() + i);
				}
			}
		}

	} else {
		for(auto& i : e) i->Hurt(false);
		m_hurtbox->GetComponent<Aen::OBBox>().ToggleActive(false);
		//ResetSword();
	}
}

void Player::SubtractHealth(const float& damage) {
	m_health -= Aen::Abs(damage);
}

void Player::Move(const Aen::Vec3f& dir) {
	m_v = dir;
}

const float& Player::GetHealth() {
	return m_health;
}

 int Player::GetPotionNr()const
{
	return m_nrPotion;
}

const bool Player::IsAttacking() {
	if(!m_eventQueue.empty())
		return (m_eventQueue.front().type == EventType::Attack);
	return false;
}

//void Player::SwordSwing(float speed, float time, const float& deltaTime)
//{
//	static float timer = 0.f;
//	timer += deltaTime;
//	if (timer > time)
//	{
//		m_sword->SetRot(0, 0, 0);
//		timer = 0.f;
//	}
//	m_sword->Rotate(-speed, -speed, 0.f);
//}
//
//void Player::ResetSword()
//{
//	m_sword->SetRot(0.f, 0.f, 0.f);
//}

void Player::AddEvent(EventData& event) {
	if(m_eventQueue.size() > 1u)
		m_eventQueue.pop_back();

	m_eventQueue.emplace_back(event);
}
