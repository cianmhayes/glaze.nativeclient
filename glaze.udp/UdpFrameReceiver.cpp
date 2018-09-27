#include "pch.h"
#include "UdpFrameReceiver.h"
#include "../glaze.core/Trace.h"

using namespace std;
using namespace glaze::core;

namespace glaze {
    namespace udp {

        void UdpFrameReceiver::Initialize()
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

            BOOL bOptVal = FALSE;
            int bOptLen = sizeof(BOOL);
            if (setsockopt(m_socketId, SOL_SOCKET, SO_BROADCAST, (char *)&bOptVal, bOptLen) != 0)
            {
                TraceError(WSAGetLastError());
            }

            struct addrinfo *result = NULL;
            struct addrinfo *ptr = NULL;
            struct addrinfo hints;

            ZeroMemory(&hints, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_DGRAM;
            hints.ai_protocol = IPPROTO_UDP;

            m_broadcastAddr.sin_family = AF_INET;
            m_broadcastAddr.sin_port = htons(27015);

            struct sockaddr_in  sockaddr_ipv4;
            int gaiResult = getaddrinfo("", nullptr, &hints, &result);

            for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
            {
                if (ptr->ai_family == AF_INET && ptr->ai_socktype == SOCK_DGRAM && ptr->ai_protocol == IPPROTO_UDP)
                {
                    sockaddr_ipv4 = *((struct sockaddr_in *) ptr->ai_addr);
                }
            }

            m_broadcastAddr.sin_addr = sockaddr_ipv4.sin_addr;
            m_broadcastAddr.sin_addr.S_un.S_un_b.s_b4 = 255;

            m_thisAddr.sin_family = AF_INET;
            m_thisAddr.sin_port = htons(27015);
            m_thisAddr.sin_addr.s_addr = INADDR_ANY;

            if (bind(m_socketId, (sockaddr*)&m_thisAddr, sizeof(m_thisAddr)) < 0)
            {
                TraceError(WSAGetLastError());
            }
        }

        void WriteToBuffer(uint32_t value, uint32_t &index, char *buffer)
        {
            buffer[index++] = (value >> 24) & 0xff;
            buffer[index++] = (value >> 16) & 0xff;
            buffer[index++] = (value >> 8) & 0xff;
            buffer[index++] = (value) & 0xff;
        }

        void WriteToBuffer(uint8_t value, uint32_t &index, char *buffer)
        {
            buffer[index++] = (value) & 0xff;
        }

        void UdpFrameReceiver::ReceiveFrame(shared_ptr<Frame> newFrame)
        {
            // header
            // payload size : uint32 : 4
            // frameNumber : uint 32 : 4
            // frameType : uint8 : 1
            // bytes per pixel : uint8 : 1
            // width : uint32 : 4
            // height: uint32 : 4
            // frameRateNumerator : unint32 : 4
            // frameRateDenominator: uint32 : 4
            //
            // datagram payload length = 26 + frame payload

            uint32_t index = 0;
            uint32_t payloadLength = 26 + newFrame->GetLength();
            char* buffer = new char[payloadLength];

            WriteToBuffer(payloadLength, index, buffer);
            WriteToBuffer(newFrame->GetFrameNumber(), index, buffer);
            WriteToBuffer((uint8_t)newFrame->GetType(), index, buffer);
            WriteToBuffer((uint8_t)newFrame->GetBytesPerPixel(), index, buffer);
            WriteToBuffer(newFrame->GetWidth(), index, buffer);
            WriteToBuffer(newFrame->GetHeight(), index, buffer);
            WriteToBuffer(newFrame->GetFrameRateNumerator(), index, buffer);
            WriteToBuffer(newFrame->GetFrameRateDenominator(), index, buffer);

            for (int i = 0; i < newFrame->GetLength(); i++)
            {
                buffer[index + i] = newFrame->GetByte(i);
            }

            int nresult = sendto(m_socketId, buffer, payloadLength, 0, (SOCKADDR *)& m_broadcastAddr, sizeof(m_broadcastAddr));
            if (nresult < 0)
            {
                TraceError(WSAGetLastError());
            }

            delete[] buffer;
        }
        
        void UdpFrameReceiver::NotifyMissingFrame() {}

        UdpFrameReceiver::~UdpFrameReceiver()
        {
            closesocket(m_socketId);
            WSACleanup();
        }
    }
}