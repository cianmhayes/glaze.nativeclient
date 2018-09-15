#include "pch.h"
#include "NoiseFrameSource.h"
#include "../glaze.core/Trace.h"
#include <chrono>
#include <cstdint>

using namespace std;
using namespace std::chrono_literals;

using namespace glaze::core;

namespace glaze {
    namespace generator {

        void NoiseFrameSource::StartImpl()
        {
            m_generationThread = thread([this] {GenerateFrames(); });
        }

        void NoiseFrameSource::GenerateFrames()
        {
            while (m_running)
            {
                if (m_frameQueue.size() < 200)
                {
                    vector<uint8_t> bytes = vector<uint8_t>(m_width * m_height * m_bytesPerPixel);
                    for (int p = 0; p < bytes.size(); p++)
                    {
                        bytes[p] = rand();
                    }
                    shared_ptr<Frame> newFrame = make_shared<Frame>(m_frameType, m_width, m_height, m_bytesPerPixel, bytes);
                    lock_guard<mutex> frameLock(m_frameMutex);
                    m_frameQueue.push(newFrame);
                    TraceFrameGenerated();
                }
            }
        }
    }
}