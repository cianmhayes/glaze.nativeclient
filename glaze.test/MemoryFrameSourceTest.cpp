#include "pch.h"
#include "CppUnitTest.h"

#include <thread>
#include <chrono>

#include "../glaze.generator/MemoryFrameSource.h"
#include "../glaze.core/LambdaFrameReceiver.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace std::chrono_literals;
using namespace glaze::core;
using namespace glaze::generator;

namespace glaze_test
{
    TEST_CLASS(MemoryFrameSourceTest)
    {
    public:
        TEST_METHOD(StartTest)
        {
			int receivedFrames = 0;
			shared_ptr<LambdaFrameReceiver> receiver =
				make_shared<LambdaFrameReceiver>([&](shared_ptr<Frame> f) mutable
			{
				receivedFrames++;
			}, nullptr);

			MemoryFrameSource source;
			for (int i = 0; i < 10; i++)
			{
				source.AddFrame(make_shared<Frame>(FrameType::Luma, 1, 1, 1, vector<uint8_t>(1, i)));
			}
			source.AddFrameReceiver(receiver);
			source.Start();

			this_thread::sleep_for(1s);

			Assert::AreEqual((unsigned int)0, source.GetFrameCount());
			Assert::AreEqual(10, receivedFrames);
        }
    };
}