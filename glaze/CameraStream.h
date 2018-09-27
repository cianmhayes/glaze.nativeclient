#pragma once

#include <ppltasks.h>
#include <memory>
#include "../glaze.core/CompoundFrameReceiver.h"

namespace glaze{

    public ref class CameraStream sealed
    {
    public:

        Windows::Foundation::IAsyncAction^ InitializeAsync();

        Windows::Foundation::IAsyncOperation<Windows::Media::Capture::Frames::MediaFrameReaderStartStatus>^ StartStream();

    internal:

        void AddFrameReceiver(std::shared_ptr<glaze::core::FrameReceiver> receiver);

        void SetFrameType(glaze::core::FrameType frameType)
        {
            m_frameType = frameType;
        }

        glaze::core::FrameType GetFrameType()
        {
            return m_frameType;
        }

    private:

        void HandleFrameArrived(Windows::Media::Capture::Frames::MediaFrameReader^ sender, Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs^args);

        uint8_t GetAverageLuma(unsigned char* pPixels, int xCell, int yCell, int sourceWidth, int cellSize);

        uint8_t GetAverageHue(unsigned char* pPixels, int xCell, int yCell, int sourceWidth, int cellSize);

        Platform::String^ m_sourceId;
        Windows::Media::Capture::MediaCapture^ m_mediaCapture;
        Windows::Media::Capture::Frames::MediaFrameSource^ m_frameSource;
        Windows::Media::Capture::Frames::MediaFrameReader^ m_frameReader;

        std::shared_ptr<glaze::core::CompoundFrameReceiver> m_frameReceiver;
        glaze::core::FrameType m_frameType;
        int m_frameNumber;
    };
}