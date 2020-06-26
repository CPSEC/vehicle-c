#include "part.h"

Part::Part(PartType parttype) {
    part_type_ = parttype;
    state_ = State::ShareMemoryInit();
}

void Part::Run(){};

Part::~Part() {}