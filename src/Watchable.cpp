//
// Created by matannag@wincs.cs.bgu.ac.il on 20/11/2019.
//
#include <iostream>
#include "../include/Watchable.h"
#include <string>
#include <vector>
#include <Session.h>
#include <User.h>
using namespace std;
Watchable::Watchable(long id, int length, const std::vector<std::string> &tags):id(id),length(length),tags(tags) {}

Watchable::~Watchable() {tags.clear();}

int Watchable::getLength() const {return length;}

long Watchable::getId() const {return id;}

const vector<string>& Watchable::getTags() {return this->tags;}

string Watchable::tagString() const{
    string ret = "[";
    int size = tags.size();
    for (int i = 0; i < size; i++) {
        ret += tags[i]+", ";
    }
    ret=ret.substr(0,ret.size()-2);
    ret+=']';
    return ret;
}

Episode::Episode(long id, const std::string &seriesName, int length, int season, int episode,
                 const std::vector<std::string> &tags): Watchable(id,length,tags),seriesName(seriesName), season(season), episode(episode), nextEpisodeId() {
}
string Episode::toString() const {return seriesName+ " S" +to_string(season)+"E"+to_string(episode);}

string Episode::toStringCont() const {
    string ret;
    if(season<=9&&episode<=9)
        ret =  to_string(getId()) + ". " +seriesName +" S0" +to_string(season)+"E0"+ to_string(episode) + " " + to_string(getLength()) + " minutes" + tagString();
    else if(season>=10&&episode<=9)
        ret =  to_string(getId()) + ". " +seriesName +" S" +to_string(season)+"E0"+ to_string(episode) + " " + to_string(getLength()) + " minutes" + tagString();
    else if(season<=9&&episode>=10)
        ret =  to_string(getId()) + ". " +seriesName +" S0" +to_string(season)+"E"+ to_string(episode) + " " + to_string(getLength()) + " minutes" + tagString();
    else
        ret =  to_string(getId()) + ". " +seriesName +" S" +to_string(season)+"E"+ to_string(episode) + " " + to_string(getLength()) + " minutes" + tagString();
    return ret;
}

Watchable* Episode::getNextWatchable(Session &sess) const {

    if (nextEpisodeId == -1)
        return sess.getActiveUser()->getRecommendation(sess);
    else{
        return sess.getContent().at(nextEpisodeId-1);
    }
}

void Episode::setNextEp(int next) {this->nextEpisodeId = next;}

const string Episode :: getName(){return this->seriesName;}

const int Episode::getSeason() {return this->season;}

const int Episode::getEpisode() {return  this->episode;}

Movie::Movie(long id, const std::string &name, int length, const std::vector<std::string> &tags):Watchable(id,length,tags), name(name) {}

string Movie::toString() const {return name;}

string Movie::toStringCont() const  {
    string ret = to_string(getId())+". " +name+" " + to_string(getLength())+" minutes "+tagString();
    return ret;
}

Watchable* Movie::getNextWatchable(Session &sess) const {return sess.getActiveUser()->getRecommendation(sess);}

//clones for copies assignment / constructors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Movie* Movie::clone() {
    Movie* movie = new Movie(this->getId(),this->toString() ,this->getLength(),this->getTags());
    //Movie* movie = new Movie(*this);
    return movie;
}

Episode* Episode::clone() {
    Episode* episode = new Episode(this->getId(),this->getName(),this->getLength(),this->getSeason(),this->getEpisode(),this->getTags());
    return  episode;
}

//copy Assignment................................................................

