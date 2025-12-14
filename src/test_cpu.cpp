#include <cstddef>
#include <iostream>
#include <cpu.h>
#include <vector>
#include <fstream>

int main(int argc, char *argv[]) {
  std::cerr << "CPU test!" << std::endl;

  if (argc != 2) {
    std::cerr << "argc != 2 (expecting file name as first argument)" << std::endl;
    return -1;
  }

  char *test_file{argv[1]};

  // std::ios::ate sets 'get pointer' (g) to eof
  std::ifstream test_stream{test_file, std::ios::binary | std::ios::ate};

  if (!test_stream.good()) {
    // check: Should I use endl with cerr?
    std::cerr << "Failed to open file" << std::endl;
  }

  // get position of g, i.e., size of file
  auto size{test_stream.tellg()};

  // move g back to (begin + 0), i.e., start of file
  test_stream.seekg(0, std::ios::beg);

  std::vector<std::byte> buffer(size);

  if (!test_stream.read(reinterpret_cast<char*>(buffer.data()), size)) {
    std::cerr << "Failed to read file" << std::endl;
  }

  CPU cpu{};

  cpu.RunTest(buffer);
  cpu.PrintRegister();
}
