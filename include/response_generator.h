#include "response.h"

class ResponseGenerator
{
public:
    ResponseGenerator();
    void init(Response response_);
    Response::status_code code_;
    /// The headers to be included in the reply.
    //std::vector<Header> headers;
    std::map<std::string, std::string> headers_;

    /// The content to be sent in the reply.
    std::string body_;

    /// Convert the reply into a vector of buffers. The buffers do not own the
    /// underlying memory blocks, therefore the reply object must remain valid and
    /// not be changed until the write operation has completed.
    std::vector<boost::asio::const_buffer> to_buffers();

    /// Get a stock reply.
    static Response stock_response(Response::status_code status);

    static boost::asio::const_buffer status_string_accessor(Response::status_code status);
    
};