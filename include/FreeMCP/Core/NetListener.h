#ifndef FREEMCP_CORE_NETLISTENER_H
#define FREEMCP_CORE_NETLISTENER_H

namespace FreeMCP::Core {
    class NetListener {
    public:
        static void start(int port);
        static void stop();
    private:
        static void listen_loop(int port);
        static bool running;
    };
}

#endif
