#! /bin/sh

BASE64_ENCODE_CMD=$4

VOCAL_BASE=/usr/local/vocal # VOCAL_BASE
VOCAL_BASE_CONFIGURED=@prefix@
if [ "x${VOCAL_BASE_CONFIGURED}" != "x@""prefix@" ]
then
    VOCAL_BASE=${VOCAL_BASE_CONFIGURED}
fi

if [ ! -x ${BASE64_ENCODE_CMD} ]
then
    if [ -x ${VOCAL_BASE}/bin/voicemail/base64encoder ]
    then
        BASE64_ENCODE_CMD=${VOCAL_BASE}/bin/voicemail/base64encoder
    fi
fi

DATE=`date | sed -e 's/ /_/g; s/:/_/g; s/\n//g;'`
HOSTNAME=`hostname`
echo "To: $1"
echo "From: $2"
echo "Subject: VoiceMail Message"
echo 'MIME-Version: 1.0'
echo "Content-ID: <${DATE}_0@${HOSTNAME}>"
echo 'Content-Type: audio/wav; name="VoiceMessage.wav"'
echo 'Content-Description: VoiceMessage.wav'
echo 'Content-Transfer-Encoding: base64'
echo ''
${BASE64_ENCODE_CMD} < $3
