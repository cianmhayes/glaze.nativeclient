#pragma once

#include <cstdint>
#include <vector>

namespace glaze {
	namespace core {

		enum FrameType : unsigned char
		{
			Unknown = 0,
			FullColour = 1,
			Hue = 2,
			Luma = 3,
            Cyanotype = 4
		};

		class Frame
		{
		public:

			Frame(
                unsigned int frameNumber,
				FrameType frameType,
				unsigned int width,
				unsigned int height,
				unsigned int bytesPerPixel,
                unsigned int frameRateNumerator,
                unsigned int frameRateDenominator,
				std::vector<std::uint8_t> bytes) :
                m_frameNumber(frameNumber),
				m_frameType(frameType),
				m_width(width),
				m_height(height),
				m_bytesPerPixel(bytesPerPixel),
                m_frameRateNumerator(frameRateNumerator),
                m_frameRateDenominator(frameRateDenominator)
			{
				// TODO: validate parametes
				// TODO: support framerate
				m_bytes = bytes;
			}

            unsigned int GetFrameNumber()
            {
                return m_frameNumber;
            }

			FrameType GetType()
			{
				return m_frameType;
			};

			unsigned int GetLength()
			{
				return m_bytes.size();
			};

			unsigned int GetWidth()
			{
				return m_width;
			};

			unsigned int GetHeight()
			{
				return m_height;
			};

			unsigned int GetBytesPerPixel()
			{
				return m_bytesPerPixel;
			};

			std::uint8_t GetByte(unsigned int index)
			{
				return m_bytes[index];
			};

            unsigned int GetFrameRateNumerator()
            {
                return m_frameRateNumerator;
            }

            unsigned int GetFrameRateDenominator()
            {
                return m_frameRateDenominator;
            }

		private:

            unsigned int m_frameNumber;
			FrameType m_frameType;
			unsigned int m_width;
			unsigned int m_height;
			unsigned int m_bytesPerPixel;
            unsigned int m_frameRateNumerator;
            unsigned int m_frameRateDenominator;
			std::vector<std::uint8_t> m_bytes;
		};

	}
}