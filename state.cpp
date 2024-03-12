// SkyEchoBridge
// Copyright R Bruce Porteous 2024
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <algorithm>
#include "state.h"
#include "outbound_flarm_converter.h"
#include "config.h"

State::State(OutboundFlarmConverter* outbound, const Config* config)
: outbound(outbound)
 , config(config)
 , heartbeatTime(0)
 , gpsAvailable(false)
 , maintRequired(false)
 , batteryLow(false)
 , utcOk(false)
 , heartbeatReceived(false)
{

}

State::~State(){
    for(auto t : traffic){
        delete t.second;
    }
}

/// @brief Receives a traffic report from the GDL-90 source
/// @param target 
void State::receivedTraffic(Target& target){
    
    //std::cout << "Traffic Lat:" << target.latitude << " lon: " << target.longitude << " Alt:" << target.altFeet << "  " << std::string(target.callsign, 8) << std::endl;
    
    // All Traffic Reports output from the GDL 90 have a Time of Applicability of the beginning of the
    // current second. Therefore, there is no explicit Time of Reception field in the Traffic Report. The
    // Time Stamp conveyed in the most recent Heartbeat message is the Time of Applicability for all
    // Traffic Reports output in that second.
    
    if(!heartbeatReceived) return;  // startup race condition - no timing until first heartbeat.
    if(target.fixInvalid()) return; // no point tracking as no position info.

    TrackedTarget* tracked(0);
    unsigned int identity = target.identity();
    auto it = traffic.find(identity);
    if( it != traffic.end()) {
        //std::cout<< "Existing target " << std::hex << std::setw(8) << identity << std::endl;
        tracked = it->second;
     } else {
        //std::cout<< "New target " << std::hex << std::setw(8) << identity << std::endl;
        tracked = new TrackedTarget(); // TODO make constructor to take a Target.
        traffic[identity] = tracked;
    }
    tracked->updateFrom(target);
    tracked->markUpdated(heartbeatTime);
}

/// @brief Receives an ownship report from the GDL-90 source
/// @param target 
void State::receivedOwnship(Target& target){
    //std::cout << "Ownship Lat:" << target.latitude << " lon: " << target.longitude << " Alt:" << target.altFeet << "  " << std::string(target.callsign, 8) << std::endl;
 
    if(!target.fixInvalid()) {
        ownship.updateFrom(target);
        ownship.markUpdated(heartbeatTime);
    }

    if(ownship.hasValidPosition(heartbeatTime)) {
        outbound->sendOwnshipData(heartbeatTime, ownship);
    }
     
}

/// @brief  Receives a HAT message from the GDL-90 source
/// @param feet 
void State::setHeightAboveTerrain(int feet){
    heightAboveTerrain = feet;
}

/// @brief receives an ownship geometric altitude report from the GDL-90 source.
/// @param altFeet 
/// @param verticalFigureOfMerit 
/// @param verticalWarning 
void State::setOwnshipGeometricAltitude(int altFeet, int verticalFigureOfMerit, bool verticalWarning){
    ownship.setGeometricAltitude(altFeet, verticalFigureOfMerit, verticalWarning);
}

/// @brief Receves a heartbeat message from the GDL-90 source.
/// @param gpsAvailable 
/// @param maintRequired 
/// @param batteryLow 
/// @param utcOk 
/// @param ts 
/// @param receivedMsgCount 
void State::setHeartbeat(bool gpsAvailable, bool maintRequired, bool batteryLow, bool utcOk, uint32_t ts, uint32_t receivedMsgCount){
    
    //std::cout << "Heartbeat at:" << ts << " gps: " << gpsAvailable << " UTC: " << utcOk << std::endl;

    // Initially send all the previous stuff.
    // Need to be careful this doesn't take so long that we lose
    // inbound messages.  Ideally we'll put this on a Q and thread it.
    if(this->heartbeatReceived){
        processCurrentState();
    }

    this->heartbeatTime = ts;
    this->gpsAvailable = gpsAvailable;
    this->maintRequired = maintRequired;
    this->batteryLow = batteryLow;
    this->utcOk = utcOk;

    this->heartbeatReceived = true;
}

/// @brief  Removes any targets that haven't had an update for the last 30 seconds.
void State::pruneOldTargets() {

     // First find keys of items to prune
    std::vector<unsigned int> toPrune;
  
    for( auto t : traffic) {
        if(heartbeatTime - t.second->lastUpdateTime() > config->oldTarget()){  
            unsigned int id = t.first;
            toPrune.push_back(id);
        }
    }
     // then delete them
    for(auto i : toPrune){
        TrackedTarget* target = traffic.at(i);
        traffic.erase(i);
        delete target;
    }
}

/// @brief Aims to come up with a number describing the immediate threat of a target
/// based on the minimum distance, vertical separation at that point and how many
/// seconds away that is.
/// Tapers down from 1 along each axis to 0 at a given distance in space or time away.
/// Final score is the product.  So anything > 0.1 (say) is likely to be a threat.
/// @param minDist 
/// @param verticalSeparation 
/// @param atTimeT 
/// @return 
float State::calculateThreat(float minDist, float verticalSeparation, float atTimeT){
    float ht = std::max( config->threatDistance() - minDist, 0.0f) / config->threatDistance();
    float vt = std::max( config->threatHeight() - fabsf(verticalSeparation),0.0f) / config->threatHeight();
    float tt = std::max( config->threatTime() - atTimeT , 0.0f) / config->threatTime();

    return ht * vt * tt;
}

// use to sort into increasing threat.
// Return true if first and second are in the right order.
static bool threatComparator(TrackedTarget* first, TrackedTarget* second){

    // If either have a threat score, sort by that.
    if(first->threat() > 0 || second->threat() > 0) {
        return first->threat() < second->threat();
    }

    // If one has an advisory needing to be sent and not the other,
    // prioritise the advisory.
    bool a1 = first->advisory() && !first-> advisorySent();
    bool a2 = second->advisory() && !second->advisorySent();
    if(a1 && !a2) return false;
    if(!a1 && a2) return true;

    // otherwise just sort by target distance (ignoring height)
    // Want to have largest distances first (lower threat)
    return first->relativeDistance() > second->relativeDistance();
}

/// @brief This should be called to process the current state of
/// of the data and transmit the results if appropriate.
void State::processCurrentState(){

    pruneOldTargets();

    // Can't do anything if we haven't got a valid position
    // for ownship (position not received or too old)
    if(ownship.hasValidPosition(heartbeatTime)){

        std::vector<TrackedTarget*> targetsToReport;

        auto ownshipPosition = ownship.extrapolatePosition(heartbeatTime);
        for( auto t : traffic) {
            TrackedTarget* target = t.second;
            //std::cout << "Processing " << std::setw(8) << std::hex << t.first << " " << std::string(target->callsign,8) << std::endl;
            
            auto targetPosition = target->extrapolatePosition(heartbeatTime);

            auto dist = Target::distance(targetPosition, ownshipPosition);
            target->setRelativeDistance(dist.first, dist.second);
            target->setRelativeVertical( (target->altFeet - ownship.altFeet) * 0.3048);
            target->setThreat(0);
            target->setAlarm(0);

            // Is this close enough to report?
            // 240kts, 4 NM a minute - 7.4km - say 10 to get a minute's warning
            if(abs(target->relativeVertical()) < config->ignoreHeight() 
                && target->relativeDistance() < config->ignoreDistance()) {
                targetsToReport.push_back(target);
            } else {
                continue;  // won't report, ignore this one.
            }


            //  Has this just come inside the close barrel?
            // In PFLAU, AlarmType 4 = traffic advisory (sent once each time an aircraft
            // enters within distance 1.5 km and vertical distance 300 m from own ship;
            if(abs(target->relativeVertical()) < config->adviseHeight() 
                && target->relativeDistance() < config->adviseDistance()) {
                if(!target->advisory()){
                    target->setAdvisory();
                }
            } else {
                target->clearAdvisory(); // as now outside the bubble.
            }
 

            // Is the target heading on a conflicting course?
            // 1 = aircraft or obstacle alarm, 13-18 seconds to impact
            // 2 = aircraft or obstacle alarm, 9-12 seconds to impact
            // 3 = aircraft or obstacle alarm, 0-8 seconds to impact
            // So 20 seconds out would be c. 2.5km
            if(abs(target->relativeVertical()) < config->threatBubbleHeight() 
                && target->relativeDistance() < config->threatBubbleDistance()){

                // Ok, so ownship at location (a,b) with velocity (va,vb).
                // Target at location (c,d) with velocity (vc,vd).
                // Centre on ownship so a=b=0 and we know (c,d) as (relativeEast, relativeNorth)

                auto ownshipVelocity = ownship.velocity();
                auto targetVelocity = target->velocity();
                float va = ownshipVelocity.second;
                float vb = ownshipVelocity.first;
                float vc = targetVelocity.second;
                float vd = targetVelocity.first;

                float c = target->relativeEast();
                float d = target->relativeNorth();

                // With a=b=0, at time t=0, effectively look at sign of rate of change.
                bool converging = (c * va + d * vb) > (c * vc + d * vd);
                if(converging){
                    float denom = (va - vc)*(va - vc) + (vb - vd)*(vb - vd);
                    if(abs(denom) < 0.01f) continue; // close to parallel courses

                    // Time of closest pass
                    float t = (c * (va - vc) + d * (vb - vd)) / denom;

                    float dx = va * t - (c + vc * t);
                    float dy = vb * t - (d + vd * t);
                    float minDist = sqrt(dx * dx + dy * dy);

                    // Vertical separation.
                    float osAlt = ownship.altFeet * 0.3048;
                    float tsAlt = target->altFeet * 0.3048;

                    if(!ownship.verticalVelocityUnknown()){
                        float osVSpeed = ownship.verticalVelocity * 0.00508f; // fpm -> fps 
                        osAlt += osVSpeed * t;
                    }
                    if(!target->verticalVelocityUnknown()){
                        float tgtVSpeed = target->verticalVelocity * 0.00508f;
                        tsAlt += tgtVSpeed * t;
                    }
                    float verticalSeparation = tsAlt - osAlt;

                    float threat = calculateThreat(minDist, verticalSeparation, t);
                    target->setThreat(threat);
                    if(threat > 0.1f){
                        // 1 = alarm, 13-18 seconds to impact
                        // 2 = alarm, 9-12 seconds to impact
                        // 3 = alarm, 0-8 seconds to impact
                        int alarm = 0;
                        if(t <= 18) alarm = 1;
                        if(t <= 12) alarm = 2;
                        if(t <= 8) alarm = 3;
                        target->setAlarm(alarm);
                    }
                }

            }
        }

        // Ok, so now sort into increasing threat and send the targets of interest.
        // In passing, see if there's something worth flagging as primary target.
        std::sort(targetsToReport.begin(), targetsToReport.end(), threatComparator);
        TrackedTarget* primaryTarget(nullptr);
        TrackedTarget* lastAdvisory(nullptr);

        for( auto t : targetsToReport) {
            outbound->sendTarget(*t);

            if(t->advisory()){
                // If a new advisory then try to send in preference to others.
                if(!t->advisorySent()) primaryTarget = t;
                lastAdvisory = t;
            }

            // But an alarm trumps everything.
            if(t->alarm() > 0) primaryTarget = t;
        }
        
        //  If nothing special but something has an advisory flag set then send that.
        if(primaryTarget == nullptr) primaryTarget = lastAdvisory;

        // GDL-90 spec says "Position is available for ADS-B Tx" against gps valid flag. 
        // HOWEVER it's misleading when the tx light is flashing on the flarm display but flarm isn't transmitting.
        // Hence can suppress it.
        bool tx = config->NoTx() ? false : gpsAvailable;    
        outbound->sendHeartbeat(traffic.size(), gpsAvailable, tx,  ownship, primaryTarget);
    }
}