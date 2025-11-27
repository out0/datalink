#include <gtest/gtest.h>
#include <chrono>

static double __last_exec_mark = -1;

void exec_start() {
    __last_exec_mark = std::chrono::duration_cast<std::chrono::duration<double>>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
void exec_finished(const char *msg) {
    double p = __last_exec_mark;
    exec_start();
    printf ("[Test benchmark] %s execution took %.2f ms\n", msg, 1000*(__last_exec_mark - p));
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}