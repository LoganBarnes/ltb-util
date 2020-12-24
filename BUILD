cc_library(
    name = "ltb-util",
    srcs = glob(["src/ltb/util/*.cpp"]),
    hdrs = glob([
        "include/google/*.h",
        "include/ltb/util/*.hpp"
    ]),
    defines = ["-DDOCTEST_CONFIG_DISABLE"]
)

cc_test(
    name = "test-ltb-util",
    srcs = glob([
        "src/ltb/util/*",
        "include/google/*.h"
        ])
)
