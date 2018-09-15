#pragma once

#include <cstdint>
#include <vector>

namespace glaze {
	namespace core {

		enum FrameType : int
		{
			Unknown = 0,
			FullColour = 1,
			Hue = 2,
			Luma = 3
		};

		class Frame
		{
		public:

			Frame(
				FrameType frameType,
				unsigned int width,
				unsigned int height,
				unsigned int bytesPerPixel,
				std::vector<std::uint8_t> bytes) :
				m_frameType(frameType),
				m_width(width),
				m_height(height),
				m_bytesPerPixel(bytesPerPixel)
			{
				// TODO: validate parametes
				// TODO: support framerate
				m_bytes = bytes;
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

		private:

			FrameType m_frameType;
			unsigned int m_width;
			unsigned int m_height;
			unsigned int m_bytesPerPixel;
			std::vector<std::uint8_t> m_bytes;
		};

	}
}