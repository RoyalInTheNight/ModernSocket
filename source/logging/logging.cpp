#include "../../include/socket/socket.h"
#include <string>
#include <fstream>
#include <iostream>

void RITNet::Logging::setLoggingPath(const std::string& path) {
    this->log_path_ = path;
}

void RITNet::Logging::setLogging(const ConnectionDefines::SocketLoggingType& log_type_) {
    this->logging_ = log_type_;
}

RITNet::ConnectionDefines::SocketLoggingType RITNet::Logging::getLogging() const {
    return this->logging_;
}

void RITNet::Logging::logging(const std::string& log_msg_) {
    if (this->logging_ == RITNet::ConnectionDefines::SocketLoggingType::stdoutLogging)
        std::cout << log_msg_ << std::endl;

    if (this->logging_ == RITNet::ConnectionDefines::SocketLoggingType::fileLogging) {
        std::ofstream file(this->log_path_);

        if (file.is_open())
            file << log_msg_ << std::endl;

        file.close();
    }
}