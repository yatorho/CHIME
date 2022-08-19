#include <iostream>
#include <memory>

#include <grpc++/grpc++.h>
#include "grpcpp/impl/codegen/server_context.h"
#include "grpcpp/server_builder.h"
#include "test/grpc_test/add_op.grpc.pb.h"

using grpc::Server;
using grpc::Status;
using grpc::ServerContext;
using grpc::ServerBuilder;

using add_op::AddService;

class AddServiceImpl final : public  AddService::Service {
  Status AddRPC(ServerContext* context, const add_op::AddRequest* request,
             add_op::AddReply* reply) override {
    reply->set_sum(request->a() + request->b());
    std::cout << "Get request from " << context->peer() << std::endl;
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  AddServiceImpl service;
  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char **argv) {
  RunServer();
  return 0;
}


