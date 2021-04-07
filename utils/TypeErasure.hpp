#ifndef TYPE_ERASURE_DEF
#define TYPE_ERASURE_DEF
#include <memory>
#include <functional>

template <typename F>
class lazy_val {
private:
    F m_computation;
    mutable std::optional<decltype(m_computation())> m_value;
public:
    lazy_val(const F& f) : m_computation(f) { }

    const decltype(m_computation())& operator()() const {
        if (!m_value) {
            m_value = std::invoke(m_computation);
        }

        return *m_value;
    }
};

template <typename F>
inline lazy_val<F> make_lazy_val(F&& computation) {
    return lazy_val<F>(std::forward<F>(computation));
}

template<typename R, typename... Args>
class FunctorBridge
{
  public:
    virtual ~FunctorBridge() {
    }
    virtual FunctorBridge* clone() const = 0;
    virtual R invoke(Args... args) const = 0;
};

template<typename Functor, typename R, typename... Args>
class SpecificFunctorBridge : public FunctorBridge<R, Args...> {
private:
  Functor functor;

public:
  template<typename FunctorFwd>
  SpecificFunctorBridge(FunctorFwd&& functor)
    : functor(std::forward<FunctorFwd>(functor)) {
  }
  virtual SpecificFunctorBridge* clone() const override {
    return new SpecificFunctorBridge(functor);
  }
  virtual R invoke(Args... args) const override {
    return functor(std::forward<Args>(args)...);
  }
};

// primary template:
template<typename Signature> 
class FunctionPtr;

// partial specialization:
template<typename R, typename... Args> 
class FunctionPtr<R(Args...)>
{
 private:
  FunctorBridge<R, Args...>* bridge;
 public:
  // constructors:
  FunctionPtr() : bridge(nullptr) {
  }
  FunctionPtr(FunctionPtr const& other) : bridge(nullptr) {
      if (other.bridge) {
          bridge = other.bridge->clone();
      }
  };
  FunctionPtr(FunctionPtr& other) 
    : FunctionPtr(static_cast<FunctionPtr const&>(other)) { 
  }
  FunctionPtr(FunctionPtr&& other) : bridge(other.bridge) {
    other.bridge = nullptr;
  }
  // construction from arbitrary function objects:
  template<typename F> FunctionPtr(F&& f) : bridge(nullptr) {
      using Functor = std::decay_t<F>;
      using Bridge = SpecificFunctorBridge<Functor, R, Args...>;
      bridge = new Bridge(std::forward<F>(f));
  }

  // assignment operators:
  FunctionPtr& operator=(FunctionPtr const& other) {
    FunctionPtr tmp(other);
    swap(*this, tmp);
    return *this;
  }
  FunctionPtr& operator=(FunctionPtr&& other) {
    delete bridge;
    bridge = other.bridge;
    other.bridge = nullptr;
    return *this;
  }
  // construction and assignment from arbitrary function objects:
  template<typename F> FunctionPtr& operator=(F&& f) {
    FunctionPtr tmp(std::forward<F>(f));
    swap(*this, tmp);
    return *this;
  }

  // destructor:
  ~FunctionPtr() { 
    delete bridge; 
  }

  friend void swap(FunctionPtr& fp1, FunctionPtr& fp2) {
    std::swap(fp1.bridge, fp2.bridge);
  }
  explicit operator bool() const {
    return bridge == nullptr;
  }

  // invocation:
  R operator()(Args... args) const {
      return bridge->invoke(std::forward<Args>(args)...);
  };
};


#endif
