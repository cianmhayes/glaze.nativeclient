//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <string>
#include <cmath>

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

using namespace glaze;
using namespace glaze::core;
using namespace glaze::generator;


// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

    m_frameSource = make_shared<NoiseFrameSource>(FrameType::Hue, 160, 90, 1);
    m_receiver = make_shared<LambdaFrameReceiver>(
        [this](shared_ptr<glaze::core::Frame> f) 
        {
            this->UpdateCanvas(f);
        },
        [this]()
        {
            this->NotifyMissingFrame();
        });
    m_frameSource->AddFrameReceiver(m_receiver);
    m_frameSource->Start();
}

void glaze::MainPage::UpdateCanvas(shared_ptr<glaze::core::Frame> newFrame)
{
    TraceUpdateCanvas();
    this->m_currentFrame = newFrame;
    this->canvas->Invalidate();
}

void glaze::MainPage::NotifyMissingFrame()
{
    TraceMissingFrame();
}

void glaze::MainPage::canvas_Draw(CanvasControl^ sender, CanvasDrawEventArgs^ args)
{
    TraceDrawStart();

    if (this->m_currentFrame == 0)
    {
        return;
    }

    shared_ptr<glaze::core::Frame> currentFrame = this->m_currentFrame;
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
                    hsv.s = 192;
                    hsv.v = 255;
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

    TraceDrawEnd();
}
