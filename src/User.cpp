//
// Created by matannag@wincs.cs.bgu.ac.il on 20/11/2019.
//

#include "../include/User.h"
#include "vector"
#include "../include/Session.h"
#include "../include/Watchable.h"
#include <algorithm>

using namespace std;

// constructors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
User::User(const string &name): history(),avLength(0),name(name)  {}

LengthRecommenderUser::LengthRecommenderUser(const std::string &name):User(name){}

RerunRecommenderUser::RerunRecommenderUser(const std::string &name): User(name),index(-1) {}

GenreRecommenderUser::GenreRecommenderUser(const std::string &name):User(name),genreCounter() {}
void LengthRecommenderUser::addToHistory(Watchable *watch) {
    history.push_back(watch);
    newAverage(watch);
}

// helper functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int RerunRecommenderUser::getIndex() {return this->index;}

void RerunRecommenderUser::setIndex(int i) {this->index=i;}

void RerunRecommenderUser::addToHistory(Watchable *watch) {
    history.push_back(watch);
    newAverage(watch);
}

void GenreRecommenderUser::addToHistory(Watchable *watch) {
    history.push_back(watch);
    newAverage(watch);
}

string User::getName() const {return this->name;}

void User::setAverageLength(int avLength) {this->avLength=avLength;}

vector<Watchable*> User::get_history() {return history;}

void User::setHistory(std::vector<Watchable*> vec) {this->history = vec;}

const int User::getAverageLength() {return this->avLength;}

void User::newAverage(Watchable* watch) {
    if (history.size()==0)
        avLength=watch->getLength();
    else
        avLength = ((avLength*(history.size()-1))+watch->getLength())/(history.size());
}

bool User::isInHistory(Watchable *watch) {
    for (auto &iter : history) {
        if (iter == watch)
            return true;
    }
    return false;
}

bool GenreRecommenderUser:: isGenreInTags(string tag, Watchable& watch){
    vector<string> tags = watch.getTags();
    for(auto& i: tags){
        if(i==tag)
            return true;
    }
    return  false;
}

// dupuser actions functions. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
void LengthRecommenderUser::duplicateUser(std::string newUser,Session& s) {
    User* dup = new LengthRecommenderUser(newUser);
    dup->setHistory(this->get_history());
    dup->setAverageLength(this->getAverageLength());
    s.insertUser(dup);
}
void RerunRecommenderUser::duplicateUser(std::string newUser,Session& s) {
    User* dup = new RerunRecommenderUser(newUser);
    dup->setHistory(this->get_history());
    dup->setAverageLength(this->getAverageLength());
    s.insertUser(dup);
}
void GenreRecommenderUser::duplicateUser(std::string newUser,Session& s) {
    User* dup = new GenreRecommenderUser(newUser);
    dup->setHistory(this->get_history());
    dup->setAverageLength(this->getAverageLength());
    s.insertUser(dup);
}

Watchable* LengthRecommenderUser::getRecommendation(Session &s) {
    int time = 2147483644;
    int average = this->getAverageLength();
    Watchable* nextWatch = nullptr;
    for( auto watch : s.getContent()){
        if (abs(watch->getLength()-average) < time && !isInHistory(watch)){
            time=abs(watch->getLength()-average);
            nextWatch=watch;
        }
    }
    return nextWatch;
}

vector<pair<int,string>> GenreRecommenderUser::getGenreCounter() {return this->genreCounter;}

void LengthRecommenderUser::maintainSearchAlgorithm(Session &s) {}
void RerunRecommenderUser::maintainSearchAlgorithm(Session &s) {}
void GenreRecommenderUser:: maintainSearchAlgorithm(Session& s){
    vector<string> vec = s.getActiveUser()->get_history().back()->getTags();
    for (string tag : vec){
        bool found = false;
        for (int i = 0; i<(int)genreCounter.size(); i++){
            if (genreCounter[i].second == tag){
                genreCounter[i].first--;
                found = true;
            }
        }
        if (!found){
            genreCounter.push_back(make_pair(0,tag));
        }
    }
    sort(genreCounter.begin(),genreCounter.end());
}

Watchable* GenreRecommenderUser::getRecommendation(Session &s) {

    for(pair<int,string> genreRec : genreCounter){
        string& tag = genreRec.second;
        for(Watchable* inContent : s.getContent()){
            if(isGenreInTags(tag,*inContent)&& !isInHistory(inContent))
                return inContent;
        }
    }
    return nullptr;
}

Watchable* RerunRecommenderUser::getRecommendation(Session &s) {

index++;
    return history.at(index);
}
// destructor
User::~User() {
    history.clear();
}

//clones for copies assignment / constructors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LengthRecommenderUser* LengthRecommenderUser::clone() {
    LengthRecommenderUser* user = new LengthRecommenderUser(this->getName());
    user->setAverageLength(this->getAverageLength());
    return user;
}
RerunRecommenderUser* RerunRecommenderUser::clone() {
    RerunRecommenderUser* user = new RerunRecommenderUser(this->getName());
    user->setAverageLength(this->getAverageLength());
    user->setIndex(this->getIndex());
    return user;

}
GenreRecommenderUser* GenreRecommenderUser::clone() {
    GenreRecommenderUser* user = new GenreRecommenderUser(this->getName());
    user->setAverageLength(this->getAverageLength());
    for(auto& i : this->getGenreCounter()){
        user->getGenreCounter().push_back(i);
    }
    return user;


}




