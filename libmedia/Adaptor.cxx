
#include "Adaptor.hxx"

using namespace Vocal::UA;

CachedEncodedRtp::CachedEncodedRtp(const string& _key,
                                   const list<RtpPldBuffer*>& _buffers)
      : key(_key) {

   list<RtpPldBuffer*>::const_iterator cii;
   for(cii = _buffers.begin(); cii != _buffers.end(); cii++) {
      RtpPldBuffer* b = new RtpPldBuffer(*(*cii));
      buffers.push_back(b);
   }
}

void CachedEncodedRtp::addBuffer(RtpPldBuffer* b) {
   buffers.push_back(b);
}


CachedEncodedRtp::CachedEncodedRtp(const string& _key)
      : key(_key) {
}

CachedEncodedRtp::~CachedEncodedRtp() {
   list<RtpPldBuffer*>::const_iterator cii;
   for(cii = buffers.begin(); cii != buffers.end(); cii++) {
      delete (*cii); // Clean out referenced memory.
   }
}
