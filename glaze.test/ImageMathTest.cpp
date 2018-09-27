#include "pch.h"
#include "CppUnitTest.h"

#include "../glaze.core/ImageMath.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace glaze::core;

namespace glaze_test
{
    TEST_CLASS(ImageMathTest)
    {
    public:
        TEST_METHOD(GetSourceCroppingTest)
        {
            unsigned int cellSize = 0;
            unsigned int croppedSourceWidth = 0;
            unsigned int croppedSourceHeight = 0;

            GetSourceCropping(40, 30, 1600, 900, cellSize, croppedSourceWidth, croppedSourceHeight);
            Assert::AreEqual((unsigned int)40, cellSize);
            Assert::AreEqual((unsigned int)40, croppedSourceWidth);
            Assert::AreEqual((unsigned int)22, croppedSourceHeight);

            GetSourceCropping(160, 90, 640, 450, cellSize, croppedSourceWidth, croppedSourceHeight);
            Assert::AreEqual((unsigned int)5, cellSize);
            Assert::AreEqual((unsigned int)128, croppedSourceWidth);
            Assert::AreEqual((unsigned int)90, croppedSourceHeight);
        }
    };
}