#include <dlfcn.h>
#include <iostream>

#include "test_common.h"

int main(int argc, const char** argv) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " TEST_NAME" << std::endl;
		return -1;
	}

	const char* test_name = argv[1];

	void* libhandle = dlopen(NULL, RTLD_LAZY);

	if (libhandle == nullptr) {
		std::cerr << "Failed to dlopen handle: " << dlerror() << std::endl;
		return -2;
	}

	test_entrypoint entrypoint = reinterpret_cast<test_entrypoint>(dlsym(libhandle, test_name));

	if (entrypoint == nullptr) {
		std::cerr << "Could not load symbol " << test_name << ": " << dlerror() << std::endl;
		return -3;
	}

	int test_argc = argc - 1;
	const char** test_argv = argv + 1;

	return entrypoint(test_argc, test_argv);
}

