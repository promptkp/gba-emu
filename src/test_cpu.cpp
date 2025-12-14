#include <iostream>
#include <cpu.h>

int main(int argc, char *argv[]) {
  std::cerr << "CPU test!" << std::endl;

  if (argc != 2) {
    std::cerr << "argc != 2 (expecting file name as first argument)" << std::endl;
    exit(-1);
  }

  char *test_file{argv[1]};

  CPU cpu{};

  cpu.RunTest(test_file);
  cpu.PrintRegister();
}
