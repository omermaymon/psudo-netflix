#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <set>

class Watchable;
class Session;

using namespace std;

class User{
public:
    User(const string& name);
    virtual User* clone()=0;
    virtual Watchable* getRecommendation(Session& s) = 0;
    virtual void duplicateUser ( string newUser,Session& s) = 0;
    string getName() const;
    void setAverageLength(int avLength);
    const int getAverageLength();
    vector<Watchable*> get_history();
    void setHistory(vector<Watchable*> vec); // for coping entire history from user
    void newAverage(Watchable* watch); // updates the average length
    bool isInHistory(Watchable* watch); // check if object is in users history
    virtual void addToHistory(Watchable*)=0; // virtual, adds one object to history
    virtual void maintainSearchAlgorithm(Session& s)=0; // virtual, updates the data for the search algo per user.
    virtual ~User();

protected:
    vector<Watchable*> history;
private:
    int avLength;
    string name;


};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const string& name);
    virtual LengthRecommenderUser* clone();
    virtual Watchable* getRecommendation(Session& s);
    virtual void addToHistory(Watchable*);
    virtual void duplicateUser (string newUser,Session& s);
    virtual void maintainSearchAlgorithm(Session& s);
private:

};

class RerunRecommenderUser : public User {
public:
    virtual RerunRecommenderUser* clone();
    RerunRecommenderUser(const string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual void addToHistory(Watchable*);
    virtual void duplicateUser (string newUser,Session& s);
    virtual void maintainSearchAlgorithm(Session& s);
    void setIndex(int i);
    int getIndex();
private:
    int index;
};

class GenreRecommenderUser : public User {
public:
    virtual GenreRecommenderUser* clone();
    GenreRecommenderUser(const string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual void addToHistory(Watchable*);
    virtual void duplicateUser (string newUser,Session& s);
    vector<pair<int,string>> getGenreCounter();
    bool isGenreInTags(string tag, Watchable& watch);
    virtual void maintainSearchAlgorithm(Session& s);
private:
    vector<pair<int,string>> genreCounter;

};

#endif
