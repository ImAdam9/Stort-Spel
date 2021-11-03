#pragma once
#include"../Enemy.h"

class Rimuru : public Enemy {
	public:
	Rimuru();
	Rimuru(const Aen::Vec3f &pos);
	~Rimuru() override;

	// Inherited via Enemy
	virtual void Update(const float& deltaTime, Aen::Entity& player) override;

	private:
	Aen::Entity* m_rimuru;
	Aen::Vec3f m_lDir;
	float v;

};