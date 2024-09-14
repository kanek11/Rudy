#pragma once
#include "RudyPCH.h"

#include <functional>
#include <vector>
#include <queue>

/*
options:

use std::function to be type safe;
use vector if the deletion is rare;
map if the deletion is frequent; i.e. dynamic ;

instead of invoke immediately;
use eventBus to manage the events,

*/

/*
member function has a implicit parameter, this;
the best solution for now is to use generic lambda to wrap it;
*/

#define BIND_MEM_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Rudy
{

// forward declaration;
template <typename Signature>
class FDelegate;

template <typename Signature>
class FQueueDelegate;

// partial specialization , accepts a function signature in the form of Ret(Args...)
// this design pattern requires declaration of the type;
template <typename Ret, typename... Args>
class FDelegate<Ret(Args...)>
{
public:
    using FunctionType = std::function<Ret(Args...)>;

    // register, bind, add,  Connect
    void Add(FunctionType func)
    {
        _callBackFn.push_back(std::move(func));
    }

    // dispatch, execute, broadcast, Emit
    template <typename... Args>
    void BlockingBroadCast(Args&&... args)
    {
        for (auto& func : _callBackFn)
        {
            if (func)
            {
                func(std::forward<Args>(args)...);
            }
        }
    }

private:
    std::vector<FunctionType> _callBackFn; // slot
};

template <typename Ret, typename... Args>
class FQueueDelegate<Ret(Args...)> : public FDelegate<Ret(Args...)>
{
public:
    using FunctionType = std::function<Ret(Args...)>;

    template <typename... Args>
    void PostBuffer(Args&&... args)
    {
        _eventQueue.emplace(std::make_tuple(std::forward<Args>(args)...));
    }

    void DispatchBuffer()
    {
        while (!_eventQueue.empty())
        {
            auto& event = _eventQueue.front();
            std::apply([this](Args... args)
                       { this->BlockingBroadCast(std::forward<decltype(args)>(args)...); },
                       event);
            _eventQueue.pop();
        }
    }

private:
    std::queue<std::tuple<Args...>> _eventQueue;
};
} // namespace Rudy
