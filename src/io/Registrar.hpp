#pragma once

#include "AEventHandler.hpp"

typedef void* Registrar;
typedef void (*RegisterCallback)(io::AEventHandler* handler, Registrar registrar);

struct RegisterContext {
    void* registrar;
    RegisterCallback callback;
};
