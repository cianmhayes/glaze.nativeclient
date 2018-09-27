#pragma once

#include "FrameReceiver.h"

namespace glaze {
    namespace core {

        class CompoundFrameReceiver : public FrameReceiver
        {
        public:

            void AddFrameReceiver(std::shared_ptr<FrameReceiver> receiver);

            void ReceiveFrame(std::shared_ptr<Frame> newFrame);

            void NotifyMissingFrame();

        private :

            std::vector<std::shared_ptr<FrameReceiver>> m_receivers;
        };

    }
}