// Copyright © 2010-2014 The CefSharp Project. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
#pragma once

#include "Stdafx.h"

#include <msclr/lock.h>

#include "CefAppWrapper.h"
#include "CefBrowserWrapper.h"
#include "CefAppUnmanagedWrapper.h"
#include "ExceptionUtils.h"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;

namespace CefSharp
{
    CefRefPtr<CefRenderProcessHandler> CefAppUnmanagedWrapper::GetRenderProcessHandler()
    {
        return this;
    };

    void CefAppUnmanagedWrapper::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
    {
        try
        {
            auto wrapper = gcnew CefBrowserWrapper(browser);
            _onBrowserCreated->Invoke(wrapper);
            Add(wrapper);
        }
        catch (Exception^ exception)
        {
            auto parameters = gcnew array<Object^>(1);
            parameters[0] = String::Format("BrowserId:{0}", browser->GetIdentifier());
            ExceptionUtils::HandleError("CefSharp::CefAppUnmanagedWrapper::OnBrowserCreated", exception, parameters);
        }
    }

    void CefAppUnmanagedWrapper::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
    {
        try
        {
            CefBrowserWrapper^ wrapper = Remove(browser);
            _onBrowserDestroyed->Invoke(wrapper);
            delete wrapper;
        }
        catch (Exception^ exception)
        {
            auto parameters = gcnew array<Object^>(1);
            parameters[0] = String::Format("BrowserId:{0}", browser->GetIdentifier());
            ExceptionUtils::HandleError("CefSharp::CefAppUnmanagedWrapper::OnBrowserDestroyed", exception, parameters);
        }
    };

    void CefAppUnmanagedWrapper::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
    {
        try
        {
            auto wrapper = Find(browser);

            if (wrapper->JavascriptRootObject != nullptr)
            {
                auto window = context->GetGlobal();

                wrapper->JavascriptRootObjectWrapper = gcnew JavascriptRootObjectWrapper(wrapper->JavascriptRootObject, wrapper->BrowserProcess);

                wrapper->JavascriptRootObjectWrapper->V8Value = window;
                wrapper->JavascriptRootObjectWrapper->Bind();
            }
        }
        catch (Exception^ exception)
        {
            auto parameters = gcnew array<Object^>(1);
            parameters[0] = String::Format("BrowserId:{0}", browser->GetIdentifier());
            ExceptionUtils::HandleError("CefSharp::CefAppUnmanagedWrapper::OnContextCreated", exception, parameters);
        }
    };

    void CefAppUnmanagedWrapper::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
    {
        try
        {
            auto wrapper = Find(browser);

            if (wrapper->JavascriptRootObjectWrapper != nullptr)
            {
                delete wrapper->JavascriptRootObjectWrapper;
                wrapper->JavascriptRootObjectWrapper = nullptr;
            }
        }
        catch (Exception^ exception)
        {
            auto parameters = gcnew array<Object^>(1);
            parameters[0] = String::Format("BrowserId:{0}", browser->GetIdentifier());
            ExceptionUtils::HandleError("CefSharp::CefAppUnmanagedWrapper::OnContextReleased", exception, parameters);
        }
    };

    void CefAppUnmanagedWrapper::Add(CefBrowserWrapper^ browser)
    {
        msclr::lock l(_sync);

        _browserWrappers->Add(browser);
    };

    CefBrowserWrapper^ CefAppUnmanagedWrapper::Remove(CefRefPtr<CefBrowser> browser)
    {
        msclr::lock l(_sync);

        for each (CefBrowserWrapper^ var in static_cast<List<CefBrowserWrapper^>^>(_browserWrappers))
        {
            if (var->GetCefBrowser()->IsSame(browser))
            {
                _browserWrappers->Remove(var);
                return var;
            }
        }

        throw gcnew InvalidOperationException("Failed to identify BrowserWrapper");
    };

    CefBrowserWrapper^ CefAppUnmanagedWrapper::Find(CefRefPtr<CefBrowser> browser)
    {
        msclr::lock l(_sync);

        for each (CefBrowserWrapper^ var in static_cast<List<CefBrowserWrapper^>^>(_browserWrappers))
        {
            if (var->GetCefBrowser()->IsSame(browser))
            {
                return var;
            }
        }

        throw gcnew InvalidOperationException("Failed to identify BrowserWrapper");
    };
}