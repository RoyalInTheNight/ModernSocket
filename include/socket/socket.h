#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <cstring>
#include <netinet/in.h>
#ifndef _WIN32
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#endif // UNIX not supported int WIN32

#include <cstdint>
#include <cstdlib>

#include <vector>
#include <string>

namespace RITNet {
    namespace unixTypes {
        typedef int32_t        socket_t;
        typedef sockaddr_un    header_t;
    }

    namespace inetTypes {
        typedef int32_t        socket_t;
        typedef sockaddr_in    header_t;
    }

    struct UnixConnectionType {
    public:
        unixTypes::header_t header_;
        unixTypes::socket_t socket_;
        socklen_t       headerSize_;

    public:
        UnixConnectionType(const char *path, unixTypes::socket_t socket) {
            strcpy(header_.sun_path, path);

            header_.sun_family = AF_UNIX;
            socket_            =  socket;
            headerSize_        = sizeof(header_);
        }

        UnixConnectionType(const UnixConnectionType& copy) {
            header_     = copy.header_;
            socket_     = copy.socket_;
            headerSize_ = copy.headerSize_;
        }
    };

    struct InetConnectionType {
    public:
        inetTypes::header_t header_;
        inetTypes::socket_t socket_;
        socklen_t       headerSize_;

    public:
        InetConnectionType(const uint32_t address, const uint16_t port, int32_t socket) {
            header_.sin_addr.s_addr = address;
            header_.sin_port        = port;
            header_.sin_family      = AF_INET;

            socket_     = socket;
            headerSize_ = sizeof(header_);
        }

        InetConnectionType(const InetConnectionType& copy) {
            header_     = copy.header_;
            socket_     = copy.socket_;
            headerSize_ = copy.headerSize_;
        }
    };

    namespace ConnectionDefines {
        enum class SocketServerStatus : uint8_t {
            errSocketDescriptor = 0xa1,
            errSocketBind,
            errSocketListen,
            errSocketAccept,
            errSocketRead,
            errSocketSend,
            ok
        };

        enum class SocketClientStatus : uint8_t {
            errSocketDescriptor = 0xb1,
            errSocketConnect,
            errSocketRead,
            errSocketSend,
            ok
        };

        enum class SocketThreadStatus : uint8_t {
            threadEnable = 0xc1,
            threadDisable
        };

        enum class SocketLoggingType  : uint8_t {
            stdoutLogging = 0xd1,
            fileLogging,
            disableLogging
        };
    }

    class Logging {
    private:
        ConnectionDefines::SocketLoggingType logging_;
        std::string                         log_path_;

    public:
        void setLoggingPath(const std::string&);
        void setLogging(const ConnectionDefines::SocketLoggingType&);
        
        [[nodiscard]] ConnectionDefines::SocketLoggingType getLogging() const;

        void logging(const std::string&);
    };

    class UnixSocketClient : public Logging {
    private:
        UnixConnectionType *           connectionData_;

        ConnectionDefines::SocketServerStatus  status_;
        ConnectionDefines::SocketThreadStatus  thread_;

        uint32_t CID_counter = 0;

    public:
        UnixSocketClient(const UnixConnectionType *, const std::string& = ".unix_log", const ConnectionDefines::SocketLoggingType& = ConnectionDefines::SocketLoggingType::disableLogging);
        UnixSocketClient(const UnixSocketClient&, const std::string& = ".unix_log", const ConnectionDefines::SocketLoggingType& = ConnectionDefines::SocketLoggingType::disableLogging);
        UnixSocketClient(const std::string& = ".unix_log", const ConnectionDefines::SocketLoggingType& = ConnectionDefines::SocketLoggingType::disableLogging);

        [[nodiscard]] UnixConnectionType            getUnixConnectionType() const;
        [[nodiscard]] ConnectionDefines::SocketServerStatus getUnixStatus() const;
        [[nodiscard]] ConnectionDefines::SocketThreadStatus getUnixThread() const;
        [[nodiscard]] ConnectionDefines::SocketLoggingType getUnixLogging() const;

        void setUnixConnectionType(const UnixConnectionType *);
        void setUnixSocketClient(const UnixSocketClient&);
        void setUnixThreadStatus(const ConnectionDefines::SocketThreadStatus&);
        void setUnixLoggingType(const ConnectionDefines::SocketLoggingType&);

        void socketInit();

        int32_t read(void *, const uint32_t);
        int32_t read(std::vector<char>&, const uint32_t);

        void send(const void *, const uint32_t);
        void send(const std::vector<char>&, const uint32_t);

        void disconnect();

        ~UnixSocketClient();
    };

    class UnixSocketServer : public Logging {
    private:
        struct UnixClient;

        UnixConnectionType *           connectionData_;

        ConnectionDefines::SocketServerStatus  status_;
        ConnectionDefines::SocketThreadStatus  thread_;

        std::vector<UnixClient>           client_list_;

        uint32_t CID_counter = 0;

    public:
        UnixSocketServer(const UnixConnectionType *, const std::string& = ".unix_log", const ConnectionDefines::SocketLoggingType& = ConnectionDefines::SocketLoggingType::disableLogging);
        UnixSocketServer(const UnixSocketServer&, const std::string& = ".unix_log", const ConnectionDefines::SocketLoggingType& = ConnectionDefines::SocketLoggingType::disableLogging);
        UnixSocketServer(const std::string& = ".unix_log", const ConnectionDefines::SocketLoggingType& = ConnectionDefines::SocketLoggingType::disableLogging);

        [[nodiscard]] UnixConnectionType             getUnixConnectionType() const;
        [[nodiscard]] ConnectionDefines::SocketServerStatus  getUnixStatus() const;
        [[nodiscard]] ConnectionDefines::SocketThreadStatus  getUnixThread() const;
        [[nodiscard]] ConnectionDefines::SocketLoggingType  getUnixLogging() const;

        void setUnixConnectionType(const UnixConnectionType *);
        void setUnixSocketServer(const UnixSocketServer&);
        void setUnixThreadStatus(const ConnectionDefines::SocketThreadStatus&);
        void setUnixLoggingType(const ConnectionDefines::SocketLoggingType&);

        void socketInit();
        void socketAccept();

        int32_t readTo(const uint32_t, void *, const uint32_t);
        int32_t readTo(const uint32_t, std::vector<char>&, const uint32_t);

        int32_t readAll(void *, const uint32_t);
        int32_t readAll(std::vector<char>&, const uint32_t);

        void sendTo(const uint32_t, const void *, const uint32_t);
        void sendTo(const uint32_t, const std::vector<char>&);

        void sendAll(const void *, const uint32_t);
        void sendAll(const std::vector<char>&);

        void disconnectTo(const uint32_t);
        void disconnectAll();

        ~UnixSocketServer();
    };

    struct UnixSocketServer::UnixClient : public Logging {
    private:
        ConnectionDefines::SocketClientStatus client_status_;

        UnixConnectionType *client_;

        uint32_t CID;
        uint8_t flag;

    public:
        UnixClient(const UnixConnectionType *, const uint32_t, const uint8_t, const std::string& = ".unix_log", const ConnectionDefines::SocketLoggingType& = ConnectionDefines::SocketLoggingType::disableLogging);
        UnixClient(const UnixClient&, const std::string& = ".unix_log", const ConnectionDefines::SocketLoggingType& = ConnectionDefines::SocketLoggingType::disableLogging);

        [[nodiscard]] UnixConnectionType                getConnectionType() const;
        [[nodiscard]] ConnectionDefines::SocketClientStatus     getStatus() const;
        [[nodiscard]] ConnectionDefines::SocketLoggingType getLoggingType() const;
        [[nodiscard]] uint32_t                                     getCID() const;

        void setConnectionType(const UnixConnectionType *);
        void setLoggingType(const ConnectionDefines::SocketLoggingType&);
        void setCID(const uint32_t);
        void setFlag(const uint8_t);

        int32_t socketRead(void *, const uint32_t);
        int32_t socketRead(std::vector<char>&, const uint32_t);

        void socketSend(const void *, const uint32_t);
        void socketSend(const std::vector<char>&, const uint32_t);

        void socketDisconnect();

        ~UnixClient();
    };
}

#endif // __SOCKET_H__