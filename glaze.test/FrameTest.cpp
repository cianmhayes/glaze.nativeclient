#include "pch.h"
#include "CppUnitTest.h"

#include "../glaze.core/Frame.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace glaze::core;

namespace glaze_test
{
	TEST_CLASS(FrameTest)
	{
	public:
		TEST_METHOD(ConstructorTest)
		{
			vector<uint8_t> bytes = vector<uint8_t>(200);
			for (int i = 0; i < bytes.size(); i++)
			{
				bytes[i] = rand() % 256;
			}
			Frame f = Frame(1, FrameType::Luma, 10, 20, 1, 0, 0, bytes);
			Assert::AreEqual((unsigned int)200, f.GetLength());
			Assert::AreEqual((unsigned int)20, f.GetHeight());
			Assert::AreEqual((unsigned int)10, f.GetWidth());
			Assert::AreEqual((unsigned int)1, f.GetBytesPerPixel());
			Assert::AreEqual(bytes[0], f.GetByte(0));
		}
	};
}