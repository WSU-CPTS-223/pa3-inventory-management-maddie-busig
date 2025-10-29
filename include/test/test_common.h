#pragma once

typedef int (*test_entrypoint)(int argc, const char** argv);

#ifdef __cplusplus
#define TEST_ENTRYPOINT extern "C"
#else
#define TEST_ENTRYPOINT
#endif

