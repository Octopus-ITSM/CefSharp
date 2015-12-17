// Copyright © 2010-2014 The CefSharp Project. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
#pragma once

#include "Stdafx.h"

#include "JavascriptMethodWrapper.h"
#include "CefAppWrapper.h"

using namespace System;
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
			return _browserProcess->CallMethod(_ownerId, _javascriptMethod->JavascriptName, parameters);
		}
		catch (...)
		{
			auto messageString = String::Format("CefSharp::JavascriptMethodWrapper::Execute has failed: {0}", String::Join("\r\n", parameters));

			if (!EventLog::SourceExists("CefSharp"))
			{
				EventLog::CreateEventSource("CefSharp", "Application");
			}
			auto entry = messageString->Length > 31500 ? messageString->Substring(0, 31500) : messageString;
			EventLog::WriteEntry("CefSharp", entry, EventLogEntryType::Error, 0);

			auto response = gcnew BrowserProcessResponse();
			response->Success = false;
			response->Result = "";
			response->Message = messageString;
			return response;
		}
	}
}