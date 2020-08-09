//
// Created by matannag@wincs.cs.bgu.ac.il on 20/11/2019.
//

#include "../include/Action.h"
#include <iostream>
#include <string>
#include <vector>
#include <User.h>
#include <unordered_map>
#include <Session.h>
#include <../include/Watchable.h>
using namespace std;

BaseAction::BaseAction():errorMsg(""), status(PENDING){}

void BaseAction::complete() {this->status=COMPLETED;}

void BaseAction::error(const std::string &errorMsg) {
    this->status = ERROR;
    this->errorMsg=errorMsg;
    cout<<"ERROR - "+this->errorMsg<<endl;
}

string BaseAction::getStatus() const {
    if(this->status== COMPLETED)
        return "COMPLETED";
    else if(this->status==ERROR)
        return"ERROR";
    else{return "PENDING";}

}

ActionStatus BaseAction::getActionStatus() const {return status;}

void BaseAction::setErrorMsg(std::string msg) {this->errorMsg=msg;}

void BaseAction::setActionStatus(ActionStatus status) {this->status=status;}

string BaseAction::getErrorMsg() const {return this->errorMsg;}

CreateUser::CreateUser(string& userName, string& recAlgo): BaseAction() ,userName(userName) ,recAlgo(recAlgo){}

string CreateUser::toString() const {
    return "CreateActiveUser " + this->getStatus()+": " + this->getErrorMsg();
}

void CreateUser::act(Session &sess) {
    User* p_user;
    if(sess.isInMap(userName)){
        this->error("User already exists");
        sess.insertAction(this);
    }
    else if(recAlgo!="len" && recAlgo!="rer" && recAlgo!="gen"){
        this->error("Unauthorized algorithem");
        sess.insertAction(this);
    }
    else {
        if (recAlgo == "len") { p_user = new LengthRecommenderUser(userName);}
        else if (recAlgo == "rer") { p_user = new RerunRecommenderUser(userName);}
        else if (recAlgo == "gen") { p_user = new GenreRecommenderUser(userName);}
        sess.insertUser(p_user);
        this->complete();
        sess.insertAction(this);
    }
}
ChangeActiveUser::ChangeActiveUser(std::string &userName):BaseAction(), userName(userName) {}

string ChangeActiveUser::toString() const {
    return "ChangeActiveUser " + this->getStatus()+": " + this->getErrorMsg();
}


void ChangeActiveUser::act(Session &sess) {
    if(sess.isInMap(userName)){
        sess.setActiveUser(userName);
        this->complete();
        sess.insertAction(this);
    }
    else{
        this->error("User does not exist");
        sess.insertAction(this);
    }
}

DeleteUser::DeleteUser(std::string &userName):BaseAction(),userName(userName){}

string DeleteUser::toString() const {
    return "DeleteUser " +  this->getStatus()+": " + this->getErrorMsg();
}

void DeleteUser::act(Session &sess) {
    if (sess.getMap().find(userName)==sess.getMap().end()){
        this->error("User does not exist");
        sess.insertAction(this);
    }
    else {

        delete sess.getMap().find(userName)->second;
        sess.deleteUserFromMap(userName);
        this->complete();
        sess.insertAction(this);
    }
}


DuplicateUser::DuplicateUser(std::string &oldUserName, std::string &newUserName):BaseAction(), oldUserName(oldUserName),newUserName(newUserName) {}

string DuplicateUser::toString() const {
    return "DuplicateUser " +  this->getStatus()+": " + this->getErrorMsg();
}

void DuplicateUser::act(Session &sess) {
    if (sess.getMap().find(oldUserName)==sess.getMap().end()){
        this->error("User to duplicate does not exist");
        sess.insertAction(this);
    }
    else if ( sess.getMap().find(newUserName)!= sess.getMap().end()){
        this->error("User to create already exist");
        sess.insertAction(this);
    }
    else{
        sess.getMap().find(oldUserName)->second->duplicateUser(newUserName,sess);
        this->complete();
        sess.insertAction(this);
    }
}


PrintContentList::PrintContentList(): BaseAction() {}

string PrintContentList::toString() const {
    return "PrintContentList " +  this->getStatus()+": " + this->getErrorMsg();
}

void PrintContentList::act(Session &sess) {
    if (sess.getContent().size()==0){
        this->error("Content list is empty");
        sess.insertAction(this);
    }
    else {
        for (auto &i : sess.getContent()) {
            cout << i->toStringCont() << endl;
        }
        this->complete();
        sess.insertAction(this);
    }
}

Watch::Watch(string toWatch): BaseAction() ,toWatch(toWatch) {}

string Watch::toString() const {
    return "Watch " +  this->getStatus()+": " + this->getErrorMsg();
}

void Watch::act(Session &sess) {
    cout<< "Watching "+ sess.getContent().at(stoi(toWatch)-1)->toString()<<endl;
    sess.getActiveUser()->addToHistory(sess.getContent().at(stoi(toWatch)-1));
    this->complete();
    sess.insertAction(this);
    sess.getActiveUser()->maintainSearchAlgorithm(sess);
    Watchable* recommend = sess.getContent().at(stoi(toWatch)-1)->getNextWatchable(sess);
    if(recommend==nullptr)
        cout<<"There are no recommandation available, choose something else"<<endl;
    else{
        cout<< "We recommend watching "+ recommend->toString()+", continue watching? [y/n]"<<endl;
        string command ="";
        cin>>command;
        if(command == "y"){
            BaseAction* action = new Watch(to_string(recommend->getId()));
            action->act(sess);
        }
    }
}

PrintWatchHistory::PrintWatchHistory(): BaseAction() {}

string PrintWatchHistory::toString() const {
    return "PrintWatchHistory " +  this->getStatus()+": " + this->getErrorMsg();
}

void PrintWatchHistory::act(Session &sess) {
    if (sess.getActiveUser()->get_history().size()==0){
        this->error("Watching history is empty");
        sess.insertAction(this);
    }
    else {
        cout<<"Watch history for "+ sess.getActiveUserName()<<endl;
        int id=1;
        for (auto &i : sess.getActiveUser()->get_history()) {
            cout << to_string(id) +". "+ i->toString() << endl;
            id++;
        }
        this->complete();
        sess.insertAction(this);
    }
}

PrintActionsLog::PrintActionsLog(): BaseAction(){}

string PrintActionsLog::toString() const {
    return "PrintActionsLog " + this->getStatus()+": " + this->getErrorMsg();
}

void PrintActionsLog::act(Session &sess) {
    if (sess.getActionLog().size()==0){
        this->error("Action log is empty");
        sess.insertAction(this);
    }
    else {
        for (auto &i: sess.getActionLog()) {
            cout << i->toString() << endl;
        }
        this->complete();
        sess.insertAction(this);
    }
}

Exit::Exit(): BaseAction() {}

string Exit::toString() const {
    return "Exit " +  this->getStatus()+": " + this->getErrorMsg();
}

void Exit::act(Session &sess) {
    this->complete();
    sess.insertAction(this);
}
BaseAction::~BaseAction(){}



// clone functions for copy constructor

CreateUser* CreateUser::clone(){
    string& userNm= this->userName;
    string& algo = this->recAlgo;
    CreateUser* user = new CreateUser(userNm,algo);
    user->setActionStatus(this->getActionStatus());
    user->setErrorMsg(this->getErrorMsg());
    return user;
}

ChangeActiveUser* ChangeActiveUser::clone(){
    ChangeActiveUser* user = new ChangeActiveUser(this->userName);
    user->setErrorMsg(this->getErrorMsg());
    user->setActionStatus(this->getActionStatus());
    return user;
}

PrintWatchHistory* PrintWatchHistory::clone() {
    PrintWatchHistory* history = new PrintWatchHistory();
    history->setActionStatus(this->getActionStatus());
    history->setErrorMsg(this->getErrorMsg());
    return history;
}
DeleteUser* DeleteUser::clone(){
    DeleteUser* delet = new DeleteUser(this->userName);
    delet->setErrorMsg(this->getErrorMsg());
    delet->setActionStatus(this->getActionStatus());
    return delet;
}

DuplicateUser* DuplicateUser::clone() {
    DuplicateUser* dup = new DuplicateUser(this->oldUserName,this->newUserName);
    dup->setActionStatus(this->getActionStatus());
    dup->setErrorMsg(this->getErrorMsg());
    return dup;
}

PrintContentList* PrintContentList::clone() {
    PrintContentList* print= new PrintContentList();
    print->setErrorMsg(this->getErrorMsg());
    print->setActionStatus(this->getActionStatus());
    return print;
}

Watch* Watch::clone() {
    Watch* watch = new Watch(this->toWatch);
    watch->setActionStatus(this->getActionStatus());
    watch->setErrorMsg(this->getErrorMsg());
    return watch;
}
PrintActionsLog* PrintActionsLog::clone(){
    PrintActionsLog* print = new PrintActionsLog();
    print->setErrorMsg(this->getErrorMsg());
    print->setActionStatus(this->getActionStatus());
    return print;
}
Exit* Exit::clone() {
    Exit* exit = new Exit();
    exit->setActionStatus(this->getActionStatus());
    exit->setErrorMsg(this->getErrorMsg());
    return exit;
}


