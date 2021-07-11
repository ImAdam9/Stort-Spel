#pragma once
#include"DX11Core.h"

namespace Aen {

	class DepthStencil : public GCore {
		public:
		DepthStencil(const Window& window);

		friend class BBuffer;
		friend class RenderSystem;

		private:
		ComDepthStencilView m_dsView;
		ComDepthStencilState m_dsState;
	};
}
