#pragma once

namespace Aen {

	class Renderer;

	class Drawable {
		public:
		Drawable();
		~Drawable();

		private:
		virtual void Draw(Renderer& renderer, const uint32_t& id, const uint32_t& layer) = 0;
		virtual void DepthDraw(Renderer& renderer, const uint32_t& id, const uint32_t& layer) = 0;

		friend class Renderer;

	};
}