#include "pch.h"
#include "FrameSourceBase.h"
#include "Trace.h"

using namespace std;

namespace glaze {
	namespace core {

		void FrameSourceBase::Start()
		{
			m_running = true;
			m_dispatchThread = thread([this] {DispatchFrame(); });
			StartImpl();
		}

		void FrameSourceBase::DispatchFrame()
		{
			while (m_running)
			{
                TraceDispatchCycleStart();
                // TODO: calcualte callback time from frame duration
                std::chrono::system_clock::time_point targetCallbackTime = std::chrono::system_clock::now();
                targetCallbackTime = targetCallbackTime + std::chrono::milliseconds(40);
				shared_ptr<Frame> frame = nullptr;
                {
                    lock_guard<mutex> frameLock(m_frameMutex);
                    if (!m_frameQueue.empty())
                    {
                        frame = m_frameQueue.front();
                        m_frameQueue.pop();
                    }
                }
                if(frame != nullptr)
                {
                    lock_guard<mutex> receiverLock(m_receiverMutex);
                    for (auto itr = m_receivers.begin(); itr < m_receivers.end(); ++itr)
                    {
                        shared_ptr<FrameReceiver> receiver = *itr;
                        thread([&] {receiver->ReceiveFrame(frame); }).detach();
                    }
                    TraceFrameDispatched();
                }

                this_thread::sleep_until(targetCallbackTime);
			}
		}

	}
}