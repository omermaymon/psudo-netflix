//
// Created by matannag@wincs.cs.bgu.ac.il on 20/11/2019.
//

#include "../include/json.hpp"
#include "../include/Session.h"
#include "../include/Watchable.h"
#include <fstream>
#include <iostream>
#include <vector>
#include "../include/User.h"
#include <Action.h>


using namespace std;

// constructor
Session::Session(const std::string &configFilePath) : content(), actionsLog(), userMap(), activeUser(new LengthRecommenderUser("default")  ) {
    this->insertUser(activeUser);
    using json = nlohmann::json;
    ifstream i(configFilePath);
    json j;
    i>>j;
    json movies = j["movies"];  // all this part inserts the movies into content
    int id = 1;
    for (auto& iter : movies.items()){
        json thisFilm = iter.value();
        Movie* movie = new Movie(id,thisFilm["name"],thisFilm["length"],thisFilm["tags"]);
        content.push_back(movie);
        id++;
    }
    json tvSeries = j["tv_series"];   // all this part inserts the tv series into content
    id = content.size()+1;
    for(auto thisSeries: tvSeries.items()){
        json episode = thisSeries.value();
        vector<int> seas = episode["seasons"];
        int numOfSeas = 1;
        for (int episodes : seas) {

            for (int epNum = 1; epNum <= episodes; ++epNum) {
                Episode *ep = new Episode(id, episode["name"], episode["episode_length"], numOfSeas, epNum,episode["tags"]);
                if (epNum < episodes) // if the episode is not the last episode of the season
                    ep->setNextEp(id + 1);
                else if (epNum == episodes && (numOfSeas) < (int)(seas.size())) // if episode is last of the season but there are more seasons
                    ep->setNextEp(id + 1);
                else {
                    ep->setNextEp(-1);
                }
                content.push_back(ep);
                id++;
           }

            numOfSeas += 1;
        }
    }

}

//copy constructor............................................................

Session::Session(const Session &other):content(),actionsLog(),userMap(),activeUser(nullptr) {
    for (auto& watch:other.content) { // copy the content
       Watchable* newWatch=watch->clone();
       this->content.push_back(newWatch);
    }
    for (auto& act: other.actionsLog){ // copy the actions log
        BaseAction* newAction = act->clone();
        this->actionsLog.push_back(newAction);
    }
    for(auto& user: other.userMap) { // copy users including their respective watch history
        User *newUser = user.second->clone();
        for (Watchable* watchInHist : user.second->get_history()){
            newUser->addToHistory(this->content[watchInHist->getId()-1]);
        }
        pair<string,User*> p(newUser->getName(),newUser);
        this->userMap.insert(p);
    }
    string user = other.getActiveUserName(); // sets the active user
    this->activeUser = this->userMap.find(user)->second;

}

//move constructor..........................................................................

Session::Session(Session &&other): content(other.content), actionsLog(other.actionsLog), userMap(other.userMap),activeUser(other.activeUser){
    other.content.clear();
    other.actionsLog.clear();
    other.userMap.clear();
    other.activeUser= nullptr;
}

//copy assignment................................................................

Session& Session::operator=(Session &other) {
    if (this == &other)
        return *this;
    delete(this);
    for (auto& watch:other.content) {
        Watchable* newWatch=watch->clone();
        this->content.push_back(newWatch);
    }
    for (auto& act: other.actionsLog){
        BaseAction* newAction = act->clone();
        this->actionsLog.push_back(newAction);
    }
    for(auto& user: other.userMap) {
        User *newUser = user.second->clone();
        for (Watchable* watchInHist : user.second->get_history()){
            newUser->addToHistory(this->content[watchInHist->getId()-1]);
        }
        pair<string,User*> p(newUser->getName(),newUser);
        this->userMap.insert(p);
    }
    string user = other.getActiveUserName();
    this->activeUser = this->userMap.find(user)->second;
    return *this;

}

//move assignment..................................................................

Session& Session::operator=(Session &&other) {

    delete(this);
    this->content = other.content;
    this->actionsLog = other.actionsLog;
    this->userMap = other.userMap;
    this->activeUser = other.activeUser;
    other.content.clear();
    other.actionsLog.clear();
    other.userMap.clear();
    other.activeUser = nullptr;
    return  *this;

}

// helper functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
string Session::getActiveUserName() const {return activeUser->getName();}

void Session::insertUser(User* user) {userMap.insert({user->getName(),user});}

void Session::insertAction(BaseAction* act) {actionsLog.push_back(act);}

const unordered_map<std::string,User*>& Session:: getMap(){return userMap;}

const vector<BaseAction*>& Session:: getActionLog(){ return actionsLog;}

vector<Watchable *> & Session::getContent(){return content;}

User* Session::getActiveUser() {return activeUser;}

bool Session::isInMap(string user) {
    if (userMap.find(user)== userMap.end())
        return false;
    return true;
}

void Session::setActiveUser(string& userName){activeUser = getMap().find(userName)->second;}

void Session::deleteUserFromMap(std::string userName) {userMap.erase(userName);}

void updateVector(vector<string> &vec, string &command, string &helper){
    for(int i=0;i<(int)command.size(); i++){
        if (command[i]==' '){
            vec.push_back(helper);
            helper ="";
        }
        else{helper=helper+command[i];}
    }
    vec.push_back(helper);

}

// destructor....................................................................................
Session::~Session() {
    for (int i = 0; i <(int)content.size() ; ++i) {
        delete(content[i]);
    }
    content.clear();
    for (int j = 0; j <(int)actionsLog.size() ; ++j) {
        delete(actionsLog[j]);
    }
    actionsLog.clear();
    for(pair<string,User*> user : userMap){
        delete(user.second);
    }
    userMap.clear();

}
// running program....................................................................................................
void Session::start() {
    cout << "SPLFLIX is now on!" << endl;
    string command = "";
    vector<string>* input = new vector<string>;
    while (command != "exit") {
        getline(cin,command);
        string helper ="";
        updateVector(*input,command,helper);
        if (input->front() == "createuser"){
            BaseAction* action = new CreateUser(input->at(1),input->at(2));
            action->act(*this);

        }
        else if (input->front() == "changeuser"){
            BaseAction* action = new ChangeActiveUser(input->at(1));
            action->act(*this);
        }
        else if (input->front() == "deleteuser"){
            BaseAction* action = new DeleteUser(input->at(1));
            action->act(*this);

        }

        else if (input->front() == "dupuser"){
            BaseAction* action = new DuplicateUser(input->at(1), input->at(2));
            action->act(*this);

        }

        else if (input->front() == "content"){
            BaseAction* action = new PrintContentList();
            action->act(*this);

        }
        else if (input->front() == "watchhist"){
            BaseAction* action = new PrintWatchHistory();
            action->act(*this);

        }
        else if (input->front() == "watch"){

            BaseAction* action = new Watch(input->at(1));
            action->act(*this);

        }
        else if (input->front() == "log"){
            BaseAction* action = new PrintActionsLog();
            action->act(*this);

        }
        else if (input->front() == "exit"){
            BaseAction* action = new Exit();
            action->act(*this);

        }
        input->clear();
    }
    delete(input);

}
