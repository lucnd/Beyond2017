#ifndef TEST_UTILS_MAKE_SP_H_INCLUDED
#define TEST_UTILS_MAKE_SP_H_INCLUDED

#include <utils/StrongPointer.h>
#include <utility>

template<class RefBase, typename... Args>
android::sp<RefBase> makeSp(Args&&... args) {
    return android::sp<RefBase>(new RefBase(std::forward<Args>(args)...));
}

#endif  // TEST_UTILS_MAKE_SP_H_INCLUDED
