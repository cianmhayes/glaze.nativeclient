#include "pch.h"
#include "UdpFrameSource.h"
#include "../glaze.core/Trace.h"

using namespace glaze::core;

namespace glaze {
    namespace udp {

        void UdpFrameSource::StartImpl()
        {
            WSADATA wsa;
            if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
            {
                TraceError(WSAGetLastError());
            }

            if ((m_socketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
            {
                TraceError(WSAGetLastError());
            }

            char enable = 1;

            if (setsockopt(m_socketId, SOL_SOCKET, SO_BROADCAST, (char *)&enable, enable) < 0)
            {
                TraceError(WSAGetLastError());
            }

            if (setsockopt(m_socketId, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
            {
                TraceError(WSAGetLastError());
            }

            struct sockaddr_in thisAddr;
            thisAddr.sin_family = AF_INET;
            thisAddr.sin_port = htons(m_port);
            thisAddr.sin_addr.s_addr = INADDR_ANY;

            if (bind(m_socketId, (sockaddr*)&thisAddr, sizeof(thisAddr)) < 0)
            {
                TraceError(WSAGetLastError());
            }

            m_listenerThread = std::thread([this] {Listener(); });
        }

        void ReadFromBuffer(uint32_t &value, uint32_t &index, char *buffer)
        {
            uint32_t c3 = ((buffer[index++]) << 24) & 0xff000000;
            uint32_t c2 = ((buffer[index++]) << 16) & 0x00ff0000;
            uint32_t c1 = ((buffer[index++]) <<  8) & 0x0000ff00;
            uint32_t c0 = (buffer[index++]) & 0x000000ff;
            
            value = c3 |c2 | c1 | c0;
        }

        void ReadFromBuffer(uint8_t &value, uint32_t &index, char *buffer)
        {
            value = buffer[index++];
        }

        void UdpFrameSource::Listener()
        {
            int packetBufferSize = 65000;
            char* pPacketBuffer = new char[packetBufferSize];
            while (m_running)
            {

                sockaddr_in from;
                int size = sizeof(from);
                int result = recvfrom(m_socketId, pPacketBuffer, packetBufferSize, 0, reinterpret_cast<SOCKADDR *>(&from), &size);

                //int result = recv(m_socketId, pPacketBuffer, packetBufferSize, 0);
                if (result < 0)
                {
                    TraceError(WSAGetLastError());
                    continue;
                }

                uint32_t index = 0;
                uint32_t packetSize = 0;
                uint32_t frameNumber = 0;
                uint8_t frameType = 0;
                uint8_t bytesPerPixel = 0;
                uint32_t width = 0;
                uint32_t height = 0;
                uint32_t frameRateNumerator = 0;
                uint32_t frameRateDenominator = 0;
                ReadFromBuffer(packetSize, index, pPacketBuffer);
                ReadFromBuffer(frameNumber, index, pPacketBuffer);
                ReadFromBuffer(frameType, index, pPacketBuffer);
                ReadFromBuffer(bytesPerPixel, index, pPacketBuffer);
                ReadFromBuffer(width, index, pPacketBuffer);
                ReadFromBuffer(height, index, pPacketBuffer);
                ReadFromBuffer(frameRateNumerator, index, pPacketBuffer);
                ReadFromBuffer(frameRateDenominator, index, pPacketBuffer);
                
                int remainingBytes = packetSize - index;
                std::vector<uint8_t> bytes(remainingBytes);

                for (int i = 0; i < remainingBytes; i++)
                {
                    bytes[i] = pPacketBuffer[index + i];
                }

                std::shared_ptr<Frame> newFrame = std::make_shared<Frame>(
                    frameNumber,
                    (FrameType) frameType,
                    width,
                    height,
                    bytesPerPixel,
                    frameRateNumerator,
                    frameRateDenominator,
                    bytes);

                m_frameQueue.push(newFrame);
            }

            delete[] pPacketBuffer;
        }

        UdpFrameSource::~UdpFrameSource()
        {
            m_running = false;
            if (m_listenerThread.joinable())
            {
                m_listenerThread.join();
            }

            closesocket(m_socketId);
            WSACleanup();
        }
    }
}