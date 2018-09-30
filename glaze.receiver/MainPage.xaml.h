//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "../glaze.core/Frame.h"
#include "../glaze.core/LambdaFrameReceiver.h"
#include "../glaze.udp/UdpFrameSource.h"

namespace glaze_receiver
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

    private:

        void canvas_Draw(Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs^ args);

        void UpdateCanvas(std::shared_ptr<glaze::core::Frame> newFrame);

        void NotifyMissingFrame();

        std::shared_ptr<glaze::core::LambdaFrameReceiver> m_receiver;
        std::shared_ptr<glaze::udp::UdpFrameSource> m_udpFrameSource;
        std::shared_ptr<glaze::core::Frame> m_currentFrame;
        
	};
}
