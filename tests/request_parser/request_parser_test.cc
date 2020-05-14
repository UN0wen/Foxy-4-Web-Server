#include "gtest/gtest.h"
#include "request_parser.h"
#include "request.h"
#include "header.h"
class RequestParserTest : public ::testing::Test
{
protected:
    RequestParser parser;
    Request request;
    RequestParser::result_type result;

    const char *get_request = "GET / HTTP/1.1\r\n\
Host: localhost:8000\r\n\
User-Agent: HTTPie/0.9.8\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept: */*\r\n\
Connection: keep-alive\r\n\r\n";

    const int GET_REQUEST_LENGTH = 135;

    const char *post_request = "POST / HTTP/1.1\r\n\
Accept: application/json, */*\r\n\
Accept-Encoding: gzip, deflate\r\n\
Connection: keep-alive\r\n\
Content-Length: 19\r\n\
Content-Type: application/json\r\n\
Host: localhost:8000\r\n\
User-Agent: HTTPie/0.9.8\r\n\r\n\
{'test_value': '1'}";

    const int POST_REQUEST_LENGTH = 225;
    const int CONTENT_LENGTH = 19;
};

// Parser will need a reset if it's being reused
TEST_F(RequestParserTest, ParserReusedWithoutReset)
{
    const char *post_data = post_request;
    RequestParser::result_type result_no_reset;
    Request request2;

    std::tie(result, std::ignore) = parser.parse(request,
                                                 post_data,
                                                 post_data + strlen(post_data));

    std::tie(result_no_reset, std::ignore) = parser.parse(request2,
                                                          post_data,
                                                          post_data + strlen(post_data));

    EXPECT_FALSE(result_no_reset == RequestParser::result_type::good);
}

TEST_F(RequestParserTest, ParserReusedWithReset)
{
    const char *post_data = post_request;
    RequestParser::result_type result_with_reset;
    Request request2;

    std::tie(result, std::ignore) = parser.parse(request,
                                                 post_data,
                                                 post_data + strlen(post_data));

    parser.reset();
    std::tie(result_with_reset, std::ignore) = parser.parse(request2,
                                                            post_data,
                                                            post_data + strlen(post_data));

    EXPECT_TRUE(result_with_reset == RequestParser::result_type::good);
}

// The get request length is correctly calculated
TEST_F(RequestParserTest, GetRequestLength)
{
    const char *get_data = get_request;

    std::tie(result, std::ignore) = parser.parse(request,
                                                 get_data,
                                                 get_data + strlen(get_data));

    EXPECT_TRUE(parser.get_char_amount() == 135);
}

// The URI (uniform resource identifier) is parsed correctly
TEST_F(RequestParserTest, GetRequestURI)
{
    const char *get_data = get_request;
    std::tie(result, std::ignore) = parser.parse(request,
                                                 get_data,
                                                 get_data + strlen(get_data));

    EXPECT_TRUE(request.uri_ == "/");
    parser.reset();
    const char *get_link_data = "GET /web/test/index.html HTTP/1.1\r\n\r\n";
    Request get_link_request;

    std::tie(result, std::ignore) = parser.parse(get_link_request,
                                                 get_link_data,
                                                 get_link_data + strlen(get_link_data));
    std::cerr << get_link_request.uri_ << std::endl;
    EXPECT_TRUE(get_link_request.uri_ == "/web/test/index.html");
}

// Http versions are parsed correctly
TEST_F(RequestParserTest, GetRequestHttpVersion)
{
    const char *get_data = get_request;
    std::tie(result, std::ignore) = parser.parse(request,
                                                 get_data,
                                                 get_data + strlen(get_data));

    //EXPECT_TRUE(request.http_version_minor == 1 && request.http_version_major == 1);
    EXPECT_TRUE(request.http_version_[2] == '1' && request.http_version_[0] == '1');
}

// Checking headers are parsed correctly
TEST_F(RequestParserTest, GetRequestHeaders)
{
    const char *get_data = get_request;
    std::tie(result, std::ignore) = parser.parse(request,
                                                 get_data,
                                                 get_data + strlen(get_data));
    bool host, user_agent, accept_encoding, accept, connection = false;

    for (auto it = request.headers_.begin(); it != request.headers_.end(); ++it)
    {
        if (it->first == "Host" && it->second == "localhost:8000")
            host = true;
        else if (it->first == "User-Agent" && it->second == "HTTPie/0.9.8")
            user_agent = true;
        else if (it->first == "Accept-Encoding" && it->second == "gzip, deflate")
            accept_encoding = true;
        else if (it->first == "Accept" && it->second == "*/*")
            accept = true;
        else if (it->first == "Connection" && it->second == "keep-alive")
            connection = true;
    }

    EXPECT_TRUE(host && user_agent && accept_encoding && accept && connection);
}

// Every header must have a name
TEST_F(RequestParserTest, RequestHeaderNoName)
{
    const char *no_header_get_data = "GET / HTTP/1.1\r\n\
: localhost:8000\r\n\r\n";
    std::tie(result, std::ignore) = parser.parse(request,
                                                 no_header_get_data,
                                                 no_header_get_data + strlen(no_header_get_data));

    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

// Headers can have no value
TEST_F(RequestParserTest, RequestHeaderNoValue)
{
    const char *no_value_get_data = "GET / HTTP/1.1\r\n\
Content-Length: \r\n\r\n";
    std::tie(result, std::ignore) = parser.parse(request,
                                                 no_value_get_data,
                                                 no_value_get_data + strlen(no_value_get_data));

    EXPECT_TRUE(result == RequestParser::result_type::good);
}

// Correctly parse the length of a typical post request
TEST_F(RequestParserTest, PostRequestLength)
{
    const char *post_data = post_request;

    std::tie(result, std::ignore) = parser.parse(request,
                                                 post_data,
                                                 post_data + strlen(post_data));

    int content_length;
    for (auto it = request.headers_.begin(); it != request.headers_.end(); ++it)
    {
        if (it->first == "Content-Length")
        {
            content_length = std::stoi(it->second);
            break;
        }
    }

    EXPECT_TRUE(content_length == CONTENT_LENGTH);
    EXPECT_TRUE((parser.get_char_amount() + content_length == POST_REQUEST_LENGTH));
}

// Correctly parse Http Methods
TEST_F(RequestParserTest, HttpMethodCorrectness)
{
    const char *post_data = post_request;
    Request request2;
    const char *get_data = get_request;
    std::tie(result, std::ignore) = parser.parse(request,
                                                 post_data,
                                                 post_data + strlen(post_data));
    parser.reset();

    std::tie(result, std::ignore) = parser.parse(request2,
                                                 get_data,
                                                 get_data + strlen(get_data));
    EXPECT_FALSE(request2.method_ == request.method_);
    EXPECT_TRUE(request.method_ == Request::POST);
    EXPECT_TRUE(request2.method_ == Request::GET);
}

// Http Method must be present
TEST_F(RequestParserTest, NoHttpMethod)
{
    const char *no_method_data = " / HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 no_method_data,
                                                 no_method_data + strlen(no_method_data));

    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

// Some characters are not supported in header names and method names
// list of supported characters: ascii 0-127
TEST_F(RequestParserTest, NotSupportedCharacters)
{
    const char *not_supported_data = "GET / HTTP/1.1\r\n\
HÖst: Ö is not supported\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 not_supported_data,
                                                 not_supported_data + strlen(not_supported_data));

    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

// http version section must strictly be HTTP/{major}.{minor}
TEST_F(RequestParserTest, WrongHttpCapitalizationH)
{
    const char *http_format_data = "GET / hTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 http_format_data,
                                                 http_format_data + strlen(http_format_data));

    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

TEST_F(RequestParserTest, WrongHttpCapitalizationFirstT)
{
    const char *http_format_data = "GET / HtTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 http_format_data,
                                                 http_format_data + strlen(http_format_data));

    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

TEST_F(RequestParserTest, WrongHttpCapitalizationSecondT)
{
    const char *http_format_data = "GET / HTtP/1.1\r\n\
Host: localhost:8000\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 http_format_data,
                                                 http_format_data + strlen(http_format_data));

    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

TEST_F(RequestParserTest, WrongHttpCapitalizationP)
{
    const char *http_format_data = "GET / HTTp/1.1\r\n\
Host: localhost:8000\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 http_format_data,
                                                 http_format_data + strlen(http_format_data));

    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

TEST_F(RequestParserTest, HttpVersionMajorAbsent)
{
    // no major
    const char *major_format_data = "GET / HTTP/.1\r\n\
Host: localhost:8000\r\n\r\n";
    RequestParser::result_type major_format_result;

    std::tie(major_format_result, std::ignore) = parser.parse(request,
                                                              major_format_data,
                                                              major_format_data + strlen(major_format_data));

    EXPECT_TRUE(major_format_result == RequestParser::result_type::bad);
}

TEST_F(RequestParserTest, HttpVersionMinorAbsent)
{
    // no minor
    const char *minor_format_data = "GET / HTTP/1.\r\n\
Host: localhost:8000\r\n\r\n";
    RequestParser::result_type minor_format_result;

    std::tie(minor_format_result, std::ignore) = parser.parse(request,
                                                              minor_format_data,
                                                              minor_format_data + strlen(minor_format_data));

    EXPECT_TRUE(minor_format_result == RequestParser::result_type::bad);
}

TEST_F(RequestParserTest, WrongHttpFormatMajor)
{
    // no other characters in major
    const char *wrong_format_data = "GET / HTTP/1a.1\r\n\
Host: localhost:8000\r\n\r\n";
    RequestParser::result_type wrong_format_result;

    std::tie(wrong_format_result, std::ignore) = parser.parse(request,
                                                              wrong_format_data,
                                                              wrong_format_data + strlen(wrong_format_data));

    EXPECT_TRUE(wrong_format_result == RequestParser::result_type::bad);
}

TEST_F(RequestParserTest, WrongHttpFormatMinor)
{
    // no other characters in minor
    const char *wrong_format_data = "GET / HTTP/1.1a\r\n\
Host: localhost:8000\r\n\r\n";
    RequestParser::result_type wrong_format_result;

    std::tie(wrong_format_result, std::ignore) = parser.parse(request,
                                                              wrong_format_data,
                                                              wrong_format_data + strlen(wrong_format_data));

    EXPECT_TRUE(wrong_format_result == RequestParser::result_type::bad);
}

// Support multiple digits of HTTP version major/minor
TEST_F(RequestParserTest, HttpVersionMultipleDigits)
{
    const char *http_format_data = "GET / HTTP/11.11\r\n\
Host: localhost:8000\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 http_format_data,
                                                 http_format_data + strlen(http_format_data));

    //EXPECT_TRUE(request.http_version_major == 11);
    EXPECT_TRUE(request.http_version_[0] == '1' && request.http_version_[1] == '1');
    //EXPECT_TRUE(request.http_version_minor == 11);
    EXPECT_TRUE(request.http_version_[3] == '1' && request.http_version_[4] == '1');
}

// Correctly skip whitespace in front of header names if there are any
TEST_F(RequestParserTest, SkipWhitespaceBeforeHeader)
{
    const char *whitespace_data = "GET / HTTP/1.1\r\n\
Host: localhost:8000\r\n\
\t\tUser-Agent: HTTPie/0.9.8\r\n\
  Accept-Encoding: gzip, deflate\r\n\
  \tAccept: */*\r\n\
\t  \tConnection: keep-alive\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 whitespace_data,
                                                 whitespace_data + strlen(whitespace_data));

    EXPECT_TRUE(result == RequestParser::result_type::good);
}

// No whitespace on first line of header
TEST_F(RequestParserTest, WhitespaceBeforeFirstHeader)
{
    const char *wrong_whitespace_data = "GET / HTTP/1.1\r\n\
\tHost: localhost:8000\r\n\
User-Agent: HTTPie/0.9.8\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept: */*\r\n\
Connection: keep-alive\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 wrong_whitespace_data,
                                                 wrong_whitespace_data + strlen(wrong_whitespace_data));

    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

// Header fields must have 1 space after the colon
TEST_F(RequestParserTest, NoSpaceAfterHeader)
{
    const char *no_whitespace_data = "GET / HTTP/1.1\r\n\
Host:localhost:8000\r\n\r\n";

    std::tie(result, std::ignore) = parser.parse(request,
                                                 no_whitespace_data,
                                                 no_whitespace_data + strlen(no_whitespace_data));

    EXPECT_TRUE(result == RequestParser::result_type::bad);
}

// needs a slash between HTTP and version
TEST_F(RequestParserTest, HttpVersionNoSlash)
{

    const char *no_slash_data = "GET / HTTP1.1\r\n\
Host: localhost:8000\r\n\r\n";
    RequestParser::result_type no_slash_result;

    std::tie(no_slash_result, std::ignore) = parser.parse(request,
                                                          no_slash_data,
                                                          no_slash_data + strlen(no_slash_data));

    EXPECT_TRUE(no_slash_result == RequestParser::result_type::bad);
}

// No special characters in URI
TEST_F(RequestParserTest, SpecialCharactersInURI)
{

    const char *uri_special_data = "GET /www/da\nta/ HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";
    RequestParser::result_type uri_special_result;

    std::tie(uri_special_result, std::ignore) = parser.parse(request,
                                                             uri_special_data,
                                                             uri_special_data + strlen(uri_special_data));

    EXPECT_TRUE(uri_special_result == RequestParser::result_type::bad);
}

// Header field can be completely empty with a space followed by CRLF (as long as there are 1 valid header field)
TEST_F(RequestParserTest, SpaceBetweenHeaders)
{

    const char *space_header_data = "GET / HTTP/1.1\r\n\
Host: localhost:8000\r\n\
\t\t\r\n\
  \r\n\
  \t\r\n\
\t  \t\r\n\r\n";

    RequestParser::result_type space_header_result;

    std::tie(space_header_result, std::ignore) = parser.parse(request,
                                                              space_header_data,
                                                              space_header_data + strlen(space_header_data));
    std::cerr << request.headers_.begin()->first << std::endl;
    EXPECT_TRUE(space_header_result == RequestParser::result_type::good);
}

// Incomplete CRLF (\r without \n) is bad
TEST_F(RequestParserTest, IncompleteCRLF)
{

    const char *incomplete_crlf_data = "GET / HTTP/1.1\r\
Host: localhost:8000\r\n\r\n";
    RequestParser::result_type incomplete_crlf_result;

    std::tie(incomplete_crlf_result, std::ignore) = parser.parse(request,
                                                                 incomplete_crlf_data,
                                                                 incomplete_crlf_data + strlen(incomplete_crlf_data));

    EXPECT_TRUE(incomplete_crlf_result == RequestParser::result_type::bad);
}

// Incomplete CRLF (\r without \n) is bad, even in headers
TEST_F(RequestParserTest, IncompleteCRLFHeader)
{

    const char *incomplete_crlf_data = "GET / HTTP/1.1\r\n\
Host: localhost:8000\r\r\n";
    RequestParser::result_type incomplete_crlf_result;

    std::tie(incomplete_crlf_result, std::ignore) = parser.parse(request,
                                                                 incomplete_crlf_data,
                                                                 incomplete_crlf_data + strlen(incomplete_crlf_data));

    EXPECT_TRUE(incomplete_crlf_result == RequestParser::result_type::bad);
}

// No control characters in Header name
TEST_F(RequestParserTest, ControlCharactersHeaderName)
{

    const char *wrong_header_data = "GET / HTTP/1.1\r\n\
Ho\nst: localhost:8000\r\n\r\n";
    RequestParser::result_type wrong_header_result;

    std::tie(wrong_header_result, std::ignore) = parser.parse(request,
                                                              wrong_header_data,
                                                              wrong_header_data + strlen(wrong_header_data));

    EXPECT_TRUE(wrong_header_result == RequestParser::result_type::bad);
}

// No control characters in Header value (beside \r at the end)
TEST_F(RequestParserTest, ControlCharactersHeaderValue)
{

    const char *wrong_header_data = "GET / HTTP/1.1\r\n\
Host: local\nhost:8000\r\n\r\n";
    RequestParser::result_type wrong_header_result;

    std::tie(wrong_header_result, std::ignore) = parser.parse(request,
                                                              wrong_header_data,
                                                              wrong_header_data + strlen(wrong_header_data));

    EXPECT_TRUE(wrong_header_result == RequestParser::result_type::bad);
}

// Cannot parse empty request
TEST_F(RequestParserTest, EmptyRequest)
{
    const char *empty_data = "";
    std::tie(result, std::ignore) = parser.parse(request,
                                                 empty_data,
                                                 empty_data + strlen(empty_data));

    EXPECT_TRUE(result == RequestParser::result_type::indeterminate);
}

// Indeterminate when parser does not reach end state but runs out of string to parse
TEST_F(RequestParserTest, NotReachEndState)
{
    const char *incomplete_data = "GET / HTTP/1.1\r\n\r\n";
    std::tie(result, std::ignore) = parser.parse(request,
                                                 incomplete_data,
                                                 incomplete_data + 1);

    EXPECT_TRUE(result == RequestParser::result_type::indeterminate);
}

// Indeterminate when parser is not given an end
TEST_F(RequestParserTest, NotEndState)
{
    const char *incomplete_data = "GET / HTTP/1.1\r\n\r\n";
    std::tie(result, std::ignore) = parser.parse(request,
                                                 incomplete_data,
                                                 incomplete_data);

    EXPECT_TRUE(result == RequestParser::result_type::indeterminate);
}

TEST_F(RequestParserTest, InvalidHttpParseTest)
{
    char msg[] = "hello\n";
    RequestParser::result_type result = parser.parse_data(request, msg, 6);
    EXPECT_TRUE(result == RequestParser::bad);
}

TEST_F(RequestParserTest, PostRequestWithData)
{
    RequestParser::result_type result = parser.parse_data(request, post_request, POST_REQUEST_LENGTH);

    EXPECT_EQ(result, RequestParser::result_type::good);
}

TEST_F(RequestParserTest, PostRequestNoData)
{
    const char *data_request = "POST / HTTP/1.1\r\n\
Accept: application/json, */*\r\n\
Accept-Encoding: gzip, deflate\r\n\
Connection: keep-alive\r\n\
Content-Length: 19\r\n\
Content-Type: application/json\r\n\
Host: localhost:8000\r\n\
User-Agent: HTTPie/0.9.8\r\n\r\n";
    RequestParser::result_type result = parser.parse_data(request, data_request, POST_REQUEST_LENGTH + CONTENT_LENGTH);

    EXPECT_EQ(result, RequestParser::result_type::missing_data);
}

TEST_F(RequestParserTest, EmptyData)
{
    const char* empty_data;

    RequestParser::result_type result = parser.parse_data(request, empty_data, 0);

    EXPECT_EQ(result, RequestParser::result_type::indeterminate);
}
