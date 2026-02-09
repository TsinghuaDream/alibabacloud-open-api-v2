// This file is auto-generated, don't edit it. Thanks.
#ifndef ALIBABACLOUD_EXCEPTIONS_ALIBABACLOUDEXCEPTION_HPP_
#define ALIBABACLOUD_EXCEPTIONS_ALIBABACLOUDEXCEPTION_HPP_
#include <darabonba/Core.hpp>
#include <darabonba/Exception.hpp>
using namespace std;
using json = nlohmann::json;
namespace AlibabaCloud
{
namespace OpenApi
{
namespace Exceptions
{
  class AlibabaCloudException : public Darabonba::ResponseException {
  public:
    friend void from_json(const Darabonba::Json& j, AlibabaCloudException& obj) { 
      // Parent class ResponseException already handles these fields
      // Just extract requestId which is specific to AlibabaCloud
      if (j.contains("requestId") && !j["requestId"].is_null()) {
        obj.requestId_ = j["requestId"].get<std::string>();
      }
    };
    
    // Override parent's to_json to include statusCode and requestId
    friend void to_json(Darabonba::Json& j, const AlibabaCloudException& obj) {
      j = Darabonba::Json{
        {"code", obj.code_},
        {"message", obj.message_},
        {"data", obj.data_},
        {"description", obj.description_},
        {"accessDeniedDetail", obj.accessDeniedDetail_},
        {"statusCode", obj.statusCode_},
        {"requestId", obj.requestId_}
      };
    };
    
    AlibabaCloudException() ;
    AlibabaCloudException(const AlibabaCloudException &) = default ;
    AlibabaCloudException(AlibabaCloudException &&) = default ;
    AlibabaCloudException(const Darabonba::Json & obj) : Darabonba::ResponseException(obj) { from_json(obj, *this); };
    virtual ~AlibabaCloudException() = default ;
    AlibabaCloudException& operator=(const AlibabaCloudException &) = default ;
    AlibabaCloudException& operator=(AlibabaCloudException &&) = default ;
    
    // Use parent class getters for inherited fields
    inline int64_t statusCode() const { return getStatusCode(); };
    inline string code() const { return getCode(); };
    inline string message() const { return getMessage(); };
    inline string description() const { return getDescription(); };
    inline string requestId() const { return requestId_; };
    
  protected:
    // Only requestId is specific to AlibabaCloud (other fields come from parent)
    std::string requestId_;
  };
  
  } // namespace Exceptions
} // namespace AlibabaCloud
} // namespace OpenApi
#endif
