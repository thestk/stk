 # To lint the spec:
 # pod spec lint --skip-import-validation --allow-warnings
 # To publish:
 # pod trunk push STK.podspec --skip-import-validation --allow-warnings

Pod::Spec.new do |spec|
  spec.name         = 'STK'
  spec.version      = '4.6.2'
  spec.summary      = 'The Synthesis ToolKit in C++ is a set of open source audio signal processing and algorithmic synthesis classes.'
  spec.homepage     = 'https://ccrma.stanford.edu/software/stk/'
  spec.source       = { :git => 'https://github.com/thestk/stk.git', :tag => spec.version }
  spec.license      = { :type => 'MIT' }
  spec.author       = { "Ariel Elkin" => "ariel@arivibes.com" }
  spec.platform     = :ios
  spec.ios.deployment_target = "10.0"
  spec.source_files = [
    "src/*.cpp",
    "include/*.h"
  ]
  spec.public_header_files = [
    "include/*.h",
    "include/SKINImsg.h",
    "include/SKINItbl.h"
  ]
  spec.exclude_files = [
    "include/Thread.h",
    "src/Thread.cpp",
    "include/Mutex.h",
    "src/Mutex.cpp",
    "include/UdpSocket.h",
    "src/UdpSocket.cpp",
    "include/Socket.h",
    "src/Socket.cpp",
    "include/TcpClient.h",
    "src/TcpClient.cpp",
    "include/TcpServer.h",
    "src/TcpServer.cpp",
    "include/InetWvIn.h",
    "src/InetWvIn.cpp",
    "include/InetWvOut.h",
    "src/InetWvOut.cpp",
    "include/RtAudio.h",
    "src/RtAudio.cpp",
    "include/RtMidi.h",
    "src/RtMidi.cpp",
    "include/RtWvIn.h",
    "src/RtWvIn.cpp",
    "include/RtWvOut.h",
    "src/RtWvOut.cpp",
    "include/RtError.h"
  ]
  spec.preserve_paths = "README.MD"
  spec.resource_bundles = { "rawwaves": "rawwaves/*.raw" }
  spec.libraries = 'c++'
end
