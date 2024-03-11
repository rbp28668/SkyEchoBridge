#include <string>
#include <assert.h>
#include <memory.h>
#include "flarm_message.h"


FlarmMessage::FlarmMessage(const char* data, size_t len)
: _len(len)
, _valid(true)
, _alarmLevel(0)
, _isTraffic(false)
, _isHeartbeat(false)
, _hasAdvisory(false)
, _id(0)
{
    assert(this);
    assert(len > 11); // $ + 5chars + * + 2checksum + <CR> + <LF>
    assert(data);
    assert(data[0] == '$');

    
    // If too big then assume invalid.
    if(len > sizeof(_buffer)){
        len = sizeof(_buffer);
        _valid = false;
    }
    memcpy(_buffer,data,len);

    if(_valid){
        _valid = checkValid();
    }
    
    if(_valid){
        _alarmLevel = extractAlarmLevel();

        // PFLAA message?
        _isTraffic = startsWith("$PFLAA");

        // PFLAU message?
        _isHeartbeat = startsWith("$PFLAU");
 
        if(_isHeartbeat){
            _hasAdvisory = extractAdvisory();
        }

        if(_isTraffic){
            _id = extractId();
        }

    }


}

bool FlarmMessage::startsWith(const char* str){
    const char* pbuff = _buffer;
    while(*str){
        if(*str != *pbuff) return false;
        ++str; ++pbuff;
    }
    return true; 
}

/// @brief Gets the idx-th field of the message.
/// Field 0 is the messsage type e.g. PFLAA so the
/// first data field is retrieved by getField(1).
/// @param idx 
/// @return 
const char* FlarmMessage::getField(int idx){
    
    // Treat 0 as special case as no preceding comma.
    if(idx == 0) return _buffer+1; // skip initial $

    // Count the commas...
    int commas = 0;
    for(int i=0; i<sizeof(_buffer); ++i){
        if(_buffer[i] == ','){
            ++commas;
            if(commas == idx){
                return _buffer + i + 1; // point at character after comma.
            }
        }
    }
    return nullptr;
}

// Flarm specific message.
bool FlarmMessage::isFlarm(){
    return isValid() && (isTraffic() || isHeartbeat());
}     

// Should start with a $, have some letters, have a *, 2 checksum digits and a CRLF pair.
// Checksum should be valid.
bool FlarmMessage::checkValid(){
    bool isValid = _buffer[0] == '$' &&
    isalpha(_buffer[1]) &&
    isalpha(_buffer[2]) &&
    isalpha(_buffer[3]) &&
    true;


    // Look for terminator and calculate checksum as we go.
    int idx = 1;
	unsigned int cs = 0;
	for(int i=1; i<sizeof(_buffer); ++i, ++idx) {
        if(_buffer[idx] == '*'){
            ++idx; // skip terminating *;
            break;
        }
		unsigned char ch = _buffer[idx];
		cs ^= ch;
	}

    if(idx >= sizeof(_buffer)) return false; // no terminator

    // Ok, we've got the checksum in cs
    // Look to see if the buffer has the same checksum
    // Note - looking for exactly 2 hex digits here as well.
    const std::string digits("0123456789ABCDEF");
    char csh = _buffer[idx++];
    char csl = _buffer[idx++];

    int h = digits.find_first_of(csh);
    int l = digits.find_first_of(csl);    
    isValid = isValid && (h != digits.npos && l != digits.npos);

    int csReceived = (h << 4) + l;

    isValid = isValid && (cs == csReceived);

    isValid = isValid && (_buffer[idx++] == '\r') && (_buffer[idx++] = '\n');
    return isValid;
}



int FlarmMessage::extractAlarmLevel(){
    int alarm = 0;
    if(isHeartbeat()){
        // PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,
        // <AlarmType>,<RelativeVertical>,<RelativeDistance>[,<ID>]
        const char* field = getField(5);
        if(field != nullptr){
            alarm = atoi(field);
        }
    } else if(isTraffic()){
        // PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,
        // <RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,
        // <ClimbRate>,<AcftType>[,<NoTrack>][,<Source>,<RSSI>]
        const char* field = getField(1);
        if(field != nullptr){
            alarm = atoi(field);
        }
    }
    return alarm;
}

// Only valid for heartbeat messages.
bool FlarmMessage::extractAdvisory(){
    assert(this);
    assert(isHeartbeat());

    bool advisory = false;
    if(isHeartbeat()){
        // PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,
        // <AlarmType>,<RelativeVertical>,<RelativeDistance>[,<ID>]
        // Look for AlarmType == 4 to signal traffic advisory.
        const char* field = getField(7);
        if(field != nullptr){
            advisory = atoi(field) == 4;
        }
    }
    return advisory;
}

// address and address type combined.
// Only valid for traffic messages.
uint32_t FlarmMessage::extractId(){
    assert(this);    
    assert(isTraffic());

    uint32_t id = 0;
    if(isTraffic()){
        // PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,
        // <RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,
        // <ClimbRate>,<AcftType>[,<NoTrack>][,<Source>,<RSSI>]
        int type = 0;
        int addr = 0;
        const char* field = getField(5);
        if(field != nullptr){
            type = atoi(field);
        }
        field = getField(6);
        if(field != nullptr){
            addr = strtol(field, nullptr, 16);
        }
        id = (uint32_t)type << 24 | (uint32_t)addr;
    }
    return id;

} 