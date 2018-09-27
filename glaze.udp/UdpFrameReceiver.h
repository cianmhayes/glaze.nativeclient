#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include "../glaze.core/FrameReceiver.h"

#define DEFAULT_PORT 50001

namespace glaze {
    namespace udp {

        class UdpFrameReceiver : public glaze::core::FrameReceiver
        {
        public:

            UdpFrameReceiver() : m_port(DEFAULT_PORT), m_socketId(0) {}

            UdpFrameReceiver(int port) : m_port(port), m_socketId(0) {}

            void Initialize();

            void ReceiveFrame(std::shared_ptr<glaze::core::Frame> newFrame);

            void NotifyMissingFrame();

            ~UdpFrameReceiver();

        private:

            int m_port;
            int m_socketId;

            struct sockaddr_in m_broadcastAddr;
            struct sockaddr_in m_thisAddr;
        };
    }
}