#pragma once
#include"Tranform/Tranform.h"
#include"Material/Material.h"
#include"Camera/Camera.h"
#include"Mesh/Mesh.h"
#include"PointLight/PointLight.h"

#include<unordered_map>

namespace Aen {

	class AEN_DECLSPEC ComponentHandler {
		private:

		ComponentHandler();

		// ---------- Camera Component ------------ //

		static const bool CameraExist(const uint32_t& id) {
			return m_cameras.count(id) > 0;
		}

		static void CreateCamera(const uint32_t& id) {
			m_cameras.emplace(id, AEN_NEW Camera());
		}

		static void RemoveCamera(const uint32_t& id) {
			if(m_cameras.count(id) > 0) {
				delete m_cameras.at(id);
				m_cameras.at(id) = nullptr;
				m_cameras.erase(id);
			}
		}

		static Camera& GetCamera(const uint32_t& id) {
			if(m_cameras.count(id) > 0)
				return *m_cameras.at(id);

			throw;
		}

		// ------------ Material Instance Component ----------- //

		static const bool MaterialInstanceExist(const uint32_t& id) {
			return m_materialInstance.count(id) > 0;
		}

		static void CreateMaterialInstance(const uint32_t& id) {
			m_materialInstance.emplace(id, AEN_NEW MaterialInstance());
		}

		static void RemoveMaterial(const uint32_t& id) {
			if(m_materialInstance.count(id) > 0) {
				delete m_materialInstance.at(id);
				m_materialInstance.at(id) = nullptr;
				m_materialInstance.erase(id);
			}
		}

		static MaterialInstance& GetMaterialInstance(const uint32_t& id) {
			if(m_materialInstance.count(id) > 0)
				return *m_materialInstance.at(id);

			throw;
		}

		// ----------- Mesh Instance Component ---------- //

		static const bool MeshInstanceExist(const uint32_t& id) {
			return m_mesheInstances.count(id) > 0;
		}

		static void CreateMeshInstance(const uint32_t& id) {
			m_mesheInstances.emplace(id, AEN_NEW MeshInstance());
		}

		static void RemoveMeshInstance(const uint32_t& id) {
			if(m_mesheInstances.count(id) > 0) {
				delete m_mesheInstances.at(id);
				m_mesheInstances.at(id) = nullptr;
				m_mesheInstances.erase(id);
			}
		}

		static MeshInstance& GetMeshInstance(const uint32_t& id) {
			if(m_mesheInstances.count(id) > 0)
				return *m_mesheInstances.at(id);
		}

		// ------------ Transform Component ------------- //

		static const bool TranslationExist(const uint32_t& id) {
			return m_translations.count(id) > 0;
		}

		static const bool RotationExist(const uint32_t& id) {
			return m_rotations.count(id) > 0;
		}

		static const bool ScaleExist(const uint32_t& id) {
			return m_scales.count(id) > 0;
		}

		static void CreateTranslation(const uint32_t& id) {
			m_translations.emplace(id, AEN_NEW Translation());
		}

		static void CreateRotation(const uint32_t& id) {
			m_rotations.emplace(id, AEN_NEW Rotation());
		}

		static void CreateScale(const uint32_t& id) {
			m_scales.emplace(id, AEN_NEW Scale());
		}

		static void RemoveTranform(const uint32_t& id) {
			if(m_translations.count(id) > 0) {
				delete m_translations.at(id);
				m_translations.at(id) = nullptr;
				m_translations.erase(id);
			}

			if(m_rotations.count(id) > 0) {
				delete m_rotations.at(id);
				m_rotations.at(id) = nullptr;
				m_rotations.erase(id);
			}

			if(m_scales.count(id) > 0) {
				delete m_scales.at(id);
				m_scales.at(id) = nullptr;
				m_scales.erase(id);
			}
		}

		static Translation& GetTranslation(const uint32_t& id) {
			if(m_translations.count(id) > 0)
				return *m_translations.at(id);

			throw;
		}

		static Rotation& GetRotation(const uint32_t& id) {
			if(m_rotations.count(id) > 0)
				return *m_rotations.at(id);

			throw;
		}

		static Scale& GetScale(const uint32_t& id) {
			if(m_scales.count(id) > 0)
				return *m_scales.at(id);

			throw;
		}

		// ----------------- Point Light Component ------------------ //

		static const bool PointLightExist(const uint32_t& id) {
			return m_pointLights.count(id) > 0;
		}

		static void CreatePointLight(const uint32_t& id) {
			m_pointLights.emplace(id, AEN_NEW PointLight());
		}

		static void RemovePointLight(const uint32_t& id) {
			if(m_pointLights.count(id) > 0) {
				delete m_pointLights.at(id);
				m_pointLights.at(id) = nullptr;
				m_pointLights.erase(id);
			}
		}

		static PointLight& GetPointLight(const uint32_t& id) {
			if(m_pointLights.count(id) > 0)
				return *m_pointLights.at(id);
		}

		static std::unordered_map<uint32_t, Camera*> m_cameras;
		static std::unordered_map<uint32_t, MaterialInstance*> m_materialInstance;
		static std::unordered_map<uint32_t, MeshInstance*> m_mesheInstances;
		static std::unordered_map<uint32_t, Translation*> m_translations;
		static std::unordered_map<uint32_t, Rotation*> m_rotations;
		static std::unordered_map<uint32_t, Scale*> m_scales;

		static std::unordered_map<uint32_t, PointLight*> m_pointLights;

		//friend class Translation;
		//friend class Rotation;
		//friend class Scale;
		//friend class MeshInstance;
		//friend class MaterialInstance;
		//friend class Camera;
		friend class Entity;
		friend class Renderer;
	};

}