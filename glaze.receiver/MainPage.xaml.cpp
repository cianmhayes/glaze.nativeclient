//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include "../glaze.core/ImageMath.h"
#include "../glaze.core/Trace.h"

using namespace glaze_receiver;

using namespace glaze::core;
using namespace glaze::udp;
using namespace std;
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
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Microsoft::Graphics::Canvas::UI::Xaml;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

    m_receiver = make_shared<LambdaFrameReceiver>(
        [this](shared_ptr<glaze::core::Frame> f)
    {
        this->UpdateCanvas(f);
    },
        [this]()
    {
        this->NotifyMissingFrame();
    });

    m_udpFrameSource = make_shared<UdpFrameSource>();
    m_udpFrameSource->AddFrameReceiver(m_receiver);
    m_udpFrameSource->Start();
}

void glaze_receiver::MainPage::UpdateCanvas(shared_ptr<glaze::core::Frame> newFrame)
{
    TraceUpdateCanvas();
    this->m_currentFrame = newFrame;
    this->canvas->Invalidate();
}

void glaze_receiver::MainPage::NotifyMissingFrame()
{
}

void glaze_receiver::MainPage::canvas_Draw(CanvasControl^ sender, CanvasDrawEventArgs^ args)
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
