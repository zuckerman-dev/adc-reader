from conans import ConanFile, CMake, tools

class AdcReaderConan(ConanFile):
   name = "adc-reader"
   version = "0.0.1"
   settings = "os", "compiler", "build_type", "arch"
   requires = "date/3.0.0",  \
            "spdlog/[>=1.4.1]",  \
            "concurrentqueue/1.0.2",  \
            "CLI11/1.9.1@cliutils/stable",  \
            "WiringPi/2.60.0@zdev/testing",  \
            "bcm2835/1.68@zdev/testing"
            
   generators = "cmake", "cmake_find_package"
   default_options = {}

   scm = {
      "type" : "git",
      "subfolder" : ".",
      "url" : "https://github.com/zuckerman-dev/adc-read.git",
      "revision" : "auto", 
      "submodule" : "recursive"
   }
   no_copy_source = True
   keep_imports = True
      
   def build(self):
      cmake = CMake(self)
      cmake.configure()
      cmake.build()   
      cmake.install()

   def package_info(self):
      # Ordered list of include paths
      self.cpp_info.includedirs = ['include/adc/', \
                                 'include/adc/ads1256', \
                                 'include/adc/ads1263' \
                                 ]  
      self.cpp_info.libs = tools.collect_libs(self)
      
   def imports(self):
      self.copy("*.dll", dst="bin", src="bin") # From bin to bin
      self.copy("*.dylib*", dst="bin", src="lib") # From lib to bin
      