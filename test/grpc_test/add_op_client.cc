#include <grpc++/grpc++.h>
#include <memory>

#include "grpcpp/channel.h"
#include "grpcpp/impl/codegen/client_context.h"
#include "test/grpc_test/add_op.grpc.pb.h"
#include "test/grpc_test/add_op.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using add_op::AddService;
using add_op::AddRequest;
using add_op::AddReply;

class AddOpClient {
 public:
  AddOpClient(std::shared_ptr<Channel> channel) 
    : _stub(AddService::NewStub(channel)) {}

  int Add(int a, int b) {
    AddRequest request;
    request.set_a(a);
    request.set_b(b);

    AddReply reply;
    ClientContext context;

    Status status = _stub->AddRPC(&context, request, &reply);

    if (status.ok()) {
      return reply.sum();
    } else {
      std::cout << "RPC failed" << std::endl;
      return 0;
    }
  }

 private:
  std::unique_ptr<AddService::Stub> _stub;
};


int main(int argc, char **argv) {
  AddOpClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  int a = 1;
  int b = 2;
  int sum = client.Add(a, b);
  std::cout << "Sum of " << a << " and " << b << " is " << sum << std::endl;
  return 0;
}
