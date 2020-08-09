#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>


class Session;

enum ActionStatus{
	PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
	BaseAction();
	virtual ~BaseAction();
	virtual BaseAction* clone()=0;
	std::string getStatus() const;
	virtual void act(Session& sess)=0;
	virtual std::string toString() const=0;
protected:
	void complete();
	ActionStatus getActionStatus() const ; // returns the action status
    void setErrorMsg(std::string msg);
    void setActionStatus(ActionStatus status);
	void error(const std::string& errorMsg);
	std::string getErrorMsg() const;
private:
	std::string errorMsg;
	ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    CreateUser(std::string& userName, std::string& recAlgo);
    virtual CreateUser* clone();
    virtual void act(Session& sess);
	virtual std::string toString() const;
private:
    std::string& userName;
    std::string& recAlgo;
};

class ChangeActiveUser : public BaseAction {
public:
    ChangeActiveUser(std::string& userName);
    virtual ChangeActiveUser* clone();
	virtual void act(Session& sess);
	virtual std::string toString() const;
private:
    std::string& userName;
};

class DeleteUser : public BaseAction {
public:
    DeleteUser(std::string& userName);
    virtual DeleteUser* clone();
	virtual void act(Session & sess);
	virtual std::string toString() const;
private:
    std::string& userName;
};


class DuplicateUser : public BaseAction {
public:
    DuplicateUser(std::string& oldUserName,std::string& newUserName);
    virtual DuplicateUser* clone();
	virtual void act(Session & sess);
	virtual std::string toString() const;
private:
    std::string& oldUserName;
    std::string& newUserName;
};

class PrintContentList : public BaseAction {
public:
    PrintContentList();
    virtual PrintContentList* clone();
	virtual void act (Session& sess);
	virtual std::string toString() const;
};

class PrintWatchHistory : public BaseAction {
public:
    PrintWatchHistory();
    virtual PrintWatchHistory* clone();
	virtual void act (Session& sess);
	virtual std::string toString() const;
};


class Watch : public BaseAction {
public:
    Watch(std::string toWatch);
    virtual Watch* clone();
	virtual void act(Session& sess);
	virtual std::string toString() const;

private:
    std::string toWatch;
};


class PrintActionsLog : public BaseAction {
public:
    virtual PrintActionsLog* clone();
    PrintActionsLog();
	virtual void act(Session& sess);
	virtual std::string toString() const;
};

class Exit : public BaseAction {
public:
    Exit();
    virtual Exit* clone();
	virtual void act(Session& sess);
	virtual std::string toString() const;
};
#endif
