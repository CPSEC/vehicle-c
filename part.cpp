#include "part.h"

Part::Part(bool is_thread) {
    //
    name_ = "";
    is_thread_ = is_thread;
    thread_ = nullptr;
}

Part::~Part() {
    //
}
