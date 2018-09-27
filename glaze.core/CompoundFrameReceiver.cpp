#include "pch.h"
#include "CompoundFrameReceiver.h"
#include <thread>

using namespace std;

namespace glaze {
    namespace core {

        void CompoundFrameReceiver::AddFrameReceiver(std::shared_ptr<FrameReceiver> receiver)
        {
            m_receivers.push_back(receiver);
        }

        void CompoundFrameReceiver::ReceiveFrame(std::shared_ptr<Frame> newFrame)
        {
            if (newFrame != nullptr)
            {
                for (auto itr = m_receivers.begin(); itr < m_receivers.end(); ++itr)
                {
                    shared_ptr<FrameReceiver> receiver = *itr;
                    thread([=] {receiver->ReceiveFrame(newFrame); }).detach();
                }
            }
        }

        void CompoundFrameReceiver::NotifyMissingFrame()
        {
            for (auto itr = m_receivers.begin(); itr < m_receivers.end(); ++itr)
            {
                shared_ptr<FrameReceiver> receiver = *itr;
                thread([=] {receiver->NotifyMissingFrame(); }).detach();
            }
        }
    }
}
