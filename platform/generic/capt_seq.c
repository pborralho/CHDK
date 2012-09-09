// generic capt_seq fuctions
#include "asmsafe.h"
#include "conf.h"
#include "stdlib.h"

#define RAWDATA_AVAILABLE (1)
#define RAWDATA_SAVED (2)

// some cams use different values
// note: many cams ROM also accepts a value of 3, meaning unknown
#ifndef NR_ON
#define NR_ON (2)
#endif
#ifndef NR_OFF
#define NR_OFF (1)
#endif

static long raw_save_stage;
volatile long shutter_open_time=0;      // for DNG EXIF creation
volatile long shutter_open_tick_count;  // for DNG EXIF creation

#ifdef CAM_CHDK_PTP_REMOTESHOOT
#include "remotecap.h"
static int ignore_current_write=0; //used by the platform routine to check whether to write the current file
#endif

void __attribute__((naked,noinline)) capt_seq_hook_raw_here()
{
 asm volatile("STMFD   SP!, {R0-R12,LR}\n");

#ifdef PAUSE_FOR_FILE_COUNTER
    // Some cameras need a slight delay for the file counter to be updated correctly
    // before raw_savefile tries to get the file name & directory.
    // Add '#define PAUSE_FOR_FILE_COUNTER 100' in the camera firmware capt_seq.c file.
    // The value can be adjusted as needed for different cameras.
    if ( (conf.save_raw && is_raw_enabled()) // Only delay if RAW enabled (prevents slowdown in HQ burst mode)
#ifdef CAM_CHDK_PTP_REMOTESHOOT
        || (remotecap_get_target() & 7) //... or if remote shooting is active
#endif //CAM_CHDK_PTP_REMOTESHOOT
    )
    {
        _SleepTask(PAUSE_FOR_FILE_COUNTER);
    }
#endif

    raw_save_stage = RAWDATA_AVAILABLE;
#ifdef CAM_CHDK_PTP_REMOTESHOOT
    if ( remotecap_get_target() == 0 ) //no remote redirection
    {
        core_rawdata_available(); //notifies spytask in core/main.c
    }
    else if ( (remotecap_get_target() & 2) > 0 ) //chdk raw requested
    {
        remotecap_raw_available();
    }
    else
    {
        raw_save_stage = RAWDATA_SAVED;
    }
#else
    core_rawdata_available(); //notifies spytask in core/main.c
#endif //CAM_CHDK_PTP_REMOTESHOOT
    while (raw_save_stage != RAWDATA_SAVED){
	_SleepTask(10);
    }

 asm volatile("LDMFD   SP!, {R0-R12,PC}\n");
}

#ifdef CAM_CHDK_PTP_REMOTESHOOT
#ifdef CAM_HAS_FILEWRITETASK_HOOK
/*
handle camera specific MAX_CHUNKS_FOR_JPEG, camp_ptp_data_chunk
get address and size of chunk N
returns NULL addr and zero size when max chunks reached
*/
static cam_ptp_data_chunk *jpeg_chunks;
void filewrite_get_jpeg_chunk(char **addr,int *size,unsigned n) {
    if(n >= MAX_CHUNKS_FOR_JPEG || jpeg_chunks == NULL) {
        *addr=(char *)0xFFFFFFFF; // signals last chunk
        *size=0;
        return;
    }
    *addr=(char *)jpeg_chunks[n].address;
    *size=jpeg_chunks[n].length;
}

void filewrite_set_discard_jpeg(int state) {
    ignore_current_write = state;
}

void filewrite_main_hook(char *name, cam_ptp_data_chunk *pdc)
{
    jpeg_chunks = pdc;
    remotecap_jpeg_available(name);
    while (remotecap_filewrite_wait()) {
        _SleepTask(10);
    }
    jpeg_chunks=NULL;

}
#endif //CAM_HAS_FILEWRITETASK_HOOK

#endif //CAM_CHDK_PTP_REMOTESHOOT

// wrapper function to store status on stack as this routine is called from assembly code in capt_seq_task()

void __attribute__((naked,noinline)) wait_until_remote_button_is_released(  ) 
{ 
asm volatile (
ASM_SAFE("BL _wait_until_remote_button_is_released\n")
 "	BX      LR \n"
	) ;
}


void hook_raw_save_complete()
{
    raw_save_stage = RAWDATA_SAVED; //spytask notifies about the finished raw save procedure
}

void __attribute__((naked,noinline)) capt_seq_hook_set_nr()
{
 asm volatile("STMFD   SP!, {R0-R12,LR}\n");
    switch (core_get_noise_reduction_value()){
    case NOISE_REDUCTION_AUTO_CANON:
        // leave it alone
#if defined(NR_AUTO)			// If value defined store it (e.g. for G12 & SX30 need to reset back to 0 to enable auto)
        *nrflag = NR_AUTO;
#endif
        break;
    case NOISE_REDUCTION_OFF:
        *nrflag = NR_OFF;
        break;
    case NOISE_REDUCTION_ON:
        *nrflag = NR_ON;
        break;
    };

 shutter_open_time=_time((void*)0);
 shutter_open_tick_count = get_tick_count();

 asm volatile("LDMFD   SP!, {R0-R12,PC}\n");
}

