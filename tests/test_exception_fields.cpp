#include <gtest/gtest.h>
#include <alibabacloud/OpenapiException.hpp>
#include <darabonba/Exception.hpp>

using namespace AlibabaCloud::OpenApi::Exceptions;

// Test that exception fields are properly preserved when constructed from JSON
TEST(ExceptionFieldsTest, ClientExceptionFieldsFromJSON) {
  json errorJson = {
    {"statusCode", 403},
    {"code", "Forbidden"},
    {"message", "code: 403, Access denied request id: req-12345"},
    {"description", "You do not have permission"},
    {"requestId", "req-12345"},
    {"data", {
      {"Code", "Forbidden"},
      {"Message", "Access denied"},
      {"RequestId", "req-12345"}
    }},
    {"accessDeniedDetail", {
      {"AuthAction", "ecs:RunInstances"},
      {"AuthPrincipalType", "SubUser"}
    }}
  };

  ClientException ex(errorJson);

  // Verify all fields are correctly set
  EXPECT_EQ(ex.statusCode(), 403);
  EXPECT_EQ(ex.code(), "Forbidden");
  EXPECT_EQ(ex.message(), "code: 403, Access denied request id: req-12345");
  EXPECT_EQ(ex.description(), "You do not have permission");
  EXPECT_EQ(ex.requestId(), "req-12345");
  
  // Verify data field
  EXPECT_FALSE(ex.getData().is_null());
  EXPECT_EQ(ex.getData()["Code"], "Forbidden");
  
  // Verify accessDeniedDetail
  EXPECT_FALSE(ex.accessDeniedDetail().is_null());
  EXPECT_EQ(ex.accessDeniedDetail()["AuthAction"], "ecs:RunInstances");
}

// Test ServerException fields
TEST(ExceptionFieldsTest, ServerExceptionFieldsFromJSON) {
  json errorJson = {
    {"statusCode", 500},
    {"code", "InternalError"},
    {"message", "code: 500, Internal server error request id: req-67890"},
    {"description", "Server encountered an error"},
    {"requestId", "req-67890"},
    {"data", {
      {"Code", "InternalError"},
      {"Message", "Internal server error"}
    }}
  };

  ServerException ex(errorJson);

  EXPECT_EQ(ex.statusCode(), 500);
  EXPECT_EQ(ex.code(), "InternalError");
  EXPECT_EQ(ex.message(), "code: 500, Internal server error request id: req-67890");
  EXPECT_EQ(ex.description(), "Server encountered an error");
  EXPECT_EQ(ex.requestId(), "req-67890");
  
  EXPECT_FALSE(ex.getData().is_null());
  EXPECT_EQ(ex.getData()["Code"], "InternalError");
}

// Test ThrottlingException with retryAfter field
TEST(ExceptionFieldsTest, ThrottlingExceptionFieldsFromJSON) {
  json errorJson = {
    {"statusCode", 429},
    {"code", "Throttling.User"},
    {"message", "code: 429, Request throttled request id: req-111"},
    {"description", "Too many requests"},
    {"requestId", "req-111"},
    {"retryAfter", 5000},
    {"data", {
      {"Code", "Throttling.User"},
      {"Message", "Request throttled"}
    }}
  };

  ThrottlingException ex(errorJson);

  EXPECT_EQ(ex.statusCode(), 429);
  EXPECT_EQ(ex.code(), "Throttling.User");
  EXPECT_EQ(ex.message(), "code: 429, Request throttled request id: req-111");
  EXPECT_EQ(ex.requestId(), "req-111");
  EXPECT_EQ(ex.retryAfter(), 5000);
  
  EXPECT_FALSE(ex.getData().is_null());
}

// Test exception catching and re-throwing preserves fields
TEST(ExceptionFieldsTest, ExceptionCatchAndRethrowPreservesFields) {
  json errorJson = {
    {"statusCode", 403},
    {"code", "Forbidden"},
    {"message", "Access denied"},
    {"requestId", "req-xyz"}
  };

  try {
    throw ClientException(errorJson);
  } catch (const Darabonba::DaraException& ex) {
    // When caught as base class, fields should still be accessible
    EXPECT_EQ(ex.getStatusCode(), 403);
    EXPECT_EQ(ex.getCode(), "Forbidden");
    EXPECT_EQ(ex.getMessage(), "Access denied");
  }
}

// Test polymorphic exception handling
TEST(ExceptionFieldsTest, PolymorphicExceptionHandling) {
  auto createException = []() -> std::shared_ptr<AlibabaCloudException> {
    json errorJson = {
      {"statusCode", 404},
      {"code", "NotFound"},
      {"message", "Resource not found"},
      {"requestId", "req-404"}
    };
    return std::make_shared<ClientException>(errorJson);
  };

  auto ex = createException();
  
  // Fields should be accessible through base class pointer
  EXPECT_EQ(ex->statusCode(), 404);
  EXPECT_EQ(ex->code(), "NotFound");
  EXPECT_EQ(ex->message(), "Resource not found");
  EXPECT_EQ(ex->requestId(), "req-404");
}

// Test that empty/null fields don't cause crashes
TEST(ExceptionFieldsTest, EmptyFieldsHandling) {
  json errorJson = {
    {"statusCode", 400},
    {"code", "BadRequest"}
    // message, description, requestId are missing
  };

  ClientException ex(errorJson);

  EXPECT_EQ(ex.statusCode(), 400);
  EXPECT_EQ(ex.code(), "BadRequest");
  EXPECT_EQ(ex.message(), "");  // Should default to empty string
  EXPECT_EQ(ex.requestId(), "");
  
  // Test serialization of empty fields
  json serialized;
  to_json(serialized, ex);
  
  // Even empty strings should be serialized
  EXPECT_TRUE(serialized.contains("statusCode"));
  EXPECT_EQ(serialized["statusCode"], 400);
  EXPECT_TRUE(serialized.contains("requestId"));
  EXPECT_EQ(serialized["requestId"], "");  // Empty string is valid
}

// Test zero statusCode handling
TEST(ExceptionFieldsTest, ZeroStatusCodeHandling) {
  json errorJson = {
    {"statusCode", 0},  // Explicitly set to 0
    {"code", "Unknown"},
    {"message", "Unknown error"}
  };

  ClientException ex(errorJson);

  EXPECT_EQ(ex.statusCode(), 0);
  
  // Serialize and check
  json serialized;
  to_json(serialized, ex);
  
  EXPECT_TRUE(serialized.contains("statusCode"));
  EXPECT_EQ(serialized["statusCode"], 0);  // 0 should be preserved
}

// Test exception from POP.cpp scenario
TEST(ExceptionFieldsTest, POPExceptionScenario) {
  // Simulate the exception thrown in POP.cpp line 148-155
  json err = {
    {"Code", "InvalidParameter"},
    {"Message", "Parameter validation failed"},
    {"RequestId", "req-pop-123"},
    {"Description", "Invalid region ID"}
  };

  json exceptionJson = {
    {"statusCode", 400},
    {"code", "InvalidParameter"},
    {"message", "code: 400, Parameter validation failed request id: req-pop-123"},
    {"description", "Invalid region ID"},
    {"data", err},
    {"requestId", "req-pop-123"}
  };

  ClientException ex(exceptionJson);

  // Verify all fields are properly set
  EXPECT_EQ(ex.statusCode(), 400);
  EXPECT_EQ(ex.code(), "InvalidParameter");
  EXPECT_NE(ex.message(), "");
  EXPECT_EQ(ex.requestId(), "req-pop-123");
  EXPECT_FALSE(ex.getData().is_null());
  EXPECT_EQ(ex.getData()["Code"], "InvalidParameter");
}

// Test the critical scenario from Client.cpp line 1088: {"exception", ex}
TEST(ExceptionFieldsTest, ExceptionSerializationToJSON) {
  // Create an exception with all fields
  json exceptionJson = {
    {"statusCode", 403},
    {"code", "Forbidden"},
    {"message", "code: 403, Access denied request id: req-456"},
    {"description", "Permission denied"},
    {"requestId", "req-456"},
    {"data", {
      {"Code", "Forbidden"},
      {"Message", "Access denied"}
    }},
    {"accessDeniedDetail", {
      {"AuthAction", "ecs:RunInstances"}
    }}
  };

  ClientException ex(exceptionJson);

  // Serialize exception to JSON (like in Client.cpp line 1088)
  json serialized;
  to_json(serialized, ex);

  // Verify critical fields are preserved during serialization
  EXPECT_EQ(serialized["code"], "Forbidden");
  EXPECT_EQ(serialized["message"], "code: 403, Access denied request id: req-456");
  EXPECT_FALSE(serialized["data"].is_null());
  
  // CRITICAL: statusCode must be preserved!
  EXPECT_TRUE(serialized.contains("statusCode")) << "statusCode field is missing in serialized JSON!";
  if (serialized.contains("statusCode")) {
    EXPECT_EQ(serialized["statusCode"], 403) << "statusCode value is incorrect!";
  }
  
  // requestId should also be preserved
  EXPECT_TRUE(serialized.contains("requestId")) << "requestId field is missing!";
  if (serialized.contains("requestId")) {
    EXPECT_EQ(serialized["requestId"], "req-456");
  }
}

// Test RetryPolicyContext scenario (Client.cpp 1084-1089)
TEST(ExceptionFieldsTest, RetryPolicyContextSerialization) {
  // Simulate the retry context creation
  json exceptionJson = {
    {"statusCode", 500},
    {"code", "InternalError"},
    {"message", "Server error"},
    {"requestId", "req-retry-123"}
  };

  ServerException ex(exceptionJson);

  // Simulate what happens in Client.cpp line 1084-1089
  json contextJson = {
    {"retriesAttempted", 2},
    {"lastRequest", nullptr},
    {"lastResponse", nullptr},
    {"exception", ex}  // This is the critical line
  };

  // Verify the exception was serialized correctly
  EXPECT_TRUE(contextJson["exception"].contains("code"));
  EXPECT_EQ(contextJson["exception"]["code"], "InternalError");
  
  // Critical check: statusCode must be in the serialized exception
  EXPECT_TRUE(contextJson["exception"].contains("statusCode")) 
    << "statusCode missing when exception serialized to RetryPolicyContext!";
  
  if (contextJson["exception"].contains("statusCode")) {
    EXPECT_EQ(contextJson["exception"]["statusCode"], 500);
  }
}

// Test complete round-trip: Exception → JSON → RetryPolicyContext → Exception
TEST(ExceptionFieldsTest, RetryPolicyContextRoundTrip) {
  // Step 1: Create original exception with all fields
  json originalJson = {
    {"statusCode", 403},
    {"code", "Forbidden"},
    {"message", "Access denied"},
    {"description", "No permission"},
    {"requestId", "req-round-trip-456"},
    {"data", {{"Code", "Forbidden"}, {"Message", "Access denied"}}},
    {"accessDeniedDetail", {{"AuthAction", "ecs:RunInstances"}}}
  };
  
  ClientException originalEx(originalJson);
  
  // Step 2: Serialize to RetryPolicyContext (like Client.cpp 1084-1089)
  json contextJson = {
    {"retriesAttempted", 3},
    {"lastRequest", nullptr},
    {"lastResponse", nullptr},
    {"exception", originalEx}
  };
  
  // Step 3: Create RetryPolicyContext from JSON
  Darabonba::Policy::RetryPolicyContext context(contextJson);
  
  // Step 4: Extract exception from context
  auto extractedException = context.getException();
  
  // Step 5: Verify all fields are preserved
  ASSERT_NE(extractedException, nullptr) << "Exception is null after round-trip!";
  
  EXPECT_EQ(extractedException->getStatusCode(), 403) << "statusCode lost after round-trip!";
  EXPECT_EQ(extractedException->getCode(), "Forbidden") << "code lost after round-trip!";
  EXPECT_EQ(extractedException->getMessage(), "Access denied") << "message lost after round-trip!";
  
  // Try to cast to ClientException to verify specific fields
  auto clientEx = std::dynamic_pointer_cast<ClientException>(extractedException);
  if (clientEx) {
    EXPECT_FALSE(clientEx->accessDeniedDetail().is_null()) 
      << "accessDeniedDetail lost after round-trip!";
    EXPECT_EQ(clientEx->accessDeniedDetail()["AuthAction"], "ecs:RunInstances");
  } else {
    // If we can't cast, at least verify the base exception has the data
    EXPECT_FALSE(extractedException->getData().is_null());
  }
}

// Test the exact scenario from UnretryableException
TEST(ExceptionFieldsTest, UnretryableExceptionScenario) {
  // Simulate multiple retry attempts with different exceptions
  json firstAttemptEx = {
    {"statusCode", 500},
    {"code", "InternalError"},
    {"message", "First attempt failed"},
    {"requestId", "req-001"}
  };
  
  json secondAttemptEx = {
    {"statusCode", 503},
    {"code", "ServiceUnavailable"},
    {"message", "Second attempt failed"},
    {"requestId", "req-002"}
  };
  
  ServerException ex1(firstAttemptEx);
  ServerException ex2(secondAttemptEx);
  
  // Create retry context with second exception
  json contextJson = {
    {"retriesAttempted", 2},
    {"lastRequest", nullptr},
    {"lastResponse", nullptr},
    {"exception", ex2}  // Last exception
  };
  
  Darabonba::Policy::RetryPolicyContext context(contextJson);
  
  // This would be thrown as UnretryableException (Client.cpp 1094)
  // Verify the last exception's fields are intact
  auto lastEx = context.getException();
  ASSERT_NE(lastEx, nullptr);
  
  EXPECT_EQ(lastEx->getStatusCode(), 503);
  EXPECT_EQ(lastEx->getCode(), "ServiceUnavailable");
  EXPECT_EQ(lastEx->getMessage(), "Second attempt failed");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
