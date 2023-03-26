//
// Created by firewolf on 09.02.23.
//
#include <coroutine>
#include <thread>
#include <iostream>
using namespace::std;

/*static auto switch_to_new_thread(jthread& out) // async func
{
    struct awaitable
    {
        jthread* p_out;
        bool await_ready() { return false; }
        void await_suspend(coroutine_handle<> h) // changer
        {
            jthread& out = *p_out;
            if (out.joinable())
                throw runtime_error("Output jthread parameter not empty");
            out = jthread([h] { h.resume(); }); // lambda resume
            std::cout << "New thread: " << out.get_id() << '\n'; // this is OK
        }
        void await_resume() {}
    };
    return awaitable{&out}; // send new awaible with new thread
}

struct task // tasker coroutine
{
    struct promise_type
    {
        task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
    struct awaitable
    {
        jthread* p_out;
        bool await_ready() { return false; }
        void await_suspend(coroutine_handle<> h) // changer
        {
            jthread& out = *p_out;
            if (out.joinable())
                throw runtime_error("Output jthread parameter not empty");
            out = jthread([h] { h.resume(); }); // lambda resume
            std::cout << "New thread: " << out.get_id() << '\n'; // this is OK
        }
        void await_resume() {}
    };

};*/

static auto switch_to_new_thread(jthread& out) // async func
{
    struct awaitable
    {
        jthread* p_out;
        bool await_ready() { return false; }
        void await_suspend(coroutine_handle<> h) // changer
        {
            jthread& out = *p_out;
            if (out.joinable())
                throw runtime_error("Output jthread parameter not empty");
            out = jthread([h] { h.resume(); }); // lambda resume
            //std::cout << "New thread: " << out.get_id() << '\n'; // this is OK
        }
        void await_resume() {}
    };
    return awaitable{&out}; // send new awaible with new thread
}

struct task // tasker coroutine
{
    struct promise_type
    {
        task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
    struct awaitable
    {
        jthread* p_out;
        bool await_ready() { return false; }
        void await_suspend(coroutine_handle<> h) // changer
        {
            jthread& out = *p_out;
            if (out.joinable())
                throw runtime_error("Output jthread parameter not empty");
            out = jthread([h] { h.resume(); }); // lambda resume
            std::cout << "New thread: " << out.get_id() << '\n'; // this is OK
        }
        void await_resume() {}
    };

};


/*task test(std::jthread& out) // test func
{
    std::cout << "BEFORE - " << std::this_thread::get_id() << '\n';
    co_await switch_to_new_thread(out); // change thread
    // awaiter destroyed here
    std::cout << "AFTER - " << std::this_thread::get_id() << '\n';
    for(int i = 0; ; i++)
    {
        cout << "123" << endl;
    }
    out.detach;
}*/