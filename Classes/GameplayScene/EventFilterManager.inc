// eventFiltersWorkingRange 是受事件 filter 处理的自定义事件类型的集合
// 未加入 eventFiltersWorkingRange 的事件类型不受 filter 处理

#include <cstdlib>

static const char* eventFilterWorkingRange[] = {
#ifndef NDEBUG
    "test_event_1",
    "test_event_2",
#endif
    // 在这添加事件类型
    "bullet_hit_boss",
    "bullet_hit_player"
};
