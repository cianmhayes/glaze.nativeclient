#pragma once

#include "Frame.h"
#include <memory>

namespace glaze {
	namespace core {

		class FrameReceiver
		{
		public:

			virtual void ReceiveFrame(std::shared_ptr<Frame> newFrame) = 0;

            virtual void NotifyMissingFrame() = 0;
		};

	}
}