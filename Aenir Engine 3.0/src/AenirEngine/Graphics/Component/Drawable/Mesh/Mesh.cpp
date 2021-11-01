#include"PCH.h"
#include"Mesh.h"
#include"Core/GlobalSettings.h"
#include"Importer/ObjImporter.h"
#include"Importer/AssimpImporter.h"
#include<thread>

#undef min

#include"assimp/Importer.hpp"
#include"assimp/scene.h"
#include"assimp/postprocess.h"
#include"assimp/matrix4x4.h"
#include"assimp/cimport.h"

namespace Aen {
	
	Mesh::Mesh()
		:m_vertices(), m_partitions(), m_meshMaterialName() {}
	
	Mesh::Mesh(const std::string& dir) {
		ImportObj(m_vertices, dir, m_partitions, m_meshMaterialName);
	}
	
	void Mesh::PrintMaterialSlots() {
		for(auto& i : m_meshMaterialName) {
			AEN_PRINT(i.first); 
			AEN_ENDL;
		}
	}

	void Mesh::Load(const std::string& dir) {


		size_t off = dir.find_last_of('.');
		std::string format = dir.substr(off + 1);
		if (format == "fbx") {
			std::vector<DirectX::XMFLOAT3> vPos;
			AssimpImport::LoadFbx(vPos, m_vertices, dir, m_partitions, m_meshMaterialName);

			size_t vStride = sizeof(DirectX::XMFLOAT3);
			m_aabb.CreateFromPoints(m_aabb, vPos.size(), vPos.data(), vStride);

		} else {
			throw;
			printf("Format not supported!");
		}
		

	}

	Mesh::~Mesh() {
		//delete[] mp_posV;
	}
}