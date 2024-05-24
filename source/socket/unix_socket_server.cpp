#include "../../include/socket/socket.h"
#include <string>
#include <sys/socket.h>
#include <vector>

using namespace RITNet;
using namespace RITNet::unixTypes;
using namespace RITNet::ConnectionDefines;

UnixSocketServer::UnixSocketServer(const UnixConnectionType *connection, 
                                   const std::string& log_path, 
                                   const ConnectionDefines::SocketLoggingType& log_type) {
    this->setLoggingPath(log_path);
    this->setLogging(log_type);

    if (connection == nullptr) {
        this->status_ = SocketServerStatus::errSocketDescriptor;

        this->logging("[ERROR]Constructor UnixSocketServer(): UnixConnectionType == nullptr");
    }

    else {
        this->status_         = SocketServerStatus::ok;
        this->connectionData_ = (UnixConnectionType *)connection;

        this->logging("[  OK  ]Constructor UnixSocketServer(): success");
    }
}

UnixSocketServer::UnixSocketServer(const UnixSocketServer&                         copy, 
                                   const std::string&                          log_path, 
                                   const ConnectionDefines::SocketLoggingType& log_type) {
    this->setLoggingPath(log_path);
    this->setLogging(log_type);

    if (copy.connectionData_ == nullptr) {
        this->status_ = SocketServerStatus::errSocketDescriptor;

        this->logging("[ERROR]Constructor UnixSocketServer(): UnixConnectionType == nullptr");
    }

    else {
        this->status_         = SocketServerStatus::ok;
        this->connectionData_ = copy.connectionData_;
        this->CID_counter     = copy.CID_counter;
        this->client_list_    = copy.client_list_;
        this->thread_         = copy.thread_;

        this->logging("[  OK  ]Constructor UnixSocketServer(): success");
    }
}

UnixSocketServer::UnixSocketServer(const std::string& log_path, const ConnectionDefines::SocketLoggingType& log_type) {
    this->setLoggingPath(log_path);
    this->setLogging(log_type);

    this->logging("[  OK  ]Constructor UnixSocketServer(): success");
}

UnixConnectionType UnixSocketServer::getUnixConnectionType() const { return *(this->connectionData_); }
SocketServerStatus UnixSocketServer::getUnixStatus()         const { return   this->status_;          }
SocketThreadStatus UnixSocketServer::getUnixThread()         const { return   this->thread_;          }
SocketLoggingType  UnixSocketServer::getUnixLogging()        const { return   this->getLogging();     }

void UnixSocketServer::setUnixConnectionType(const UnixConnectionType *connection) {
    if (connection == nullptr) {
        this->status_ = SocketServerStatus::errSocketDescriptor;

        this->logging("[ERROR]Func setUnixConnectionType(): UnixConnectionType == nullptr");
    }

    else {
        this->status_         = SocketServerStatus::ok;
        this->connectionData_ = (UnixConnectionType *)connection;

        this->logging("[  OK  ]Func setUnixConnectionType(): success");
    }
}

void UnixSocketServer::setUnixSocketServer(const UnixSocketServer& server) {
    if (server.connectionData_ == nullptr) {
        this->status_ = SocketServerStatus::errSocketDescriptor;

        this->logging("[ERROR]Func setUnixSocketServer(): UnixConnectionType == nullptr");
    }

    else {
        this->status_         = SocketServerStatus::ok;
        this->connectionData_ = server.connectionData_;
        this->CID_counter     = server.CID_counter;
        this->client_list_    = server.client_list_;
        this->thread_         = server.thread_;

        this->logging("[  OK  ]Func setUnixSocketServer(): success");
    }
}

void UnixSocketServer::setUnixThreadStatus(const ConnectionDefines::SocketThreadStatus& thread) { this->thread_ = thread; }
void UnixSocketServer::setUnixLoggingType(const ConnectionDefines::SocketLoggingType& log) { this->setLogging(log); }

#include <iostream>
#include <unistd.h>

void UnixSocketServer::socketInit() {
    if (this->connectionData_->socket_ < 0) {
        this->status_ = SocketServerStatus::errSocketDescriptor;
        this->logging("[FAILED]Func socketInit(): invalid socket");
    
        return;
    }

    else {
        this->status_ = SocketServerStatus::ok;
        this->logging("[  OK  ]Func socketInit(): configure socket success");
    }

    int32_t _ = ::bind(this->connectionData_->socket_, (sockaddr *)&this->connectionData_->header_, 
                      this->connectionData_->headerSize_);

    if (_ < 0) {
        this->status_ = SocketServerStatus::errSocketBind;
        this->logging("[FAILED]Func socketInit(): bind error");

        return;
    }

    else {
        this->status_ = SocketServerStatus::ok;
        this->logging("[  OK  ]Func socketInit(): bind success");
    }

    _ = ::listen(this->connectionData_->socket_, 1024);

    if (_ < 0) {
        this->status_ = SocketServerStatus::errSocketListen;
        this->logging("[FAILED]Func socketInit(): listen error");
    
        return;
    }

    else {
        this->status_ = SocketServerStatus::ok;
        this->logging("[  OK  ]Func socketInit(): listen success");
    }

    if (this->status_ != SocketServerStatus::ok) {
        this->logging("[FAILED]Func socketInit(): failed");
    
        return;
    }

    else
        this->logging("[  OK  ]Func socketInit(): success");
}

void UnixSocketServer::socketAccept() {
    if (this->connectionData_->socket_ < 0) {
        this->status_ = ConnectionDefines::SocketServerStatus::errSocketDescriptor;

        this->logging("[FAILED]Func socketAccept(): invalid socket");
    }

    else {
        this->status_ = ConnectionDefines::SocketServerStatus::ok;

        this->logging("[  OK  ]Func socketAccept(): success");
    }

    if (this->thread_ == ConnectionDefines::SocketThreadStatus::threadDisable) {
        socket_t acceptSocket_ = ::accept(this->connectionData_->socket_, nullptr, nullptr);

        if (acceptSocket_ < 0) {
            this->status_ = SocketServerStatus::errSocketAccept;

            this->logging("[FAILED]Func socketAccept(): accept connection failed");
        }

        else {
            client_list_
                .push_back(
                    UnixClient(
                        new UnixConnectionType(
                            this->connectionData_->header_.sun_path, 
                            acceptSocket_
                        ), CID_counter, 0, "." + std::to_string(
                                                CID_counter
                                           ) + "log", 
                                           
                                           this->getUnixLogging(

                                           )
                    )
                );
        
            ++CID_counter;

            this->status_ = SocketServerStatus::ok;
            this->logging("[  OK  ]Func socketAccept(): success");
        }

        return;
    }

    else {
        // to dev

        return;
    }
}

int32_t UnixSocketServer::readTo(const uint32_t CID, void *buffer, const uint32_t size) {
    for (auto& _: client_list_)
        if (_.getCID() == CID)
            return _.socketRead(buffer, size);

    return -1;
}

int32_t UnixSocketServer::readTo(const uint32_t CID, std::vector<char>& buffer, const uint32_t size) {
    for (auto& _: client_list_)
        if (_.getCID() == CID)
            return _.socketRead(buffer, size);

    return -1;
}

int32_t UnixSocketServer::readAll(std::vector<char>& buffer, const uint32_t size) {
    if (this->thread_ == SocketThreadStatus::threadDisable) {
        int32_t count = 0;

        for (auto& _: client_list_)
            if (_.socketRead(buffer, size))
                ++count;

        if (count != client_list_.size())
            return 0;

        else
            return 1;
    }

    else {
        int32_t count = 0;

        // to dev

        if (count != client_list_.size())
            return 0;

        else
            return 1;
    }

    return 0;
}

int32_t UnixSocketServer::readAll(void *buffer, const uint32_t size) {
    if (this->thread_ == SocketThreadStatus::threadDisable) {
        int32_t count = 0;

        for (auto& _: client_list_)
            if (_.socketRead(buffer, size))
                ++count;

        if (count != client_list_.size())
            return 0;

        else
            return 1;
    }


    else {
        int32_t count = 0;

        // to dev

        if (count != client_list_.size())
            return 0;

        else
            return 1;
    }
}

void UnixSocketServer::sendTo(const uint32_t CID, const void *buffer, const uint32_t size) {
    for (auto& _: client_list_)
        if (_.getCID() == CID)
            return _.socketSend(buffer, size);
}

void UnixSocketServer::sendTo(const uint32_t CID, const std::vector<char>& buffer) {
    for (auto& _: client_list_)
        if (_.getCID() == CID)
            return _.socketSend(buffer, buffer.size());
}

void UnixSocketServer::sendAll(const void *buffer, const uint32_t size) {
    for (auto& _: client_list_)
        _.socketSend(buffer, size);
}

void UnixSocketServer::sendAll(const std::vector<char>& buffer) {
    for (auto& _: client_list_)
        _.socketSend(buffer, buffer.size());
}

void UnixSocketServer::disconnectTo(const uint32_t CID) {
    for (auto& _: client_list_)
        if (_.getCID() == CID)
            _.socketDisconnect();
}

void UnixSocketServer::disconnectAll() {
    for (auto& _: client_list_)
        _.socketDisconnect();
}

UnixSocketServer::~UnixSocketServer() { this->disconnectAll(); }