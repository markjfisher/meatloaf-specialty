#ifdef BUILD_IEC

#include "iecProtocolBase.h"

#include "bus.h"

#include "../../../include/pinmap.h"
#include "../../../include/debug.h"

using namespace Protocol;

int16_t IecProtocolBase::timeoutWait(uint8_t pin, bool target_status, size_t wait_us, bool watch_atn)
{
    uint64_t start = 0;
    uint64_t current = 0;
    uint64_t elapsed = 0;

    // Delay for line to be pulled up
    if ( target_status == RELEASED )
        wait(4);

    esp_timer_init();
    start = current = esp_timer_get_time();

    // Sample ATN and set flag to indicate COMMAND or DATA mode
    bool atn_status = IEC.status ( PIN_IEC_ATN );
    if ( pin == PIN_IEC_ATN )
    {
        watch_atn = false;
    }

    //IEC.pull ( PIN_IEC_SRQ );
    while ( IEC.status ( pin ) != target_status )
    {
        //fnSystem.delay_microseconds(1);
        current = esp_timer_get_time();
        elapsed = ( current - start );

        if ( elapsed >= wait_us && wait_us != FOREVER )
        {
            //IEC.release ( PIN_IEC_SRQ );
            if ( wait_us == TIMEOUT_DEFAULT )
                return -1;
            
            return wait_us;
        }

        if ( watch_atn )
        {
            bool atn_check = IEC.status ( PIN_IEC_ATN );
            if ( atn_check != atn_status )
            {
                //IEC.release ( PIN_IEC_SRQ );
                //Debug_printv("pin[%d] state[%d] wait[%d] elapsed[%d]", pin, target_status, wait, elapsed);
                return -1;
            }            
        }
    }
    //IEC.release ( PIN_IEC_SRQ );

    // Debug_printv("pin[%d] state[%d] wait[%d] step[%d] t[%d]", pin, target_status, wait, elapsed);
    return elapsed;

}

bool IecProtocolBase::wait(size_t wait_us, bool watch_atn)
{
    return wait(wait_us, 0, watch_atn);
}

bool IecProtocolBase::wait(size_t wait_us, uint64_t start, bool watch_atn)
{
    uint64_t current, elapsed;
    current = 0;
    elapsed = 0;

    if ( wait_us == 0 ) return true;
//    wait_us--; // Shave 1us for overhead

    esp_timer_init();
    if ( start == 0 )
        start = current = esp_timer_get_time();
    else
        current = esp_timer_get_time();

    // Sample ATN and set flag to indicate SELECT or DATA mode
    bool atn_status = IEC.status ( PIN_IEC_ATN );

    //IEC.pull ( PIN_IEC_SRQ );
    while ( elapsed < wait_us )
    {
        //fnSystem.delay_microseconds(1);
        current = esp_timer_get_time();
        elapsed = current - start;

        if ( watch_atn )
        {
            bool atn_check = IEC.status ( PIN_IEC_ATN );
            if ( atn_check != atn_status )
            {
                //IEC.release ( PIN_IEC_SRQ );
                //Debug_printv("wait[%d] elapsed[%d] start[%d] current[%d]", wait, elapsed, start, current);
                return false;
            }
        }
    }

    //IEC.release ( PIN_IEC_SRQ );
    return true;
}

#endif /* BUILD_IEC */