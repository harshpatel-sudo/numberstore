#include "Response.hxx"
#include "../utils/Constants.hxx"
#include "../utils/Logger.hxx"
#include <sstream>

namespace NumberStore {
    Response::Response(ResponseType respType, ErrorCode code, const std::string& responseData)
        : Message(MessageType::RESPONSE, ""), responseType(respType), errorCode(code), data(responseData) {
    }

    ResponseType Response::getResponseType() const {
        return responseType;
    }

    ErrorCode Response::getErrorCode() const {
        return errorCode;
    }

    const std::string& Response::getData() const {
        return data;
    }

    bool Response::isSuccess() const {
        return responseType == ResponseType::SUCCESS || responseType == ResponseType::DATA;
    }

    std::string Response::serialize() const {
        std::ostringstream oss;
        oss << "RESP:" << responseTypeToString(responseType);
        
        if (responseType == ResponseType::ERROR_RESPONSE) {
            oss << " " << static_cast<int>(errorCode);
        }
        
        if (!data.empty()) {
            if (responseType == ResponseType::DATA) {
                oss << "\n" << data;  // Put data on new line for DATA responses
            } else {
                oss << " " << data;   // Space separator for other responses
            }
        }
        
        return oss.str();
    }

    std::unique_ptr<Response> Response::deserialize(const std::string& content) {
        std::istringstream iss(content);
        std::string responseStr;
        iss >> responseStr;

        ResponseType respType = stringToResponseType(responseStr);
        
        if (respType == ResponseType::ERROR_RESPONSE) {
            int errorCodeInt;
            if (!(iss >> errorCodeInt)) {
                Logger::getInstance().error("Missing error code for error response");
                return nullptr;
            }
            
            std::string remainingData;
            std::getline(iss, remainingData);
            if (!remainingData.empty() && remainingData[0] == ' ') {
                remainingData = remainingData.substr(1); // Remove leading space
            }
            
            return std::make_unique<Response>(respType, static_cast<ErrorCode>(errorCodeInt), remainingData);
        } else {
            std::string remainingData;
            if (respType == ResponseType::DATA) {
                // For DATA responses, read everything after the first line
                std::string line;
                while (std::getline(iss, line)) {
                    if (!remainingData.empty()) {
                        remainingData += "\n";
                    }
                    remainingData += line;
                }
            } else {
                // For other responses, read the rest of the current line
                std::getline(iss, remainingData);
                if (!remainingData.empty() && remainingData[0] == ' ') {
                    remainingData = remainingData.substr(1); // Remove leading space
                }
            }
            
            return std::make_unique<Response>(respType, ErrorCode::SUCCESS, remainingData);
        }
    }

    std::unique_ptr<Response> Response::createSuccessResponse(const std::string& message) {
        return std::make_unique<Response>(ResponseType::SUCCESS, ErrorCode::SUCCESS, message);
    }

    std::unique_ptr<Response> Response::createErrorResponse(ErrorCode code, const std::string& message) {
        std::string errorMessage = message.empty() ? ErrorHandler::getErrorMessage(code) : message;
        return std::make_unique<Response>(ResponseType::ERROR_RESPONSE, code, errorMessage);
    }

    std::unique_ptr<Response> Response::createDataResponse(const std::string& data) {
        return std::make_unique<Response>(ResponseType::DATA, ErrorCode::SUCCESS, data);
    }

    ResponseType Response::stringToResponseType(const std::string& str) {
        if (str == Constants::RESP_SUCCESS) return ResponseType::SUCCESS;
        if (str == Constants::RESP_ERROR) return ResponseType::ERROR_RESPONSE;
        if (str == Constants::RESP_DATA) return ResponseType::DATA;
        
        Logger::getInstance().error("Unknown response type: " + str);
        return ResponseType::ERROR_RESPONSE; // Default fallback
    }

    std::string Response::responseTypeToString(ResponseType type) {
        switch (type) {
            case ResponseType::SUCCESS: return Constants::RESP_SUCCESS;
            case ResponseType::ERROR_RESPONSE: return Constants::RESP_ERROR;
            case ResponseType::DATA: return Constants::RESP_DATA;
            default: return Constants::RESP_ERROR;
        }
    }
}
