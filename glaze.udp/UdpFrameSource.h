#pragma once

#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../glaze.core/FrameSourceBase.h"
#include "UdpFrameReceiver.h"

namespace glaze {
    namespace udp {

        class UdpFrameSource : public glaze::core::FrameSourceBase
        {
        public:

            UdpFrameSource() : m_port(DEFAULT_PORT), m_socketId(0) {}

            UdpFrameSource(int port) : m_port(port), m_socketId(0) {}

            ~UdpFrameSource();

        private:

            void StartImpl();

            void Listener();

            std::thread m_listenerThread;

            int m_port;
            SOCKET m_socketId;
        };
    }
}