//SYCL programs must include the <CL/sycl.hpp> header file
//to provide all of the SYCL features
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
  // All SYCL names are defined in the cl::sycl namespace
  cl::sycl::default_selector device_selector;

  // A queue connects a host program to a single device
  cl::sycl::queue queue(device_selector);

  // Allocating data to be worked on which is going to
  // be accessible on both CPU (host) and an accelerator (device)
  const size_t size = read_from_command_line_args(argc, argv);
  auto *data = cl::sycl::malloc_shared<float>(size, queue);

  // Submit a command group function object to the queue
  // for asynchronous execution
  queue.submit([&] (cl::sycl::handler& cgh) {
    // The function object accepts a command
    // group handler constructed by the SYCL runtime

    // A command group is a way to encapsulate a device-side operation
    // and all its data dependencies in a single object by grouping
    // all the related commands

    // Invokes a kernel function for a SYCL range
    cgh.parallel_for(cl::sycl::range<1>(size),
                     [=](cl::sycl::id<1> i) {
      // `id` is an abstraction that describes the location
      // of a point in a sycl::range
        
      // The body of each loop instance 
      data[i] = cl::sycl::pown(static_cast<float>(i), 2);
    });
  });

  // waits for all queue operations to complete
  queue.wait();

  // print results
  for (size_t i = 0; i < size; ++i) {
    std::cout << i << '\t' << data[i] << std::endl;
  }

  // delete allocated memory
  free(data, queue);

  // print device info
  const auto device = queue.get_device();
  std::cout << "Computations were performed on: " 
            << device.get_info<cl::sycl::info::device::name>()
            << std::endl;

  // exit program
  return 0;
}
