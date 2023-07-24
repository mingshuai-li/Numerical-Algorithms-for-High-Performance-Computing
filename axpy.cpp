#include <CL/sycl.hpp>
#include <iostream>
#include <string>

size_t read_from_command_line_args(int argc,char* argv[]) {
  if (argc == 2) {
    try {
      return std::stoi(std::string(argv[1]));
    }
    catch (std::exception error) {
      std::cout << "cannot convert the 2nd command line arg to `int`" << std::endl;
    }
  }
  else {
    std::cout << "invalid num. command line arguments" << std::endl;
  }
  exit(-1);
}


int main(int argc, char* argv[]) {
  cl::sycl::default_selector device_selector;
  cl::sycl::queue queue(device_selector);

  const size_t size = read_from_command_line_args(argc, argv);
  auto *x = cl::sycl::malloc_shared<float>(size, queue);
  auto *y = cl::sycl::malloc_shared<float>(size, queue);
  auto *z = cl::sycl::malloc_shared<float>(size, queue);
  float alpha{1.5};
  float beta{4.3};

  for (size_t i = 0; i < size; ++i) {
      x[i] = 1.0;
      y[i] = -1.0;
  }

  queue.submit([&] (cl::sycl::handler& cgh) {
    cgh.parallel_for(cl::sycl::range<1>(size),
                     [=](cl::sycl::id<1> i) {
      z[i] = alpha * x[i] + beta * y[i];
    });
  });

  queue.wait();

  for (size_t i = 0; i < size; ++i) {
    std::cout << i << '\t' << z[i] << std::endl;
  }

  free(x, queue);
  free(y, queue);
  free(z, queue);

  const auto device = queue.get_device();
  std::cout << "Computations were performed on: " 
            << device.get_info<cl::sycl::info::device::name>()
            << std::endl;

  return 0;
}
