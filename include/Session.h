#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"

using namespace std;
class User;
class Watchable;

class Session{
public:
    Session(const std::string &configFilePath);
    ~Session(); // destructor
    void start();
    Session(const Session& other); //copy constructor
    Session(Session &&);//move constructor
    Session& operator=(Session &);// copy assignment
    Session& operator=(Session &&); // move assignment
    const std::unordered_map<std::string,User*>& getMap();
    std::vector<Watchable*>& getContent();
    void insertUser(User* user); // inserts a user to the map
    void insertAction(BaseAction* act); // inserts an action to the log
    User* getActiveUser(); // get the active user
    const std::vector<BaseAction*>& getActionLog();
    void setActiveUser(std::string& userName);
    bool isInMap(std::string u);
    void deleteUserFromMap(std::string userName);
    string getActiveUserName() const; // returns the name of the active user

private:
    std::vector<Watchable*> content; // all the watchables
    std::vector<BaseAction*> actionsLog; // list of History actions
    std::unordered_map<std::string,User*> userMap; //list of users
    User* activeUser; //active user

};
#endif