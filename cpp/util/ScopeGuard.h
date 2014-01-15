#ifndef SCOPEGUARD_H_
#define SCOPEGUARD_H_

#include <functional>
#include <windows.h>

/*
Usage:

    HANDLE h = 0;       //CreateFile(...);
    ScopeGuard onExit([&] { 
        CloseHandle(h);
        MessageBox(NULL, _T("sss"), _T("sss"), 0);
    });
    onExit.Dismiss();

    ON_SCOPE_EXIT([&]{
        
        MessageBox(NULL, _T("hello"), _T("hello"), 0);
    });
*/

class ScopeGuard
{
public:
    explicit ScopeGuard(std::function<void()> onExitScope)
        : onExitScope_(onExitScope), dismissed_(false)
    { }

    ~ScopeGuard()
    {
        if(!dismissed_)
        {
            onExitScope_();
        }
    }

    void Dismiss()
    {
        dismissed_ = true;
    }

private:
    std::function<void()> onExitScope_;
    bool dismissed_;

private: // noncopyable
    ScopeGuard(ScopeGuard const&);
    ScopeGuard& operator=(ScopeGuard const&);
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)


#endif