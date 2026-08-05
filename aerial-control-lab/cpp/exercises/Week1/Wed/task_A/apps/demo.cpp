#include "week1_wed_task_a/lifecycle_raii.hpp"

#include <iostream>
#include <memory>
#include <string_view>
#include <utility>

namespace {

void print_stats(
    const std::string_view stage,
    const week1_wed_task_a::LifecycleStats& stats) {
    std::cout << stage << ": direct=" << stats.direct_constructions
              << ", copy=" << stats.copy_constructions
              << ", move=" << stats.move_constructions
              << ", destroyed=" << stats.destructions << '\n';
}

}  // namespace

int main() {
    using week1_wed_task_a::LifecycleProbe;
    using week1_wed_task_a::LifecycleStats;
    using week1_wed_task_a::ScopeTimer;

    LifecycleStats lifecycle_stats;
    {
        LifecycleProbe original(lifecycle_stats, 42);
        print_stats("after direct construction", lifecycle_stats);

        LifecycleProbe copied(original);
        print_stats("after copy construction", lifecycle_stats);

        LifecycleProbe moved(std::move(original));
        print_stats("after move construction", lifecycle_stats);

        std::cout << "copied value=" << copied.value()
                  << ", moved value=" << moved.value() << '\n';
    }
    print_stats("after leaving lifecycle scope", lifecycle_stats);

    LifecycleStats ownership_stats;
    auto owner = week1_wed_task_a::make_owned_probe(ownership_stats, 7);
    std::cout << "unique owner value=" << owner->value() << '\n';

    std::unique_ptr<LifecycleProbe> new_owner = std::move(owner);
    std::cout << "ownership transferred=" << std::boolalpha
              << (!owner && static_cast<bool>(new_owner)) << '\n';
    new_owner.reset();
    print_stats("after unique owner reset", ownership_stats);

    long long sum = 0;
    {
        ScopeTimer timer("accumulate integers", std::cout);
        for (int value = 0; value < 100000; ++value) {
            sum += value;
        }
    }
    std::cout << "sum=" << sum << '\n';

    return 0;
}
