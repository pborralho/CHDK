#include "ptp.h"
#include "stdlib.h"
#include "platform.h"
#include "conf.h"
#include "remotecap.h"
static int filewrite_wait; // counter for filewrite wait, decrements for every 10ms sleep

static int available_image_data=0; // type of data available

int remotecap_get_target_support(void) {
    int ret = 0;
#ifdef CAM_CHDK_PTP_REMOTESHOOT
    ret |= PTP_CHDK_CAPTURE_RAW;
#ifdef CAM_HAS_FILEWRITETASK_HOOK
    ret |= PTP_CHDK_CAPTURE_JPG;
#endif
#endif
    return ret;
}


static int remote_file_target=0;

typedef struct {
    unsigned int address;
    unsigned int length;
} ptp_data_chunk;

#define MAX_CHUNKS_FOR_RAW 1 //raw data, could include the DNG exif

static ptp_data_chunk rawchunk[MAX_CHUNKS_FOR_RAW];
static int rawcurrchnk;
static char nameforptp[12];
static int startline=0;
static int linecount=0;

#ifdef CAM_HAS_FILEWRITETASK_HOOK

#define MAX_CHUNKS_FOR_YUV 1 //yuv data
static ptp_data_chunk yuvchunk[MAX_CHUNKS_FOR_YUV];
static int yuvcurrchnk;
static int jpegcurrchnk;
#endif //CAM_HAS_FILEWRITETASK_HOOK

int remotecap_get_target(void) {
    return remote_file_target;
}

//called to activate or deactive hooks
int remotecap_set_target ( int type, int lstart, int lcount )
{
    // unknown type or not in rec, clear and return error
    // TODO not clear REC should be required, could be valid to set / clear before switching
    if ((type & ~remotecap_get_target_support()) || !(mode_get() & MODE_REC)) {
        remote_file_target=0;
        remotecap_get_data_chunk(-1, NULL, NULL); //frees up current hook (if any)
        return 0;
    }
    remote_file_target=type;
    // clear requested
    if(type==0) {
        remotecap_get_data_chunk(-1, NULL, NULL); //frees up current hook (if any)
        return 1;
    }
    startline=lstart;
    linecount=lcount;
    return 1;
}

int remotecap_get_available_data_type(void) {
    return available_image_data;
}

void remotecap_set_available_data_type(int type)
{
    available_image_data = type;
}

void filewrite_set_discard_jpeg(int state);
void filewrite_get_jpeg_chunk(char **ardr,unsigned *size, unsigned n);

void remotecap_raw_available(void) {
    if (startline<0) startline=0;
    if (startline>CAM_RAW_ROWS-1) startline=0;
    if (linecount<=0) linecount=CAM_RAW_ROWS;
    if ( (linecount+startline)>CAM_RAW_ROWS ) linecount=CAM_RAW_ROWS-startline;
    
    rawchunk[0].address=(unsigned int)(hook_raw_image_addr()+startline*CAM_RAW_ROWPIX*CAM_SENSOR_BITS_PER_PIXEL/8 );
    if ( (startline==0) && (linecount==CAM_RAW_ROWS) )
    {
        //hook_raw_size() is sometimes different than CAM_RAW_ROWS*CAM_RAW_ROWPIX*CAM_SENSOR_BITS_PER_PIXEL/8
        // TODO above shoudln't be true!!!
        //if whole size is requested, send hook_raw_size()
        rawchunk[0].length=(unsigned int)hook_raw_size();
    }
    else
    {
        rawchunk[0].length=linecount*CAM_RAW_ROWPIX*CAM_SENSOR_BITS_PER_PIXEL/8;
    }
    rawcurrchnk=0;
// TODO this should probably just be noop if hook doesn't exist
#ifdef CAM_HAS_FILEWRITETASK_HOOK
    filewrite_set_discard_jpeg(1);
#endif
    remotecap_set_available_data_type(PTP_CHDK_CAPTURE_RAW); //notifies ptp code in core/ptp.c, first thing to happen
}

#ifdef CAM_HAS_FILEWRITETASK_HOOK
/*
called from filewrite hook to notify code that jpeg data is available
TODO name is not currently saved here
*/
void remotecap_jpeg_available(const char *name) {
    if(!(remote_file_target & (PTP_CHDK_CAPTURE_JPG | PTP_CHDK_CAPTURE_YUV | PTP_CHDK_CAPTURE_RAW))) {
        filewrite_wait = 0; // don't block filewrite task
        return;
    }
    filewrite_wait = 3000; // x10ms sleeps = 30 sec timeout, TODO make setable
    //todo:
    //- find an address for this
    //- check the current picture size (L,M1,M2,S,whatever...)
    //- implement it
    yuvchunk[0].address=0;
    yuvchunk[0].length=0;

    jpegcurrchnk=0;
    yuvcurrchnk=0;
    remotecap_set_available_data_type(remote_file_target & (PTP_CHDK_CAPTURE_JPG | PTP_CHDK_CAPTURE_YUV));
}

/*
return true until data is read, or timeout / cancel
*/
int remotecap_filewrite_wait(void) {
    if(filewrite_wait) {
        filewrite_wait--;
    }
    return filewrite_wait && remotecap_get_available_data_type();
}
#endif

// called by ptp code to get next chunk address/size for the format (fmt) that is being currently worked on
void remotecap_get_data_chunk( int fmt, char **addr, unsigned int *size )
{
    switch ( fmt )
    {
        case 0: //name
            //two ways to get this
            //1) get_file_next_counter(), may increment between the raw and filewrite hooks
            //2) from filewritetask data (only available for jpeg and yuv)
            sprintf(nameforptp,"IMG_%04d",get_target_file_num());
            *addr=&nameforptp[0];
            *size=9;
            break;
        case PTP_CHDK_CAPTURE_RAW: //raw
            if ( rawcurrchnk >= MAX_CHUNKS_FOR_RAW ) {
                *addr=(char*)0xffffffff;
                *size=0;
            }
            else {
                *addr=(char*)rawchunk[rawcurrchnk].address;
                *size=rawchunk[rawcurrchnk].length;
                rawcurrchnk+=1;
            }
            if ( (*addr==0) || (*size==0) ) {
                remotecap_set_available_data_type(available_image_data & ~PTP_CHDK_CAPTURE_RAW);
            }
            break;
#ifdef CAM_HAS_FILEWRITETASK_HOOK
        case PTP_CHDK_CAPTURE_JPG: //jpeg
            filewrite_get_jpeg_chunk(addr,size,jpegcurrchnk);
            // TODO this will require an extra request cycle,
            // would be more efficient to set a flag for last
            jpegcurrchnk+=1;
            if ( (*addr==0) || (*size==0) ) {
                remotecap_set_available_data_type(available_image_data & ~PTP_CHDK_CAPTURE_JPG);
            }
            break;
        case 4: //yuv
            // TODO this will probably be like jpeg
            if ( yuvcurrchnk >= MAX_CHUNKS_FOR_YUV ) {
                *addr=(char*)0xffffffff;
                *size=0;
            }
            else {
                *addr=(char*)yuvchunk[yuvcurrchnk].address;
                *size=yuvchunk[yuvcurrchnk].length;
                yuvcurrchnk+=1;
            }
            if ( (*addr==0) || (*size==0) ) {
                remotecap_set_available_data_type(available_image_data & ~PTP_CHDK_CAPTURE_YUV);
            }
            break;
#else
        default:
            /*
             * attempting to get an unsupported image format will result in
             * freeing up the raw hook
             */
            *addr=NULL;
            *size=0;
            remotecap_set_available_data_type(0);
#endif
    }
    if ( (!remotecap_get_available_data_type()) || (fmt == -1) ) {
        remotecap_set_available_data_type(0); // for fmt -1 case
        filewrite_wait = 0;
        // allow raw hook to continue
        // TODO could do like filewrite wait
        hook_raw_save_complete(); 
        state_shooting_progress=SHOOTING_PROGRESS_DONE; //shoot() needs this... TODO
    }
}

