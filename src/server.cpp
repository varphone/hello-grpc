#include "hello.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <memory>

class GreeterServiceImpl final : public GreeterService::Service {
    grpc::Status SayHello(grpc::ServerContext* context,
                          const HelloRequest* request,
                          HelloResponse* reply) override
    {
        std::string prefix("Hello ");
        reply->set_message(prefix + request->name());
        return grpc::Status::OK;
    }
};

int main()
{
    std::string server_address("0.0.0.0:50051");
    GreeterServiceImpl service;

    grpc::ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
    return 0;
}
