#include "hello.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>

int main()
{
    std::string server_address("127.0.0.1:50051");

    // Create a channel to the server
    auto channel =
        grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    std::unique_ptr<GreeterService::Stub> stub =
        GreeterService::NewStub(channel);

    // Prepare the request
    HelloRequest request;
    request.set_name("world");

    // Call the service
    HelloResponse reply;
    grpc::ClientContext context;
    grpc::Status status = stub->SayHello(&context, request, &reply);

    // Check the response
    if (status.ok()) {
        std::cout << "Greeter received: " << reply.message() << std::endl;
    }
    else {
        std::cout << "RPC failed" << std::endl;
    }

    return 0;
}
