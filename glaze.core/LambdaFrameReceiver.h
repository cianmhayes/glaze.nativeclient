#pragma once

#include <functional>
#include "FrameReceiver.h"

namespace glaze {
	namespace core {

		class LambdaFrameReceiver : public FrameReceiver
		{
		public:

			LambdaFrameReceiver(
                std::function<void(std::shared_ptr<Frame>)> receiveFrameCallback,
                std::function<void()> missingFrameCallback
            ) :
                m_receiveFrameCallback(receiveFrameCallback),
                m_missingFrameCallback(missingFrameCallback)
            {};

			void ReceiveFrame(std::shared_ptr<Frame> newFrame)
			{
                if (m_receiveFrameCallback)
                {
                    m_receiveFrameCallback(newFrame);
                }
            };

            void NotifyMissingFrame()
            {
                if (m_missingFrameCallback)
                {
                    m_missingFrameCallback();
                }
            }

		private:

			std::function<void(std::shared_ptr<Frame>)> m_receiveFrameCallback;
            std::function<void()> m_missingFrameCallback;
		};
	}
}