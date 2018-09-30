//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <string>
#include <cmath>

#include <ppltasks.h>
#include <MemoryBuffer.h>

#include "../glaze.generator/NoiseFrameSource.h"
#include "../glaze.core/ImageMath.h"
#include "../glaze.core/Trace.h"

using namespace std;
using namespace std::chrono_literals;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Microsoft::Graphics::Canvas::UI::Xaml;

using namespace concurrency;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
//using namespace Windows::Foundation::Metadata;
using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::System::Threading;

using namespace glaze;
using namespace glaze::core;
using namespace glaze::generator;
using namespace glaze::udp;


MainPage::MainPage()
{
	InitializeComponent();

    m_cameraSource = ref new CameraStream();

    op = create_task(m_cameraSource->InitializeAsync()).then([this]()
    {
        return create_task(m_cameraSource->StartStream());
    });

    m_receiver = make_shared<LambdaFrameReceiver>(
        [this](shared_ptr<glaze::core::Frame> f) 
        {
            this->UpdateCanvas(f);
        },
        [this]()
        {
            this->NotifyMissingFrame();
        });
    m_cameraSource->SetFrameType(FrameType::Luma);
    m_cameraSource->AddFrameReceiver(m_receiver);
    
    m_udpFrameReceiver = make_shared<UdpFrameReceiver>();
    m_udpFrameReceiver->Initialize();
    m_cameraSource->AddFrameReceiver(m_udpFrameReceiver);
}

void glaze::MainPage::UpdateCanvas(shared_ptr<glaze::core::Frame> newFrame)
{
    TraceUpdateCanvas();
    this->m_currentFrame = newFrame;
    this->canvas->Invalidate();
}

void glaze::MainPage::NotifyMissingFrame()
{
}

void glaze::MainPage::canvas_Draw(CanvasControl^ sender, CanvasDrawEventArgs^ args)
{
    if (this->m_currentFrame == 0)
    {
        TraceDrawStart(-1);
        return;
    }

    shared_ptr<glaze::core::Frame> currentFrame = this->m_currentFrame;
    TraceDrawStart(this->m_currentFrame->GetFrameNumber());

    unsigned int frameWidth = currentFrame->GetWidth();
    unsigned int frameHeight = currentFrame->GetHeight();
    unsigned int bytesPerPixel = currentFrame->GetBytesPerPixel();
    unsigned int croppedWidth = 0;
    unsigned int croppedHeight = 0;
    unsigned int cellSize = 0;

    GetSourceCropping(
        frameWidth,
        frameHeight,
        sender->ActualWidth,
        sender->ActualHeight,
        cellSize,
        croppedWidth,
        croppedHeight);

    unsigned int topOffset = ceil((frameHeight - croppedHeight) / 2.0);
    unsigned int leftOffset = ceil((frameWidth - croppedWidth) / 2.0);

    for (int x = 0; x < croppedWidth; x++)
    {
        for (int y = 0; y < croppedHeight; y++)
        {
            unsigned int pixelIndex = ((topOffset + y) * frameWidth) + leftOffset + x;

            Color cellColour = Colors::Black;

            switch (currentFrame->GetType())
            {
                case FrameType::Cyanotype:
                {
                    uint8_t luma = currentFrame->GetByte(pixelIndex * bytesPerPixel);
                    HsvColor hsv;
                    hsv.h = 147;
                    hsv.s = 230; // 90%
                    hsv.v = (((((float)luma) / 255)  * 0.7) + 0.2) * 255;
                    RgbColor rgb = HsvToRgb(hsv);
                    cellColour = ColorHelper::FromArgb(255, rgb.r, rgb.g, rgb.b);
                    break;
                }
                case FrameType::Luma:
                {
                    uint8_t luma = currentFrame->GetByte(pixelIndex * bytesPerPixel);
                    cellColour = ColorHelper::FromArgb(255, luma, luma, luma);
                    break;
                }
                case FrameType::Hue:
                {
                    HsvColor hsv;
                    hsv.h = currentFrame->GetByte(pixelIndex * bytesPerPixel);
                    hsv.s = 173; // 68%
                    hsv.v = 224; // 88%
                    RgbColor rgb = HsvToRgb(hsv);
                    cellColour = ColorHelper::FromArgb(255, rgb.r, rgb.g, rgb.b);
                    break;
                }
                default:
                    cellColour = Colors::Black;
            }

            args->DrawingSession->FillRectangle(x * cellSize, y * cellSize, cellSize, cellSize, cellColour);
        }
    }

    TraceDrawEnd(this->m_currentFrame->GetFrameNumber());
}

