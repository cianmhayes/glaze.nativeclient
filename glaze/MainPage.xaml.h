//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

#include<memory>
#include "../glaze.core/Frame.h"
#include "../glaze.core/LambdaFrameReceiver.h"
#include "../glaze.core/FrameSourceBase.h"

namespace glaze
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

        std::shared_ptr<glaze::core::FrameSourceBase> m_frameSource;
        std::shared_ptr<glaze::core::LambdaFrameReceiver> m_receiver;
		std::shared_ptr<glaze::core::Frame> m_currentFrame;
	};
}
