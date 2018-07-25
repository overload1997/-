package(default_visibility = ["//visibility:public"])

cc_library(
    name = "libgen_roi",
    hdrs = ["gen_roi.h",
            ],
    deps = [
        "//s_loop/src/map/semantic_map:interface",
        "//s_loop/src/map/semantic_map/adapters:leveldb_adapter",
        "//tools/hd_map_dev/tilemap:map_reader",
            ],
)

cc_binary(
    name = "gen_roi",
    srcs = ["gen_roi.cc",
            ],
    deps = [":libgen_roi",
            "@com_github_gflags_gflags//:gflags",
            ],
)
#        "@pcl//:common",
#            "@pcl//:visualization",
#            "//s_loop/src/perception/common:pcl_util",
#            "//s_loop/src/perception/common/ficp:ficp",
#            "//s_loop/src/perception/detector/lane:lane_detector",
#            "@ros//:opencv3",
#            "//third_party/json:json",
#            "@glog//:glog",
#            "@eigen//:eigen",
#            "//experimental/zhiqian/zq_tools:zq_tools",
