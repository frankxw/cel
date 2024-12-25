#include <stdlib.h>
#include "Idler.h"
#include "Logging.h"

using namespace cel;

void idlerHook(uv_idle_t* handle);

Idler::~Idler()
{
    m_uvIdler.data = nullptr;
}

void Idler::Start(uv_loop_t* loop)
{
    uv_idle_init(loop, &m_uvIdler);
    m_uvIdler.data = this;
    uv_idle_start(&m_uvIdler, idlerHook);
}

void idlerHook(uv_idle_t* handle)
{
    Idler* idler = static_cast<Idler*>(handle->data);
    if(!idler) {
        LogErr(LogLevel::Normal, "idlerHook error: missing Idler context (handle->data)\n");
        return;
    }

    idler->Execute();
}
