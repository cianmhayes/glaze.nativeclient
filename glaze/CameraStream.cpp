#include "pch.h"
#include "CameraStream.h"
#include "../glaze.core/Trace.h"
#include "../glaze.core/ImageMath.h"
#include "../glaze.core/ErrorCodes.h"

#include <MemoryBuffer.h>

using namespace concurrency;
using namespace std;

using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Graphics::Imaging;

using namespace glaze::core;

namespace glaze {

    IAsyncAction^ CameraStream::InitializeAsync()
    {
        m_frameNumber = 0;
        m_frameReceiver = make_shared<CompoundFrameReceiver>();
        m_mediaCapture = ref new MediaCapture();

        return create_async([this]() {
            return create_task(MediaFrameSourceGroup::FindAllAsync())
                .then([this](IVectorView<MediaFrameSourceGroup^>^ groups)
            {
                for (auto&& sg : groups)
                {
                    for (auto&& si : sg->SourceInfos)
                    {
                        if (si->MediaStreamType == MediaStreamType::VideoRecord &&
                            si->SourceKind == MediaFrameSourceKind::Color)
                        {
                            m_sourceId = si->Id;
                            return sg;
                        }
                    }
                }

                throw ref new Exception(-1, "No compatible MediaFrameSourceGroup");

            }).then([this](MediaFrameSourceGroup^ sourceGroup)
            {
                MediaCaptureInitializationSettings^ settings = ref new MediaCaptureInitializationSettings();
                settings->SourceGroup = sourceGroup;
                settings->SharingMode = MediaCaptureSharingMode::SharedReadOnly;
                settings->StreamingCaptureMode = StreamingCaptureMode::Video;
                settings->MemoryPreference = MediaCaptureMemoryPreference::Cpu;

                return create_task(m_mediaCapture->InitializeAsync(settings));
            }).then([this]()
            {
                m_frameSource = m_mediaCapture->FrameSources->Lookup(m_sourceId);
                return create_task(m_mediaCapture->CreateFrameReaderAsync(m_frameSource));
            }).then([this](MediaFrameReader^ reader)
            {
                m_frameReader = reader;
                m_frameReader->FrameArrived +=
                    ref new TypedEventHandler<MediaFrameReader^, MediaFrameArrivedEventArgs^>(this, &CameraStream::HandleFrameArrived);
            });
        });
    }

    void CameraStream::AddFrameReceiver(shared_ptr<FrameReceiver> receiver)
    {
        m_frameReceiver->AddFrameReceiver(receiver);
    }

    IAsyncOperation<MediaFrameReaderStartStatus>^ CameraStream::StartStream()
    {
        return create_async([this]()
        {
            return create_task(m_frameReader->StartAsync());
        });
    }

    void CameraStream::HandleFrameArrived(Windows::Media::Capture::Frames::MediaFrameReader^ sender, Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs^args)
    {
        int currentFrameNumber = m_frameNumber++;
        TraceFrameCaptureStart(currentFrameNumber);

        MediaFrameReference^ frameReference = sender->TryAcquireLatestFrame();

        if (frameReference == nullptr)
        {
            TraceMissingFrame(currentFrameNumber);
            m_frameReceiver->NotifyMissingFrame();
            return;
        }

        VideoMediaFrame^ vmf = frameReference->VideoMediaFrame;
        SoftwareBitmap^ bmp = vmf->SoftwareBitmap;
        BitmapPixelFormat pixelFormat = bmp->BitmapPixelFormat;

        SoftwareBitmap^ convertedBmp = SoftwareBitmap::Convert(bmp, BitmapPixelFormat::Rgba8);

        BitmapPixelFormat convertedPixelFormat = convertedBmp->BitmapPixelFormat;

        if (convertedPixelFormat != BitmapPixelFormat::Rgba8)
        {
            TraceError(GLAZE_ERROR_UNSUPPORTED_PIXEL_FORMAT);
            throw ref new Exception(GLAZE_ERROR_UNSUPPORTED_PIXEL_FORMAT, "");
        }

        BitmapBuffer^ bitmapBuffer = convertedBmp->LockBuffer(BitmapBufferAccessMode::Read);
        IMemoryBufferReference^ reference = bitmapBuffer->CreateReference();
        ComPtr<IMemoryBufferByteAccess> pBufferByteAccess;
        if ((reinterpret_cast<IInspectable*>(reference)->QueryInterface(IID_PPV_ARGS(&pBufferByteAccess))) != S_OK)
        {
            TraceError(GLAZE_ERROR_INNACCESSIBLE_BUFFER);
            throw ref new Exception(GLAZE_ERROR_INNACCESSIBLE_BUFFER, "");
        }

        unsigned int capacity = 0;
        unsigned char* pPixels = nullptr;
        if (pBufferByteAccess->GetBuffer(&pPixels, &capacity) != S_OK)
        {
            TraceError(GLAZE_ERROR_INNACCESSIBLE_BUFFER);
            throw ref new Exception(GLAZE_ERROR_INNACCESSIBLE_BUFFER, "");
        }

        FrameType frameType = m_frameType;
        unsigned int cellSize = 10;
        unsigned int width = convertedBmp->PixelWidth;
        unsigned int height = convertedBmp->PixelHeight;
        unsigned int targetWidth = width / cellSize;
        unsigned int targetHeight = height / cellSize;
        unsigned int bytesPerPixel = 1;
        std::vector<uint8_t> result(targetWidth * targetHeight * bytesPerPixel);
        for (int x = 0; x < targetWidth; x++)
        {
            for (int y = 0; y < targetHeight; y++)
            {
                int targetByteIndex = (y * targetWidth) + x;
                switch (frameType)
                {
                case FrameType::Hue:
                    result[targetByteIndex] = GetAverageHue(pPixels, x, y, width, cellSize);
                    break;
                //case FrameType::Cyanotype:
                //case FrameType::Luma:
                default:
                    result[targetByteIndex] = GetAverageLuma(pPixels, x, y, width, cellSize);
                    break;
                }
            }
        }

        shared_ptr<glaze::core::Frame> newFrame =
            make_shared<glaze::core::Frame>(
                currentFrameNumber,
                frameType,
                targetWidth,
                targetHeight,
                bytesPerPixel,
                frameReference->Format->FrameRate->Numerator,
                frameReference->Format->FrameRate->Denominator,
                result);
        m_frameReceiver->ReceiveFrame(newFrame);

        TraceFrameCaptureEnd(currentFrameNumber);
    }

    uint8_t CameraStream::GetAverageLuma(unsigned char* pPixels, int xCell, int yCell, int sourceWidth, int cellSize)
    {
        int totalLuma = 0;
        for (int sub_x = 0; sub_x < cellSize; sub_x++)
        {
            for (int sub_y = 0; sub_y < cellSize; sub_y++)
            {
                int total_y = (yCell * cellSize) + sub_y;
                int total_x = (xCell * cellSize) + sub_x;
                int pixelIndex = (total_y * sourceWidth) + total_x;
                int byteIndex = pixelIndex * 4;

                RgbColor source;
                source.r = pPixels[byteIndex];
                source.g = pPixels[byteIndex + 1];
                source.b = pPixels[byteIndex + 2];
                totalLuma += GetLuma(source);
            }
        }

        return (totalLuma / (cellSize * cellSize));
    }

    uint8_t CameraStream::GetAverageHue(unsigned char* pPixels, int xCell, int yCell, int sourceWidth, int cellSize)
    {
        float sumSin = 0;
        float sumCos = 0;
        for (int sub_x = 0; sub_x < cellSize; sub_x++)
        {
            for (int sub_y = 0; sub_y < cellSize; sub_y++)
            {
                int total_y = (yCell * cellSize) + sub_y;
                int total_x = (xCell * cellSize) + sub_x;
                int pixelIndex = (total_y * sourceWidth) + total_x;
                int byteIndex = pixelIndex * 4;

                RgbColor source;
                source.r = pPixels[byteIndex];
                source.g = pPixels[byteIndex + 1];
                source.b = pPixels[byteIndex + 2];
                HsvColor hsv = RgbToHsv(source);
                float radians = ToRadians(hsv.h);
                sumSin += sin(radians);
                sumCos += cos(radians);
            }
        }

        return ToDegrees(atan2(sumSin, sumCos));
    }

}