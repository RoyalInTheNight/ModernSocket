#include "../../include/socket/socket.h"
#include <vector>

using namespace RITNet;
using namespace RITNet::unixTypes;
using namespace RITNet::ConnectionDefines;

UnixSocketServer::UnixClient::UnixClient(const UnixConnectionType *conn_data, 
                                         const uint32_t CID, 
                                         const uint8_t flag, 
                                         const std::string& path, 
                                         const ConnectionDefines::SocketLoggingType& log) {
    this->setLoggingPath(path);
    this->setLogging(log);

    if (conn_data == nullptr) {
        this->client_status_ = ConnectionDefines::SocketClientStatus::errSocketConnect;

        this->logging("[ERROR]Constructor UnixClient(): connection data is nullptr");
    }

    else {
        this->client_ = (UnixConnectionType *)conn_data;

        this->logging("[  OK  ]Constructor UnixClient(): success");
    }
}

UnixSocketServer::UnixClient::UnixClient(const UnixClient&                        client, 
                                         const std::string&                         path, 
                                         const ConnectionDefines::SocketLoggingType& log) {
    this->setLoggingPath(path);
    this->setLogging(log);

    if (client.client_ == nullptr) {
        this->client_status_ = ConnectionDefines::SocketClientStatus::errSocketConnect;

        this->logging("[ERROR]Constructor UnixClient(): connection data is nullptr");
    }

    else {
        this->client_        = (UnixConnectionType *)client.client_;
        this->CID            = client.CID;
        this->flag           = client.flag;
        this->client_status_ = client.client_status_;

        this->logging("[  OK  ]Constructor UnixClient(): success");
    }
}

UnixConnectionType UnixSocketServer::UnixClient::getConnectionType() const { return *(this->client_); }
ConnectionDefines::SocketClientStatus UnixSocketServer::UnixClient::getStatus() const { return this->client_status_; }
ConnectionDefines::SocketLoggingType UnixSocketServer::UnixClient::getLoggingType() const { return this->getLogging(); }
uint32_t UnixSocketServer::UnixClient::getCID() const { return this->CID; }

void UnixSocketServer::UnixClient::setConnectionType(const UnixConnectionType *conn_data) {
    if (conn_data == nullptr) {
        this->client_status_ = SocketClientStatus::errSocketConnect;


        this->logging("[ERROR]Func setConnectionType(): connection data is nullptr");
    }

    else {
        this->client_ = (UnixConnectionType *)conn_data;

        this->logging("[  OK  ]Func setConnectionType(): success");
    }
}

void UnixSocketServer::UnixClient::setLoggingType(const ConnectionDefines::SocketLoggingType& log_type) {
    this->setLogging(log_type);
}

void UnixSocketServer::UnixClient::setCID(const uint32_t CID) {
    this->CID = CID;
}

void UnixSocketServer::UnixClient::setFlag(const uint8_t flag) {
    this->flag = flag;
}

int32_t UnixSocketServer::UnixClient::socketRead(void *buffer, const uint32_t size) {
    if (this->client_ == nullptr) {
        this->client_status_ = SocketClientStatus::errSocketDescriptor;
        this->logging("[ERROR]Func socketRead(): conn_data error");

        return -1;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketRead(): success");
    }

    if (this->client_->socket_ < 0) {
        this->client_status_ = SocketClientStatus::errSocketDescriptor;
        this->logging("[ERROR]Func socketRead(): invalid socket");
    
        return -1;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketRead(): success");
    }

    if (buffer == nullptr) {
        this->client_status_ = SocketClientStatus::errSocketRead;
        this->logging("[ERROR]Func socketRead(): buffer is nullptr");

        return -1;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketRead(): success");
    }

    int32_t bytes_read = ::recv(this->client_->socket_, buffer, size, 0);

    if (bytes_read < 0) {
        this->client_status_ = SocketClientStatus::errSocketRead;
        this->logging("[ERROR]Func socketRead(): read error");

        return bytes_read;
    }

    this->client_status_ = SocketClientStatus::ok;
    this->logging("[  OK  ]Func socketRead(): success");

    return bytes_read;
}

int32_t UnixSocketServer::UnixClient::socketRead(std::vector<char>& buffer, const uint32_t size) {
    if (this->client_ == nullptr) {
        this->client_status_ = SocketClientStatus::errSocketDescriptor;
        this->logging("[ERROR]Func socketRead(): conn_data error");

        return -1;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketRead(): success");
    }

    if (this->client_->socket_ < 0) {
        this->client_status_ = SocketClientStatus::errSocketDescriptor;
        this->logging("[ERROR]Func socketRead(): invalid socket");
    
        return -1;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketRead(): success");
    }

    int32_t bytes_read = ::recv(this->client_->socket_, buffer.data(), size, 0);

    if (bytes_read < 0) {
        this->client_status_ = SocketClientStatus::errSocketRead;
        this->logging("[ERROR]Func socketRead(): read error");

        return bytes_read;
    }

    this->client_status_ = SocketClientStatus::ok;
    this->logging("[  OK  ]Func socketRead(): success");

    return bytes_read;
}

void UnixSocketServer::UnixClient::socketSend(const void *buffer, const uint32_t size) {
    if (this->client_ == nullptr) {
        this->client_status_ = SocketClientStatus::errSocketDescriptor;
        this->logging("[ERROR]Func socketSend(): conn_data error");

        return;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketSend(): success");
    }

    if (this->client_->socket_ < 0) {
        this->client_status_ = SocketClientStatus::errSocketDescriptor;
        this->logging("[ERROR]Func socketSend(): invalid socket");

        return;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketSend(): success");
    }

    int32_t bytes_read = ::send(this->client_->socket_, buffer, size, 0);

    if (bytes_read < 0) {
        this->client_status_ = SocketClientStatus::errSocketSend;
        this->logging("[ERROR]Func socketSend(): send error");
    
        return;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketSend(): success");
    }
}

void UnixSocketServer::UnixClient::socketSend(const std::vector<char>& buffer, const uint32_t size) {
    if (this->client_ == nullptr) {
        this->client_status_ = SocketClientStatus::errSocketDescriptor;
        this->logging("[ERROR]Func socketSend(): conn_data error");

        return;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketSend(): success");
    }

    if (this->client_->socket_ < 0) {
        this->client_status_ = SocketClientStatus::errSocketDescriptor;
        this->logging("[ERROR]Func socketSend(): invalid socket");

        return;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketSend(): success");
    }

    int32_t bytes_read = ::send(this->client_->socket_, buffer.data(), size, 0);

    if (bytes_read < 0) {
        this->client_status_ = SocketClientStatus::errSocketSend;
        this->logging("[ERROR]Func socketSend(): send error");
    
        return;
    }

    else {
        this->client_status_ = SocketClientStatus::ok;

        this->logging("[  OK  ]Func socketSend(): success");
    }
}

#include <unistd.h>

void UnixSocketServer::UnixClient::socketDisconnect() {
    close(this->client_->socket_);
}

UnixSocketServer::UnixClient::~UnixClient() { this->socketDisconnect(); }