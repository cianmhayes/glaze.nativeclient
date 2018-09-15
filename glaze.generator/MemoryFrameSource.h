#pragma once

#include "../glaze.core/FrameSourceBase.h"

namespace glaze {
	namespace generator {

		class MemoryFrameSource : public glaze::core::FrameSourceBase
		{
		public:
			void AddFrame(std::shared_ptr<glaze::core::Frame> frame);

		private:
			void StartImpl();
		};
	}
}