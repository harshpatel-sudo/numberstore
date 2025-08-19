#ifndef RESPONSE_HXX
#define RESPONSE_HXX

#include "Message.hxx"
#include "../utils/ErrorCodes.hxx"

namespace NumberStore {
    enum class ResponseType {
        SUCCESS,
        ERROR_RESPONSE,
        DATA
    };

    class Response : public Message {
    private:
        ResponseType responseType;
        ErrorCode errorCode;
        std::string data;

    public:
        Response(ResponseType respType, ErrorCode code, const std::string& responseData = "");
        
        ResponseType getResponseType() const;
        ErrorCode getErrorCode() const;
        const std::string& getData() const;
        bool isSuccess() const;
        
        std::string serialize() const override;
        static std::unique_ptr<Response> deserialize(const std::string& content);
        
        // Factory methods for creating specific responses
        static std::unique_ptr<Response> createSuccessResponse(const std::string& message = "");
        static std::unique_ptr<Response> createErrorResponse(ErrorCode code, const std::string& message = "");
        static std::unique_ptr<Response> createDataResponse(const std::string& data);
        
    private:
        static ResponseType stringToResponseType(const std::string& str);
        static std::string responseTypeToString(ResponseType type);
    };
}

#endif // RESPONSE_HXX
