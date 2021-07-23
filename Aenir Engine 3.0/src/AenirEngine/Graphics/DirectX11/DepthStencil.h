#pragma once
#include"DX11Core.h"

namespace Aen {

	class DepthStencil : public GCore {
		public:
		~DepthStencil();
		DepthStencil();
		DepthStencil(const Window& window);

		void Create(const Window& window);
		friend class RenderSystem;

		private:
		ComDepthStencilView m_dsView;
		ComDepthStencilState m_dsState;
	};
}
