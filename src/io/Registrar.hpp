#pragma once

#include "AEventHandler.hpp"

typedef void* Registrar;

typedef void (*RegisterCallback)(io::AEventHandler* handler, Registrar registrar);
typedef void (*DeleteCallback)(io::AEventHandler* handler, Registrar registrar);

struct RegisterContext {
    Registrar registrar;
    RegisterCallback callback;
    DeleteCallback delete_cb;
};
