#ifndef TYPE_ERASURE_DEF
#define TYPE_ERASURE_DEF
#include <memory>
#include <mutex>

/* template <typename R, typename... Args> */
/* class FunctorBridge { */
/* public: */
/*     virtual ~FunctorBridge() = default; */
/*     virtual std::shared_ptr<FunctorBridge> clone() const = 0; */
/*     virtual R invoke(Args... args) const = 0; */
/* }; */

/* template <typename Signature> */
/* class FunctionPtr; */

/* template <typename R, typename... Args> */
/* class FunctionPtr<R(Args...)> { */
/* private: */
/*     std::shared_ptr<FunctorBridge<R, Args...>> bridge = nullptr; */
/* public: */
/*     FunctionPtr() = default; */
/*     FunctionPtr(FunctionPtr const& other) { */
/*         if (other.bridge) { */
/*             bridge = other.bridge->clone(); */
/*         } */
/*     } */
/*     FunctionPtr(FunctionPtr& other) : FunctionPtr(static_cast<FunctionPtr const&>(other)) { } */
/*     FunctionPtr(FunctionPtr&& other): bridge(other.bridge) { */
/*         other.bridge = nullptr; */
/*     } */
/*     template<typename F> FunctionPtr(F&& f); */
/*     FunctionPtr& operator=(FunctionPtr const& other) { */
/*         FunctionPtr tmp(other); */
/*         swap(*this, tmp); */
/*         return *this; */
/*     } */
/*     FunctionPtr& operator=(FunctionPtr&& other) { */
/*         /1* delete bridge; *1/ */
/*         bridge = other.bridge; */
/*         other.bridge = nullptr; */
/*         return *this; */
/*     } */

/*     template<typename F> FunctionPtr& operator=(F&& f) { */
/*         FunctionPtr tmp(std::forward<F>(f)); */
/*         swap(*this, tmp); */
/*         return *this; */
/*     } */

/*     R operator()(Args... args) const { */
/*         return bridge->invoke(std::forward<Args>(args)...); */
/*     } */

/*     ~FunctionPtr() { */
/*         /1* delete bridge; *1/ */
/*     } */

/*     friend void swap(FunctionPtr& fp1, FunctionPtr& fp2) { */
/*         std::swap(fp1.bridge, fp2.bridge); */
/*     } */
/*     explicit operator bool() const { */
/*         return bridge == nullptr; */
/*     } */
/* }; */

/* template <typename Functor, typename R, typename... Args> */
/* class SpecificFunctorBridge : public FunctorBridge<R, Args...> { */
/*     Functor functor; */
/* public: */
/*     template <typename FunctorFwd> */
/*     SpecificFunctorBridge(FunctorFwd&& functor) */
/*         : functor(std::forward<FunctorFwd>(functor)) { } */

/*     virtual std::shared_ptr<FunctorBridge<R, Args...>> clone() const override { */
/*         return std::make_shared<FunctorBridge<R, Args...>>(functor); */
/*     } */

/*     virtual R invoke(Args... args) const override { */
/*         return functor(std::forward<Args>(args)...); */
/*     } */
/* }; */

/* template <typename R, typename... Args> */
/* template <typename F> */
/* FunctionPtr<R(Args...)>::FunctionPtr(F&& f) */
/* { */
/*     using Functor = std::decay_t<F>; */
/*     using Bridge = SpecificFunctorBridge<Functor, R, Args...>; */
/*     bridge = std::make_shared<FunctorBridge<R, Args...>>(std::forward<F>(f)); */
/* } */

/* template <typename T> */
/* class lazy_val { */
/* private: */
/*     FunctionPtr<T()> m_computation; */
/*     mutable T m_cache; */
/*     mutable std::once_flag m_value_flag; */
/* public: */
/*     lazy_val(FunctionPtr<T()> f) : m_computation(f) { } */
/*     const T& operator()() const { */
/*         std::call_once(m_value_flag, [this] { */
/*                 m_cache = m_computation(); */
/*         }); */
/*         return m_cache; */
/*     } */
/* }; */


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
  };    // see functionptr-cpinv.hpp
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
  }  // see functionptr-init.hpp

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
