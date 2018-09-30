#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include "Frame.h"
#include "FrameReceiver.h"

namespace glaze {
	namespace core {

		class FrameSourceBase
		{
		public:

			virtual void Start();

			virtual void Stop()
			{
				m_running = false;
			};

			virtual void AddFrameReceiver(std::shared_ptr<FrameReceiver> receiver)
			{
				m_receivers.push_back(receiver);
			};

			virtual unsigned int GetFrameCount()
			{
				return m_frameQueue.size();
			}

		protected:

			~FrameSourceBase()
			{
				m_running = false;
				if (m_dispatchThread.joinable())
				{
					m_dispatchThread.join();
				}
			}

			virtual void StartImpl() = 0;

			virtual void DispatchFrame();

			bool m_running;
			std::thread m_dispatchThread;
			std::mutex m_receiverMutex;
			std::mutex m_frameMutex;
			std::queue<std::shared_ptr<Frame>> m_frameQueue;
			std::vector<std::shared_ptr<FrameReceiver>> m_receivers;
		};
	}
}
