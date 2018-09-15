#include "pch.h"
#include "MemoryFrameSource.h"
#include <chrono>
#include <cstdint>

using namespace std;

namespace glaze {
	namespace generator {

		void MemoryFrameSource::StartImpl()
		{
		}

		void MemoryFrameSource::AddFrame(shared_ptr<glaze::core::Frame> frame)
		{
			m_frameQueue.push(frame);
		}
	}
}