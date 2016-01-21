// Copyright © 2010-2013 The CefSharp Project. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.

#pragma once


#include "Stdafx.h"
#include "include/cef_app.h"
#include "include/cef_base.h"

#include "CefBrowserWrapper.h"
#include "CefAppWrapper.h"

using namespace System::Collections::Generic;
using namespace System::Collections::Concurrent;

namespace CefSharp
{
    private class CefAppUnmanagedWrapper : CefApp, CefRenderProcessHandler
    {
    private:
        gcroot<Action<CefBrowserWrapper^>^> _onBrowserCreated;
        gcroot<Action<CefBrowserWrapper^>^> _onBrowserDestroyed;
        gcroot<List<CefBrowserWrapper^>^> _browserWrappers;
        gcroot<Object^> _sync;

        CefBrowserWrapper^ Find(CefRefPtr<CefBrowser> browser);
        void Add(CefBrowserWrapper^ browser);
        CefBrowserWrapper^ Remove(CefRefPtr<CefBrowser> browser);

    public:
        
        CefAppUnmanagedWrapper(Action<CefBrowserWrapper^>^ onBrowserCreated, Action<CefBrowserWrapper^>^ onBrowserDestoryed)
        {
            _onBrowserCreated = onBrowserCreated;
            _onBrowserDestroyed = onBrowserDestoryed;
            _browserWrappers = gcnew List<CefBrowserWrapper^>();
            _sync = gcnew Object();
        }

        ~CefAppUnmanagedWrapper()
        {
            delete _browserWrappers;
            delete _onBrowserCreated;
            delete _onBrowserDestroyed;
            delete _sync;
        }

        virtual DECL CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE;
        virtual DECL void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
        virtual DECL void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
        virtual DECL void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
        virtual DECL void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;

        IMPLEMENT_REFCOUNTING(CefAppUnmanagedWrapper);
    };
}
