#include <iostream>
#include <memory>
#include <string>

class Resource {
 public:
  virtual ~Resource() = default;

  [[nodiscard]] virtual std::string describe() const = 0;
};

class TextResource final : public Resource {
 public:
  [[nodiscard]] std::string describe() const override {
    return "TextResource";
  }
};

void demonstrate_shared_ownership() {
  auto first =
      std::make_shared<std::string>("shared resource");

  std::cout << "count after first: "
            << first.use_count() << '\n';

  {
    auto second = first;

    std::cout << "count after second: "
              << first.use_count() << '\n';

    *second = "modified through second";

    std::cout << "value through first: "
              << *first << '\n';
  }

  std::cout << "count after second is destroyed: "
            << first.use_count() << '\n';
}

void demonstrate_weak_observer() {
  std::weak_ptr<std::string> observer;

  {
    auto owner =
        std::make_shared<std::string>("observed resource");

    observer = owner;

    std::cout << std::boolalpha;
    std::cout << "strong count after weak observer: "
              << owner.use_count() << '\n';

    std::cout << "expired while owner exists: "
              << observer.expired() << '\n';

    if (auto locked = observer.lock()) {
      std::cout << "value through lock: "
                << *locked << '\n';
      std::cout << "strong count during lock: "
                << owner.use_count() << '\n';
    }

    std::cout << "strong count after lock scope: "
              << owner.use_count() << '\n';
  }

  std::cout << "expired after owner is destroyed: "
            << observer.expired() << '\n';

  const auto locked_after_expiration = observer.lock();
  std::cout << "lock succeeds after expiration: "
            << static_cast<bool>(locked_after_expiration)
            << '\n';
}

int main() {
  // std::unique_ptr<Resource> resource =
  //     std::make_unique<TextResource>();

  // std::cout << "resource: "
  //           << resource->describe() << '\n';

 // demonstrate_shared_ownership();
  demonstrate_weak_observer();
  return 0;
}

