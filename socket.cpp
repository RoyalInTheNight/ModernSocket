#include "include/socket/socket.h"
#include <sys/socket.h>

int32_t main() {
    RITNet::UnixSocketServer server(
        new RITNet::UnixConnectionType(
            "/tmp/path.sock", 
            ::socket(
                AF_UNIX, 
                SOCK_STREAM, 
                0
            )
        ),

        ".log",
        RITNet::ConnectionDefines::SocketLoggingType::stdoutLogging
    );

    server.setUnixThreadStatus(
        RITNet::ConnectionDefines::SocketThreadStatus::threadDisable
    );
    
    server.socketInit();
    server.socketAccept();

    return 0;
}