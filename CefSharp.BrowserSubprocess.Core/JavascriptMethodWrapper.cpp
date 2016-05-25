// Copyright © 2010-2014 The CefSharp Project. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
#pragma once

#include "Stdafx.h"

#include "JavascriptMethodWrapper.h"
#include "ExceptionUtils.h"
#include "CefAppWrapper.h"

using namespace System;
using namespace CefSharp::Internals;
using namespace System::Diagnostics;

namespace CefSharp
{
	void JavascriptMethodWrapper::Bind()
	{
		auto methodName = StringUtils::ToNative(_javascriptMethod->JavascriptName);
		auto v8Function = CefV8Value::CreateFunction(methodName, _javascriptMethodHandler.get());

		V8Value->SetValue(methodName, v8Function, V8_PROPERTY_ATTRIBUTE_NONE);
	};

	BrowserProcessResponse^ JavascriptMethodWrapper::Execute(array<Object^>^ parameters)
	{
		try
		{
			auto clientChannel = (IClientChannel^)_browserProcess;

			if (clientChannel->State != CommunicationState::Opened)
			{
				auto response = gcnew BrowserProcessResponse();
				response->Success = false;
				response->Result = "";
				response->Message = "CefSharp::JavascriptMethodWrapper::Channel is not opened";
				return response;
			}

			return _browserProcess->CallMethod(_ownerId, _javascriptMethod->JavascriptName, parameters);
		}
		catch (Exception^ ex)
		{
			auto messageString = ExceptionUtils::HandleError("CefSharp::JavascriptMethodWrapper::Execute", ex, parameters);

			auto response = gcnew BrowserProcessResponse();
			response->Success = false;
			response->Result = "";
			response->Message = messageString;
			return response;
		}
		catch (...)
		{
            auto messageString = ExceptionUtils::HandleError("CefSharp::JavascriptMethodWrapper::Execute", parameters);

			auto response = gcnew BrowserProcessResponse();
			response->Success = false;
			response->Result = "";
			response->Message = messageString;
			return response;
		}
	}
}