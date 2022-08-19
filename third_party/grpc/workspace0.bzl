load("//third_party:repo.bzl", "ce_http_archive")

def repo():
    ce_http_archive(
        name = "com_github_grpc_grpc",
        urls = [
            "https://github.com/grpc/grpc/archive/de893acb6aef88484a427e64b96727e4926fdcfd.tar.gz",
        ],
        strip_prefix = "grpc-de893acb6aef88484a427e64b96727e4926fdcfd",
        sha256 = "61272ea6d541f60bdc3752ddef9fd4ca87ff5ab18dd21afc30270faad90c8a34",
    )
