﻿#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include "Frame.h"
#include "FrameReceiver.h"
#include "FrameSourceBase.h"

namespace glaze {
	namespace core {

		class QueuedFrameSourceBase : public FrameSourceBase
		{
		public:

			void Start();

			void Stop()
			{
				m_running = false;
			};

			unsigned int GetFrameCount()
			{
				return m_frameQueue.size();
			}

		protected:

			~QueuedFrameSourceBase()
			{
				m_running = false;
				if (m_dispatchThread.joinable())
				{
					m_dispatchThread.join();
				}
			}

			virtual void StartImpl() = 0;

			void DispatchFrame();

			bool m_running;
			std::thread m_dispatchThread;
			//std::mutex m_receiverMutex;
			std::mutex m_frameMutex;
			std::queue<std::shared_ptr<Frame>> m_frameQueue;
			//std::vector<std::shared_ptr<FrameReceiver>> m_receivers;
		};
	}
}
