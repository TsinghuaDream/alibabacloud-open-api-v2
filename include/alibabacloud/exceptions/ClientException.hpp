// This file is auto-generated, don't edit it. Thanks.
#ifndef ALIBABACLOUD_EXCEPTIONS_CLIENTEXCEPTION_HPP_
#define ALIBABACLOUD_EXCEPTIONS_CLIENTEXCEPTION_HPP_
#include <darabonba/Core.hpp>
using namespace std;
using json = nlohmann::json;
namespace AlibabaCloud
{
namespace OpenApi
{
namespace Exceptions
{
  class ClientException : public AlibabaCloudException {
  public:
    friend void from_json(const Darabonba::Json& j, ClientException& obj) { 
      DARABONBA_ANY_FROM_JSON(accessDeniedDetail, accessDeniedDetail_);
    };
    
    // Override to_json to include accessDeniedDetail
    friend void to_json(Darabonba::Json& j, const ClientException& obj) {
      // Call parent to_json
      to_json(j, static_cast<const AlibabaCloudException&>(obj));
      // Override accessDeniedDetail if present
      if (!obj.accessDeniedDetail_.is_null()) {
        j["accessDeniedDetail"] = obj.accessDeniedDetail_;
      }
    };
    
    ClientException() ;
    ClientException(const ClientException &) = default ;
    ClientException(ClientException &&) = default ;
    ClientException(const Darabonba::Json & obj) : AlibabaCloudException(obj) { from_json(obj, *this); };
    virtual ~ClientException() = default ;
    ClientException& operator=(const ClientException &) = default ;
    ClientException& operator=(ClientException &&) = default ;
    inline     const Darabonba::Json & accessDeniedDetail() const { DARABONBA_GET(accessDeniedDetail_) };
    inline Darabonba::Json & accessDeniedDetail() { DARABONBA_GET(accessDeniedDetail_) };
  protected:
    // Access Denied Detail
    Darabonba::Json accessDeniedDetail_ {};
  };
  
  } // namespace Exceptions
} // namespace AlibabaCloud
} // namespace OpenApi
#endif
