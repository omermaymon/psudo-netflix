#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>

using namespace std;
class Session;

class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);
    virtual Watchable* clone() = 0; // cloner
    virtual ~Watchable(); // destructor
    virtual std::string toString() const = 0;
    virtual std::string toStringCont() const = 0; // virtual, give back the string to be printed from action "content"
    virtual Watchable* getNextWatchable(Session&) const = 0;
    const std::vector<std::string>& getTags(); // return the tags of the watchable
    int getLength() const;
    long getId() const;
protected:

    std::string tagString() const;
private:
    const long id;
    int length;
    std::vector<std::string> tags; //holds tags of specific watchable
};

class Movie : public Watchable{
public:
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    virtual Movie* clone();

    virtual std::string toString() const;
    virtual std::string toStringCont() const;
    virtual Watchable* getNextWatchable(Session&) const;

private:
    std::string name;
};


class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    virtual Episode* clone();
    const string getName();
    const int getSeason();
    const int getEpisode();
    virtual std::string toString() const;
    virtual std::string toStringCont() const;
    virtual Watchable* getNextWatchable(Session&) const;

    void setNextEp(int next);
private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;


};



#endif
