#include "PCH.h"
#include"StaticBody.h"

namespace Aen {

	StaticBody::~StaticBody() {
		RemoveRigid();
		mp_Material->release();
	}

	StaticBody::StaticBody(const size_t& id)
		:Component(id), m_gType(StaticGeometryType::PLANE), mp_Material(nullptr), 
		mp_LocalPhysics(PhysicsHandler::GetInstance()->GetPxPhysics()), mp_StaticBody(nullptr), m_scale(Vec3f::one) {
		mp_Material = mp_LocalPhysics->createMaterial(1.f, 1.f, 0.2f);
		mp_Material->setFrictionCombineMode(px::PxCombineMode::eAVERAGE);
		mp_Material->setRestitutionCombineMode(px::PxCombineMode::eAVERAGE);

		px::PxPlane plane(px::PxVec3(0.f, 0.f, 0.f), px::PxVec3(0.f, 1.f, 0.f));
		mp_StaticBody = PxCreatePlane(*mp_LocalPhysics, plane, *mp_Material);
		PhysicsHandler::GetInstance()->AddActor(mp_StaticBody);
	}

	void StaticBody::SetStaticFriction(const float& value) {
		mp_Material->setStaticFriction(value);
	}

	void StaticBody::SetDynamicFriction(const float& value) {
		mp_Material->setDynamicFriction(value);
	}

	void StaticBody::SetRestitution(const float& value) {
		mp_Material->setRestitution(value);
	}

	void StaticBody::SetFrictionCombineMode(const CombineMode& mode) {
		mp_Material->setFrictionCombineMode((px::PxCombineMode::Enum)mode);
	}

	void StaticBody::SetRestitutionCombineMode(const CombineMode& mode) {
		mp_Material->setRestitutionCombineMode((px::PxCombineMode::Enum)mode);
	}

	void StaticBody::SetMaterialFlag(const MaterialFlag& flag) {
		mp_Material->setFlags((px::PxMaterialFlag::Enum)flag);
	}

	void StaticBody::SetGeometry(const StaticGeometryType& geometry, const Vec3f& scale) {

		if(geometry == m_gType)
			return;

		m_scale = scale;
		m_gType = geometry;
		px::PxTransform t = mp_StaticBody->getGlobalPose();
		RemoveRigid();

		switch(geometry) {
			case StaticGeometryType::SPHERE: {
				float r = Max(m_scale.x, Max(m_scale.y, m_scale.x)) * 0.5;
				px::PxSphereGeometry sphere(r);
				mp_StaticBody = PxCreateStatic(*mp_LocalPhysics, t, sphere, *mp_Material);
			} break;
			case StaticGeometryType::CAPSULE: {
				float rc = Max(m_scale.x, m_scale.z);
				px::PxCapsuleGeometry capsule(rc * 0.5f, m_scale.y);
				mp_StaticBody = PxCreateStatic(*mp_LocalPhysics, t, capsule, *mp_Material);
			} break;
			case StaticGeometryType::PLANE: {
				px::PxPlane plane(px::PxVec3(0.f, 0.f, 0.f), px::PxVec3(0.f, 1.f, 0.f));
				mp_StaticBody = PxCreatePlane(*mp_LocalPhysics, plane, *mp_Material);
			} break;
			case StaticGeometryType::CUBE: {
				px::PxBoxGeometry cube(m_scale.x * 0.5f, m_scale.y * 0.5f, m_scale.z * 0.5f);
				mp_StaticBody = PxCreateStatic(*mp_LocalPhysics, t, cube, *mp_Material);
			} break;
		}

		PhysicsHandler::GetInstance()->AddActor(mp_StaticBody);
	}


	void StaticBody::RemoveRigid() {
		PhysicsHandler::GetInstance()->RemoveActor(mp_StaticBody);
		mp_StaticBody->release();
		mp_StaticBody = nullptr;
	}

	const Mat4f StaticBody::GetTransform() {
		px::PxTransform t = mp_StaticBody->getGlobalPose();
		return MatQuaternion(t.q.x, t.q.y, t.q.z, t.q.w) * MatTranslate(t.p.x, t.p.y, t.p.z);
	}

	const Mat4f StaticBody::GetTranslate() {
		px::PxTransform t = mp_StaticBody->getGlobalPose();
		return MatTranslate(t.p.x, t.p.y, t.p.z);
	}

	const Mat4f StaticBody::GetRotMat() {
		px::PxTransform t = mp_StaticBody->getGlobalPose();
		return MatQuaternion(t.q.x, t.q.y, t.q.z, t.q.w);
	}

	void StaticBody::SetPos(const Vec3f& pos) {
		px::PxTransform t(pos.x, pos.y, pos.z);
		mp_StaticBody->setGlobalPose(t);
	}

	void StaticBody::SetPos(const float& x, const float& y, const float& z) {
		px::PxTransform t(px::PxVec3(x, y, z));
		mp_StaticBody->setGlobalPose(t);
	}

	void StaticBody::SetRot(const Vec3f& rot) {
		Vec4f tempRot = EulerToQuat(rot);
		px::PxTransform t(px::PxQuat(tempRot.x, tempRot.y, tempRot.z, tempRot.w));
		mp_StaticBody->setGlobalPose(t);
	}

	void StaticBody::SetRot(const float& p, const float& y, const float& r) {
		Vec4f tempRot = EulerToQuat(p, y, r);
		px::PxTransform t(px::PxQuat(tempRot.x, tempRot.y, tempRot.z, tempRot.w));
		mp_StaticBody->setGlobalPose(t);
	}

	const Vec3f StaticBody::GetPos() {
		px::PxTransform t = mp_StaticBody->getGlobalPose();
		return Vec3f(t.p.x, t.p.y, t.p.z);
	}

	const Vec3f StaticBody::GetRot() {
		return Vec3f();
	}
}