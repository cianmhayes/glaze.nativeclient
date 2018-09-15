#pragma once

#include <thread>
#include "../glaze.core/FrameSourceBase.h"

namespace glaze {
    namespace generator {

        class NoiseFrameSource : public glaze::core::FrameSourceBase
        {
        public:

            NoiseFrameSource(glaze::core::FrameType frameType, unsigned int width, unsigned int height, unsigned int bytesPerPixel) :
                m_frameType(frameType),
                m_width(width),
                m_height(height),
                m_bytesPerPixel(bytesPerPixel) {};

            ~NoiseFrameSource()
            {
                m_running = false;
                if (m_generationThread.joinable())
                {
                    m_generationThread.join();
                }
            }

        private:

            void StartImpl();

            void GenerateFrames();

            std::thread m_generationThread;
            glaze::core::FrameType m_frameType;
            unsigned int m_width;
            unsigned int m_height;
            unsigned int m_bytesPerPixel;
        };
    }
}