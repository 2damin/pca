from conans import ConanFile, CMake, tools
import os

class DependencyConan(ConanFile):
  #  conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan 
  settings = "os", "compiler", "build_type", "arch"
  requires = "openssl/1.1.1g", "zlib/1.2.11", "protobuf/3.9.1@bincrafters/stable", "grpc/1.25.0@conan+common/stable", "spdlog/1.5.0", "boost/1.73.0", "tbb/2020.1", "eigen/3.3.7"
  generators = "cmake"
  default_options = "openssl:shared=False", "zlib:shared=False", "protobuf:shared=False", "grpc:build_codegen=False", "grpc:build_csharp_ext=False", "spdlog:shared=False", "tbb:shared=False", "protobuf:shared=False", "boost:shared=False"
