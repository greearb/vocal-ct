#include "SoundCard.hxx"
#include "cpLog.h"
#include "rtpTypes.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
//#include "junk.cxx"

using Vocal::UA::SoundCard;
using Vocal::UA::SoundCardFormat;

extern "C"
{
    unsigned char linear2ulaw( int pcm_val );
    int ulaw2linear( unsigned char u_val );
}


int main(int argc, char** argv)
{

    if(argc > 2)
    {
	int src = open(argv[1], O_RDONLY);
	int dst = creat(argv[2], 0666);


	int count;
	unsigned char buf[2048];
	unsigned char buf2[2048];

	int byte = 0;
	int byte2 = 0;

	while((count = read(src, buf, 1024))  > 0) {
	    byte = 0;
	    byte2 = 0;
	    while(byte < count) {
		int t = (int)buf[byte+1];
		t = t << 8;
		t |= buf[byte];

		printf("%2.2x%2.2x == %4.4x\n", (int)buf[byte], 
		       (int)buf[byte+1], t);

		buf2[byte2] = linear2ulaw( t  );
		byte2++;
		byte += 2;
	    }
	    write(dst, buf2, byte2);
	}

	close(src);
	close(dst);
	exit(0);
    }

    SoundCard mySoundCard("/dev/dsp");

    // open audio hardware device
    if( mySoundCard.open() == -1 )
    {
        cpLog( LOG_ERR, "Failed to open %s", "" );
        exit( -1 );
    }

    RtpPayloadType apiCodec;

    // set audio API type
    if(mySoundCard.getFormat() == Vocal::UA::SoundCardSigned16LE)
    {
        apiCodec = rtpPayloadL16_mono;
    }
    else if (mySoundCard.getFormat() == Vocal::UA::SoundCardUlaw)
    {
        apiCodec = rtpPayloadPCMU;
    }
    else 
    {
        cpLog(LOG_ERR, "could not get valid sound card encoding");
        exit ( -1 );
    }

    cout << "apiCodec: " << apiCodec << endl;

    cpLog(LOG_DEBUG, "Opened audio device");

    int     wav = creat("tmpfile.raw", 0666);

    int cc;
    int bytes = 0;

    while (bytes < 10000) 
    {
//	cout << "t" << endl;
	unsigned char dataBuffer[1024];
	cc = mySoundCard.read( dataBuffer, 10 );
	if(cc > 0)
	{
	    write(wav, dataBuffer, cc);
	    bytes += cc;
	    cout << "d" << endl;
	}
    }

    close(wav);

    return 0;
}
