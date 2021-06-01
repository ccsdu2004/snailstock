#ifndef SIGSLOT_H
#define SIGSLOT_H
#include <set>
#include <list>

class SlotHolder;

class BasicConnection0
{
public:
    virtual SlotHolder* targetSlotHolder() const = 0;
    virtual void submit() = 0;
    virtual BasicConnection0* clone() = 0;
	virtual BasicConnection0* duplicate(SlotHolder* newTarget) = 0;
};

template<class Type1>
class BasicConnection1
{
public:
	virtual SlotHolder* targetSlotHolder() const = 0;
	virtual void submit(Type1) = 0;
	virtual BasicConnection1<Type1>* clone() = 0;
	virtual BasicConnection1<Type1>* duplicate(SlotHolder* newTarget) = 0;
};

template<class Type1, class Type2>
class BasicConnection2
{
public:
	virtual SlotHolder* targetSlotHolder() const = 0;
	virtual void submit(Type1, Type2) = 0;
	virtual BasicConnection2<Type1, Type2>* clone() = 0;
	virtual BasicConnection2<Type1, Type2>* duplicate(SlotHolder* newTarget) = 0;
};

template<class Type1, class Type2, class Type3>
class BasicConnection3
{
public:
	virtual SlotHolder* targetSlotHolder() const = 0;
	virtual void submit(Type1, Type2, Type3) = 0;
	virtual BasicConnection3<Type1, Type2, Type3>* clone() = 0;
	virtual BasicConnection3<Type1, Type2, Type3>* duplicate(SlotHolder* newTarget) = 0;
};

template<class Type1, class Type2, class Type3, class Type4>
class BasicConnection4
{
public:
	virtual SlotHolder* targetSlotHolder() const = 0;
    virtual void submit(Type1, Type2, Type3, Type4) = 0;
	virtual BasicConnection4<Type1, Type2, Type3, Type4>* clone() = 0;
	virtual BasicConnection4<Type1, Type2, Type3, Type4>* duplicate(SlotHolder* newTarget) = 0;
};

template<class Type1, class Type2, class Type3, class Type4, class Type5>
class BasicConnection5
{
public:
	virtual SlotHolder* targetSlotHolder() const = 0;
    virtual void submit(Type1, Type2, Type3, Type4, Type5) = 0;
	virtual BasicConnection5<Type1, Type2, Type3, Type4, Type5>* clone() = 0;
	virtual BasicConnection5<Type1, Type2, Type3, Type4, Type5>* duplicate(SlotHolder* newTarget) = 0;
};

class BasicSignal
{
public:
    virtual ~BasicSignal() {}
	virtual void disconnectSlot(SlotHolder* slot) = 0;
	virtual void duplicateSlot(const SlotHolder* oldSlot, SlotHolder* newSlot) = 0;
};

class SlotHolder
{
private:
	typedef std::set<BasicSignal*> SenderSet;
	typedef SenderSet::const_iterator const_iterator;
public:
	SlotHolder() {}

	SlotHolder(const SlotHolder& holder)
	{
		const_iterator it    = holder.senders_.begin();
		const_iterator itEnd = holder.senders_.end();

		while(it != itEnd)
		{
			(*it)->duplicateSlot(&holder, this);
			senders_.insert(*it);
			++it;
		}
	}

	void connectTo(BasicSignal* sender)
	{
		senders_.insert(sender);
	}

	void disconnectFrom(BasicSignal* sender)
	{
		senders_.erase(sender);
	}

	virtual ~SlotHolder()
	{
		disconnectAll();
	}

	void disconnectAll()
	{
		const_iterator it = senders_.begin();
		const_iterator itEnd = senders_.end();

		while(it != itEnd)
		{
			(*it)->disconnectSlot(this);
			++it;
		}

		senders_.erase(senders_.begin(), senders_.end());
	}
private:
    SenderSet senders_;
};

class BasicSignal0 : public BasicSignal
{
public:
	typedef std::list<BasicConnection0 *>  ConnectionList;

	BasicSignal0() {}

	BasicSignal0(const BasicSignal0& s) : BasicSignal(s)
	{
        ConnectionList::const_iterator it = s.connectedSlots_.begin();
	    ConnectionList::const_iterator itEnd = s.connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->connectTo(this);
			connectedSlots_.push_back((*it)->clone());
		    ++it;
		}
	}

    ~BasicSignal0()
	{
		disconnectAll();
	}

	void disconnectAll()
	{
        ConnectionList::const_iterator it = connectedSlots_.begin();
		ConnectionList::const_iterator itEnd = connectedSlots_.end();

	    while(it != itEnd)
		{
			(*it)->targetSlotHolder()->disconnectFrom(this);
			delete *it;
            ++it;
		}

	    connectedSlots_.erase(connectedSlots_.begin(), connectedSlots_.end());
	}

	void disconnect(SlotHolder* slotHolder)
	{
		ConnectionList::iterator it = connectedSlots_.begin();
		ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			if((*it)->targetSlotHolder() == slotHolder)
		    {
				delete *it;
				connectedSlots_.erase(it);
				slotHolder->disconnectFrom(this);
				return;
			}

		    ++it;
		}
	}

	void disconnectSlot(SlotHolder* slot)
	{
		ConnectionList::iterator it = connectedSlots_.begin();
		ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			ConnectionList::iterator itNext = it;
			++itNext;

		    if((*it)->targetSlotHolder() == slot)
			{
				connectedSlots_.erase(it);
				//!            delete *it;
			}

			it = itNext;
		}
	}

	void duplicateSlot(const SlotHolder* oldTarget, SlotHolder* newTarget)
	{
		ConnectionList::iterator it = connectedSlots_.begin();
		ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
		    if((*it)->targetSlotHolder() == oldTarget)
			{
			    connectedSlots_.push_back((*it)->duplicate(newTarget));
			}

			++it;
		}
	}
protected:
    ConnectionList connectedSlots_;
};

template<class Type1>
class BasicSignal1 : public BasicSignal
{
public:
	typedef std::list<BasicConnection1<Type1> *>  ConnectionList;

	BasicSignal1(){}

    BasicSignal1(const BasicSignal1<Type1>& s) : BasicSignal(s)
	{
		typename ConnectionList::const_iterator it = s.connectedSlots_.begin();
		typename ConnectionList::const_iterator itEnd = s.connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->connectTo(this);
			connectedSlots_.push_back((*it)->clone());
            ++it;
		}
	}

	void duplicateSlot(const SlotHolder* oldTarget, SlotHolder* newTarget)
	{
        typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
            if((*it)->targetSlotHolder() == oldTarget)
			{
				connectedSlots_.push_back((*it)->duplicate(newTarget));
			}

			++it;
		}
	}

	~BasicSignal1()
	{
		disconnectAll();
	}

	void disconnectAll()
	{
        typename ConnectionList::const_iterator it = connectedSlots_.begin();
	    typename ConnectionList::const_iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->disconnectFrom(this);
			delete *it;
			++it;
		}

		connectedSlots_.erase(connectedSlots_.begin(), connectedSlots_.end());
	}

	void disconnect(SlotHolder* slotHolder)
	{
        typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			if((*it)->targetSlotHolder() == slotHolder)
			{
				delete *it;
				connectedSlots_.erase(it);
				slotHolder->disconnectFrom(this);
				return;
			}

			++it;
		}
	}

	void disconnectSlot(SlotHolder* slot)
	{
        typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			typename ConnectionList::iterator itNext = it;
			++itNext;

			if((*it)->targetSlotHolder() == slot)
			{
			    connectedSlots_.erase(it);
				//            delete *it;
			}

			it = itNext;
		}
	}
protected:
	ConnectionList connectedSlots_;
};

template<class Type1, class Type2>
class BasicSignal2 : public BasicSignal
{
public:
    typedef std::list<BasicConnection2<Type1, Type2> *> ConnectionList;

    BasicSignal2() {}

    BasicSignal2(const BasicSignal2<Type1, Type2>& s) : BasicSignal(s)
    {
        typename ConnectionList::const_iterator it = s.connectedSlots_.begin();
        typename ConnectionList::const_iterator itEnd = s.connectedSlots_.end();

        while(it != itEnd)
        {
            (*it)->targetSlotHolder()->connectTo(this);
            connectedSlots_.push_back((*it)->clone());
            ++it;
        }
    }

    void duplicateSlot(const SlotHolder* oldTarget, SlotHolder* newTarget)
    {
        typename ConnectionList::iterator it = connectedSlots_.begin();
        typename ConnectionList::iterator itEnd = connectedSlots_.end();

        while(it != itEnd)
        {
            if((*it)->targetSlotHolder() == oldTarget)
            {
                connectedSlots_.push_back((*it)->duplicate(newTarget));
            }

            ++it;
        }
    }

    ~BasicSignal2()
    {
        disconnectAll();
    }

    void disconnectAll()
    {
	    typename ConnectionList::const_iterator it = connectedSlots_.begin();
		typename ConnectionList::const_iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->disconnectFrom(this);
			delete *it;
            ++it;
		}

		connectedSlots_.erase(connectedSlots_.begin(), connectedSlots_.end());
    }

    void disconnect(SlotHolder* slotHolder)
    {
        typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			if((*it)->targetSlotHolder() == slotHolder)
			{
				delete *it;
				connectedSlots_.erase(it);
				slotHolder->disconnectFrom(this);
				return;
			}
            ++it;
		}
	}

	void disconnectSlot(SlotHolder* slot)
	{
        typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			typename ConnectionList::iterator itNext = it;
			++itNext;

			if((*it)->targetSlotHolder() == slot)
			{
				connectedSlots_.erase(it);
				//  delete *it;
			}

			it = itNext;
		}
	}

protected:
	ConnectionList connectedSlots_;
};

template<class Type1, class Type2, class Type3>
class BasicSignal3 : public BasicSignal
{
public:
	typedef std::list<BasicConnection3<Type1, Type2, Type3> *> ConnectionList;

	BasicSignal3() {}

	BasicSignal3(const BasicSignal3<Type1, Type2, Type3>& s) : BasicSignal(s)
	{
		typename ConnectionList::const_iterator it = s.connectedSlots_.begin();
		typename ConnectionList::const_iterator itEnd = s.connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->connectTo(this);
			connectedSlots_.push_back((*it)->clone());
            ++it;
		}
	}

	void duplicateSlot(const SlotHolder* oldTarget, SlotHolder* newTarget)
	{
        typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			if((*it)->targetSlotHolder() == oldTarget)
			{
				connectedSlots_.push_back((*it)->duplicate(newTarget));
			}
			++it;
		}
	}

	~BasicSignal3()
	{
		disconnectAll();
	}

	void disconnectAll()
	{
		typename ConnectionList::const_iterator it = connectedSlots_.begin();
		typename ConnectionList::const_iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->disconnectFrom(this);
			delete *it;
			++it;
		}

		connectedSlots_.erase(connectedSlots_.begin(), connectedSlots_.end());
	}

	void disconnect(SlotHolder* slotHolder)
	{
		typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			if((*it)->targetSlotHolder() == slotHolder)
			{
				delete *it;
				connectedSlots_.erase(it);
				slotHolder->disconnectFrom(this);
				return;
			}

			++it;
		}
	}

	void disconnectSlot(SlotHolder* slot)
	{
		typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			typename ConnectionList::iterator itNext = it;
			++itNext;

			if((*it)->targetSlotHolder() == slot)
			{
				connectedSlots_.erase(it);
				//            delete *it;
			}
		    it = itNext;
		}
	}

protected:
	ConnectionList connectedSlots_;
};

template<class Type1, class Type2, class Type3, class Type4>
class BasicSignal4 : public BasicSignal
{
public:
	typedef std::list<BasicConnection4<Type1,Type2,Type3,Type4> *> ConnectionList;

	BasicSignal4(){}

	BasicSignal4(const BasicSignal4<Type1,Type2,Type3,Type4>& s):BasicSignal(s)
	{
		typename ConnectionList::const_iterator it = s.connectedSlots_.begin();
		typename ConnectionList::const_iterator itEnd = s.connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->connectTo(this);
			connectedSlots_.push_back((*it)->clone());
            ++it;
		}
	}

	void duplicateSlot(const SlotHolder* oldTarget, SlotHolder* newTarget)
	{
		typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			if((*it)->targetSlotHolder() == oldTarget)
			{
				connectedSlots_.push_back((*it)->duplicate(newTarget));
			}

			++it;
		}
	}

	~BasicSignal4()
	{
		disconnectAll();
	}

	void disconnectAll()
	{
		typename ConnectionList::const_iterator it = connectedSlots_.begin();
		typename ConnectionList::const_iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->disconnectFrom(this);
			delete *it;
            ++it;
		}

		connectedSlots_.erase(connectedSlots_.begin(), connectedSlots_.end());
	}

	void disconnect(SlotHolder* slotHolder)
	{
		typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			if((*it)->targetSlotHolder() == slotHolder)
			{
				delete *it;
				connectedSlots_.erase(it);
				slotHolder->disconnectFrom(this);
				return;
			}

			++it;
		}
	}

	void disconnectSlot(SlotHolder* slot)
	{
		typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			typename ConnectionList::iterator itNext = it;
			++itNext;

			if((*it)->targetSlotHolder() == slot)
			{
				connectedSlots_.erase(it);
				//  delete *it;
			}

			it = itNext;
		}
	}

protected:
	ConnectionList connectedSlots_;
};

template<class Type1, class Type2, class Type3, class Type4, class Type5>
class BasicSignal5 : public BasicSignal
{
public:
	typedef std::list<BasicConnection5<Type1,Type2,Type3,Type4,Type5>*>  ConnectionList;

	BasicSignal5(){}
    BasicSignal5(const BasicSignal5<Type1, Type2, Type3, Type4, Type5>& s) : BasicSignal(s)
	{
		typename ConnectionList::const_iterator it = s.connectedSlots_.begin();
		typename ConnectionList::const_iterator itEnd = s.connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->connectTo(this);
			connectedSlots_.push_back((*it)->clone());
            ++it;
		}
	}

	void duplicateSlot(const SlotHolder* oldTarget, SlotHolder* newTarget)
	{
		typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			if((*it)->targetSlotHolder() == oldTarget)
			{
				connectedSlots_.push_back((*it)->duplicate(newTarget));
			}

			++it;
		}
	}

	~BasicSignal5()
	{
		disconnectAll();
	}

	void disconnectAll()
	{
		typename ConnectionList::const_iterator it = connectedSlots_.begin();
		typename ConnectionList::const_iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			(*it)->targetSlotHolder()->disconnectFrom(this);
			delete *it;
            ++it;
		}

		connectedSlots_.erase(connectedSlots_.begin(), connectedSlots_.end());
	}

	void disconnect(SlotHolder* slotHolder)
	{
		typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			if((*it)->targetSlotHolder() == slotHolder)
			{
				delete *it;
				connectedSlots_.erase(it);
				slotHolder->disconnectFrom(this);
				return;
			}

			++it;
		}
	}

	void disconnectSlot(SlotHolder* slot)
	{
		typename ConnectionList::iterator it = connectedSlots_.begin();
		typename ConnectionList::iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			typename ConnectionList::iterator itNext = it;
			++itNext;

			if((*it)->targetSlotHolder() == slot)
			{
				connectedSlots_.erase(it);
				//  delete *it;
			}

			it = itNext;
		}
	}
protected:
	ConnectionList connectedSlots_;
};

template<class TargetType>
class Connection0 : public BasicConnection0
{
public:
	Connection0()
	{
		object_ = 0;
		memberFunction_ = 0;
		voidMemberFunction_ = 0;
	}

	Connection0(TargetType* anObject, void (TargetType::*aMemberFunction)())
	{
		object_ = anObject;
		memberFunction_ = 0;
		voidMemberFunction_ = aMemberFunction;
	}

	Connection0(TargetType* anObject, TargetType& (TargetType::*aMemberFunction)())
	{
		object_ = anObject;
		memberFunction_ = aMemberFunction;
	}

	virtual BasicConnection0* clone()
	{
		return new Connection0<TargetType>(*this);
	}

	virtual BasicConnection0* duplicate(SlotHolder* newTarget)
	{
		return new Connection0<TargetType>((TargetType *)newTarget, memberFunction_);
	}

	virtual void submit()
	{
		if(memberFunction_ != 0)
			(object_->*memberFunction_)();
		else
			(object_->*voidMemberFunction_)();
	}

	virtual SlotHolder* targetSlotHolder() const
	{
		return object_;
	}

private:
    TargetType* object_;
	TargetType& (TargetType::* memberFunction_)();
	void (TargetType::* voidMemberFunction_)();
};

template<class TargetType, class Type1>
class Connection1 : public BasicConnection1<Type1>
{
public:
	Connection1()
	{
		object_ = 0;
		memberFunction_ = 0;
		voidMemberFunction_ = 0;
	}

	Connection1(TargetType* anObject, void (TargetType::*aMemberFunction)(Type1))
	{
		object_ = anObject;
		memberFunction_ = 0;
		voidMemberFunction_ = aMemberFunction;
	}

	Connection1(TargetType* anObject, TargetType& (TargetType::*aMemberFunction)(Type1))
	{
		object_ = anObject;
		voidMemberFunction_ = 0;
		memberFunction_ = aMemberFunction;
	}

	virtual BasicConnection1<Type1>* clone()
	{
		return new Connection1<TargetType, Type1>(*this);
	}

    virtual BasicConnection1<Type1>* duplicate(SlotHolder* newTarget)
	{
		return new Connection1<TargetType, Type1>((TargetType *)newTarget, memberFunction_);
	}

	virtual void submit(Type1 a1)
	{
		if(memberFunction_ != 0)
			(object_->*memberFunction_)(a1);
		else
			(object_->*voidMemberFunction_)(a1);
	}

	virtual SlotHolder* targetSlotHolder() const
	{
		return object_;
	}

private:
	TargetType* object_;
	void (TargetType::* voidMemberFunction_)(Type1);
	TargetType& (TargetType::* memberFunction_)(Type1);
};

template<class TargetType, class Type1, class Type2>
class Connection2 : public BasicConnection2<Type1, Type2>
{
public:
	Connection2()
	{
		object_ = 0;
		memberFunction_ = 0;
		voidMemberFunction_ = 0;
	}

	Connection2(TargetType* anObject, void (TargetType::*aMemberFunction)(Type1, Type2))
	{
		object_ = anObject;
		memberFunction_ = 0;
		voidMemberFunction_ = aMemberFunction;
	}

	Connection2(TargetType* anObject, TargetType& (TargetType::*aMemberFunction)(Type1, Type2))
	{
		object_ = anObject;
		voidMemberFunction_ = 0;
		memberFunction_ = aMemberFunction;
	}

	virtual BasicConnection2<Type1, Type2>* clone()
	{
		return new Connection2<TargetType, Type1, Type2>(*this);
	}

    virtual BasicConnection2<Type1, Type2>* duplicate(SlotHolder* newTarget)
	{
		return new Connection2<TargetType, Type1, Type2>((TargetType *)newTarget, memberFunction_);
	}

	virtual void submit(Type1 a1, Type2 a2)
	{
		if(memberFunction_ != 0)
			(object_->*memberFunction_)(a1, a2);
		else
			(object_->*voidMemberFunction_)(a1, a2);
	}

	virtual SlotHolder* targetSlotHolder() const
	{
		return object_;
	}

private:
	TargetType* object_;
    void (TargetType::* voidMemberFunction_)(Type1, Type2);
	TargetType& (TargetType::* memberFunction_)(Type1, Type2);
};

template<class TargetType, class Type1, class Type2, class Type3>
class Connection3 : public BasicConnection3<Type1, Type2, Type3>
{
public:
	Connection3()
	{
		object_ = 0;
	    memberFunction_ = 0;
		voidMemberFunction_ = 0;
	}

	Connection3(TargetType* anObject, void (TargetType::*aMemberFunction)(Type1, Type2, Type3))
	{
		object_ = anObject;
		memberFunction_ = 0;
		voidMemberFunction_ = aMemberFunction;
	}

	Connection3(TargetType* anObject, TargetType& (TargetType::*aMemberFunction)(Type1, Type2, Type3))
	{
		object_ = anObject;
		voidMemberFunction_ = 0;
		memberFunction_ = aMemberFunction;
	}

	virtual BasicConnection3<Type1, Type2, Type3>* clone()
	{
		return new Connection3<TargetType, Type1, Type2, Type3>(*this);
	}

	virtual BasicConnection3<Type1, Type2, Type3>* duplicate(SlotHolder* newTarget)
	{
		return new Connection3<TargetType, Type1, Type2, Type3>((TargetType *)newTarget, memberFunction_);
	}

	virtual void submit(Type1 a1, Type2 a2, Type3 a3)
	{
		if(memberFunction_ != 0)
			(object_->*memberFunction_)(a1, a2, a3);
		else
			(object_->*voidMemberFunction_)(a1, a2, a3);
	}

	virtual SlotHolder* targetSlotHolder() const
	{
		return object_;
	}

private:
	TargetType* object_;
	void (TargetType::* voidMemberFunction_)(Type1, Type2, Type3);
	TargetType& (TargetType::* memberFunction_)(Type1, Type2, Type3);
};

template<class TargetType, class Type1, class Type2, class Type3, class Type4>
class Connection4 : public BasicConnection4<Type1, Type2, Type3, Type4>
{
public:
	Connection4()
	{
		object_ = 0;
		memberFunction_ = 0;
		voidMemberFunction_ = 0;
	}

	Connection4(TargetType* anObject, void (TargetType::*aMemberFunction)(Type1,
		Type2, Type3, Type4))
	{
		object_ = anObject;
		memberFunction_ = 0;
		voidMemberFunction_ = aMemberFunction;
	}

	Connection4(TargetType* anObject, TargetType& (TargetType::*aMemberFunction)(Type1,
		Type2, Type3, Type4))
	{
		object_ = anObject;
		voidMemberFunction_ = 0;
		memberFunction_ = aMemberFunction;
	}

	virtual BasicConnection4<Type1, Type2, Type3, Type4>* clone()
	{
		return new Connection4<TargetType, Type1, Type2, Type3, Type4>(*this);
	}

	virtual BasicConnection4<Type1, Type2, Type3, Type4>* duplicate(SlotHolder* newTarget)
	{
		return new Connection4<TargetType, Type1, Type2, Type3, Type4>((TargetType *)newTarget, memberFunction_);
	}

	virtual void submit(Type1 a1, Type2 a2, Type3 a3, Type4 a4)
	{
		if(memberFunction_ != 0)
			(object_->*memberFunction_)(a1, a2, a3, a4);
		else
			(object_->*voidMemberFunction_)(a1, a2, a3, a4);
	}

	virtual SlotHolder* targetSlotHolder() const
	{
		return object_;
	}

private:
	TargetType* object_;
	void (TargetType::* voidMemberFunction_)(Type1, Type2, Type3, Type4);
	TargetType& (TargetType::* memberFunction_)(Type1, Type2, Type3, Type4);
};

template<class TargetType, class Type1, class Type2, class Type3, class Type4, class Type5>
class Connection5 : public BasicConnection5<Type1, Type2, Type3, Type4, Type5>
{
public:
	Connection5()
	{
		object_ = 0;
		memberFunction_ = 0;
		voidMemberFunction_ = 0;
	}

	Connection5(TargetType* anObject, void (TargetType::*aMemberFunction)(Type1,
		Type2, Type3, Type4, Type5))
	{
		object_ = anObject;
		memberFunction_ = 0;
		voidMemberFunction_ = aMemberFunction;
	}

	Connection5(TargetType* anObject, TargetType& (TargetType::*aMemberFunction)(Type1,
		Type2, Type3, Type4, Type5))
	{
		object_ = anObject;
		voidMemberFunction_ = 0;
		memberFunction_ = aMemberFunction;
	}

	virtual BasicConnection5<Type1, Type2, Type3, Type4,Type5>* clone()
	{
		return new Connection5<TargetType, Type1, Type2, Type3, Type4,Type5>(*this);
	}

	virtual BasicConnection5<Type1, Type2, Type3, Type4,Type5>* duplicate(SlotHolder* newTarget)
	{
		return new Connection5<TargetType, Type1, Type2, Type3, Type4,Type5>((TargetType *)newTarget, memberFunction_);
	}

	virtual void submit(Type1 a1, Type2 a2, Type3 a3, Type4 a4, Type5 a5)
	{
		if(memberFunction_ != 0)
			(object_->*memberFunction_)(a1, a2, a3, a4, a5);
		else
			(object_->*voidMemberFunction_)(a1, a2, a3, a4, a5);
	}

	virtual SlotHolder* targetSlotHolder() const
	{
		return object_;
	}
private:
	TargetType* object_;
	void (TargetType::* voidMemberFunction_)(Type1, Type2, Type3, Type4, Type5);
	TargetType& (TargetType::* memberFunction_)(Type1, Type2, Type3, Type4, Type5);
};

class Signal0 : public BasicSignal0
{
public:
	typedef BasicSignal0::ConnectionList ConnectionList;
public:
	Signal0() {}
	Signal0(const Signal0& s) : BasicSignal0(s) {}

	template<class TargetType>
	void connect(TargetType* slotHolder, void (TargetType::*aMemberFunction)())
	{
		Connection0<TargetType>* conn =
			new Connection0<TargetType>(slotHolder, aMemberFunction);
		connectedSlots_.push_back(conn);
		slotHolder->connectTo(this);
	}

	template<class TargetType>
	void connect(TargetType* slotHolder, TargetType& (TargetType::*aMemberFunction)())
	{
		Connection0<TargetType>* conn =
			new Connection0<TargetType>(slotHolder, aMemberFunction);
		connectedSlots_.push_back(conn);
		slotHolder->connectTo(this);
	}

    void submit()
	{
		ConnectionList::const_iterator itNext, it = connectedSlots_.begin();
		ConnectionList::const_iterator itEnd = connectedSlots_.end();

		while(it != itEnd)
		{
			itNext = it;
			++itNext;
			(*it)->submit();
			it = itNext;
		}
	}

	void operator()()
	{
		submit();
	}
};

template<class Type1>
class Signal1 : public BasicSignal1<Type1>
{
    //��dev c++�»��� connectedSlots_û�ж���,
    std::list<BasicConnection1<Type1> *> connectedSlots_;
public:
    typedef typename BasicSignal1<Type1>::ConnectionList ConnectionList;
public:
    Signal1() {}
    Signal1(const Signal1<Type1>& s) : BasicSignal1<Type1>(s) {}

    template<class TargetType>
    void connect(TargetType* slotHolder, void (TargetType::*aMemberFunction)(Type1))
    {
        Connection1<TargetType, Type1>* conn =
            new Connection1<TargetType, Type1>(slotHolder, aMemberFunction);
        connectedSlots_.push_back(conn);
        slotHolder->connectTo(this);
    }

    template<class TargetType>
    void connect(TargetType* slotHolder, TargetType& (TargetType::*aMemberFunction)(Type1))
    {
        Connection1<TargetType, Type1>* conn =
            new Connection1<TargetType, Type1>(slotHolder, aMemberFunction);
        connectedSlots_.push_back(conn);
        slotHolder->connectTo(this);
    }

    void submit(Type1 a1)
    {
        typename ConnectionList::const_iterator itNext, it = connectedSlots_.begin();
        typename ConnectionList::const_iterator itEnd = connectedSlots_.end();

        while(it != itEnd)
        {
            itNext = it;
            ++itNext;

            (*it)->submit(a1);
            it = itNext;
        }
    }

    void operator()(Type1 a1)
    {
        submit(a1);
    }
};

template<class Type1, class Type2>
class Signal2 : public BasicSignal2<Type1, Type2>
{
public:
	typedef typename BasicSignal2<Type1, Type2>::ConnectionList ConnectionList;
public:
	Signal2() {}
    Signal2(const Signal2<Type1, Type2>& s) : BasicSignal2<Type1, Type2>(s) {}

	template<class TargetType>
	void connect(TargetType* slotHolder, void (TargetType::*aMemberFunction)(Type1, Type2))
	{
		Connection2<TargetType, Type1, Type2>* conn = new
			Connection2<TargetType, Type1, Type2>(slotHolder, aMemberFunction);
		BasicSignal2<Type1, Type2>::connectedSlots_.push_back(conn);
		slotHolder->connectTo(this);
	}

	template<class TargetType>
	void connect(TargetType* slotHolder, TargetType& (TargetType::*aMemberFunction)(Type1, Type2))
	{
		Connection2<TargetType, Type1, Type2>* conn = new
			Connection2<TargetType, Type1, Type2>(slotHolder, aMemberFunction);
		BasicSignal2<Type1, Type2>::connectedSlots_.push_back(conn);
		slotHolder->connectTo(this);
	}

	void submit(Type1 a1, Type2 a2)
	{
		typename ConnectionList::const_iterator itNext, it = BasicSignal2<Type1, Type2>::connectedSlots_.begin();
		typename ConnectionList::const_iterator itEnd = BasicSignal2<Type1, Type2>::connectedSlots_.end();

		while(it != itEnd)
		{
			itNext = it;
			++itNext;

			(*it)->submit(a1, a2);
            it = itNext;
		}
	}

	void operator()(Type1 a1, Type2 a2)
	{
		submit(a1,a2);
	}
};

template<class Type1, class Type2, class Type3>
class Signal3 : public BasicSignal3<Type1, Type2, Type3>
{
public:
	typedef typename BasicSignal3<Type1, Type2, Type3>::ConnectionList ConnectionList;
public:
	Signal3() {}
    Signal3(const Signal3<Type1, Type2, Type3>& s) : BasicSignal3<Type1, Type2, Type3>(s) {}

	template<class TargetType>
	void connect(TargetType* slotHolder, void (TargetType::*aMemberFunction)(Type1,Type2,Type3))
	{
		Connection3<TargetType, Type1, Type2, Type3>* conn = new Connection3<TargetType, Type1, Type2, Type3>(slotHolder,aMemberFunction);
		    BasicSignal2<Type1, Type2>::connectedSlots_.push_back(conn);
		slotHolder->connectTo(this);
	}

	template<class TargetType>
	void connect(TargetType* slotHolder, TargetType& (TargetType::*aMemberFunction)(Type1, Type2, Type3))
	{
		Connection3<TargetType,Type1,Type2,Type3>* conn = new Connection3<TargetType, Type1, Type2, Type3>(slotHolder,aMemberFunction);
			BasicSignal2<Type1, Type2>::connectedSlots_.push_back(conn);
		slotHolder->connectTo(this);
	}

	void submit(Type1 a1, Type2 a2, Type3 a3)
	{
		auto it = BasicSignal2<Type1, Type2>::connectedSlots_.begin();
		auto itNext = it;
		auto itEnd = BasicSignal2<Type1, Type2>::connectedSlots_.end();

		while(it != itEnd)
		{
			itNext = it;
			++itNext;
            (*it)->submit(a1, a2, a3);
            it = itNext;
		}
	}

	void operator()(Type1 a1, Type2 a2, Type3 a3)
	{
		submit(a1,a2,a3);
	}
};

template<class Type1, class Type2, class Type3, class Type4>
class Signal4 : public BasicSignal4<Type1, Type2, Type3, Type4>
{
public:
    typedef typename BasicSignal4<Type1, Type2, Type3, Type4>::ConnectionList ConnectionList;
public:
	Signal4() {}

	Signal4(const Signal4<Type1, Type2, Type3, Type4>& s)
		: BasicSignal4<Type1, Type2, Type3, Type4>(s)
	{}

	template<class TargetType>
	void connect(TargetType* slotHolder, void (TargetType::*aMemberFunction)(Type1,Type2, Type3, Type4))
	{
		Connection4<TargetType, Type1, Type2, Type3, Type4>*
			conn = new Connection4<TargetType, Type1, Type2, Type3,Type4>(slotHolder, aMemberFunction);
		BasicSignal4<Type1, Type2,Type3,Type4>::connectedSlots_.push_back(conn);
		slotHolder->connectTo(this);
	}

	template<class TargetType>
	void connect(TargetType* slotHolder, TargetType& (TargetType::*aMemberFunction)(Type1,Type2, Type3, Type4))
	{
		Connection4<TargetType, Type1, Type2, Type3, Type4>*
		conn = new Connection4<TargetType, Type1, Type2, Type3,
		Type4>(slotHolder, aMemberFunction);
		BasicSignal2<Type1, Type2>::connectedSlots_.push_back(conn);
		slotHolder->connectTo(this);
	}

	void submit(Type1 a1, Type2 a2, Type3 a3, Type4 a4)
	{
		auto it = BasicSignal4<Type1, Type2,Type3, Type4>::connectedSlots_.begin();
		auto itNext = it;
		auto itEnd = BasicSignal4<Type1, Type2,Type3, Type4>::connectedSlots_.end();

		while(it != itEnd)
		{
			itNext = it;
			++itNext;

			(*it)->submit(a1, a2, a3, a4);

			it = itNext;
		}
	}

	void operator()(Type1 a1, Type2 a2, Type3 a3, Type4 a4)
	{
		submit(a1,a2,a3,a4);
	}
};

template<class Type1, class Type2, class Type3, class Type4, class Type5>
class Signal5 : public BasicSignal5<Type1, Type2, Type3, Type4, Type5>
{
public:
    typedef typename BasicSignal5<Type1, Type2, Type3, Type4, Type5>::ConnectionList ConnectionList;
public:
    Signal5()
    {
    }

    Signal5(const Signal5<Type1, Type2, Type3, Type4, Type5>& s): BasicSignal5<Type1, Type2, Type3, Type4, Type5>(s)
    {
    }

    template<class TargetType>
    void connect(TargetType* slotHolder, void (TargetType::*aMemberFunction)(Type1,
        Type2, Type3, Type4, Type5))
    {
        Connection5<TargetType, Type1, Type2, Type3, Type4,Type5>* conn = new Connection5<TargetType, Type1, Type2,
            Type3, Type4, Type5>(slotHolder, aMemberFunction);
        BasicSignal2<Type1, Type2>::connectedSlots_.push_back(conn);
        slotHolder->connectTo(this);
    }

    template<class TargetType>
    void connect(TargetType* slotHolder, TargetType& (TargetType::*aMemberFunction)(Type1,
        Type2, Type3, Type4, Type5))
    {
        Connection5<TargetType, Type1, Type2, Type3, Type4,
            Type5>* conn = new Connection5<TargetType, Type1, Type2,
            Type3, Type4, Type5>(slotHolder, aMemberFunction);
        BasicSignal5<Type1, Type2, Type3, Type4, Type5>::connectedSlots_.push_back(conn);
        slotHolder->connectTo(this);
    }

    void submit(Type1 a1, Type2 a2, Type3 a3, Type4 a4, Type5 a5)
    {
        auto it = BasicSignal5<Type1, Type2, Type3, Type4, Type5>::connectedSlots_.begin();
        auto itNext = it;
        auto itEnd = BasicSignal5<Type1, Type2, Type3, Type4, Type5>::connectedSlots_.end();

        while(it != itEnd)
        {
            itNext = it;
            ++itNext;

            (*it)->submit(a1, a2, a3, a4, a5);

            it = itNext;
        }
    }

    void operator()(Type1 a1, Type2 a2, Type3 a3, Type4 a4, Type5 a5)
    {
        submit(a1,a2,a3,a4,a5);
    }
};

#endif
