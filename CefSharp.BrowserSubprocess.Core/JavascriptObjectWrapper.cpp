// Copyright © 2010-2014 The CefSharp Project. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.
#pragma once

#include "Stdafx.h"

#include "JavascriptObjectWrapper.h"
#include "CefAppWrapper.h"
#include "ExceptionUtils.h"

using namespace System;
using namespace CefSharp::Internals;
using namespace System::Diagnostics;

namespace CefSharp
{
	void JavascriptObjectWrapper::Bind()
	{
		//Create property handler for get and set of Properties of this object
		JsPropertyHandler = new JavascriptPropertyHandler(
			gcnew Func<String^, BrowserProcessResponse^>(this, &JavascriptObjectWrapper::GetProperty),
			gcnew Func<String^, Object^, BrowserProcessResponse^>(this, &JavascriptObjectWrapper::SetProperty)
			);

		//V8Value that represents this javascript object - only one per complex type
		auto javascriptObject = V8Value->CreateObject(JsPropertyHandler.get());

		auto objectName = StringUtils::ToNative(_object->JavascriptName);
		V8Value->SetValue(objectName, javascriptObject, V8_PROPERTY_ATTRIBUTE_NONE);

		for each (JavascriptMethod^ method in Enumerable::OfType<JavascriptMethod^>(_object->Methods))
		{
			auto wrappedMethod = gcnew JavascriptMethodWrapper(method, _object->Id, _browserProcess);
			wrappedMethod->V8Value = javascriptObject;
			wrappedMethod->Bind();

			_wrappedMethods->Add(wrappedMethod);
		}

		for each (JavascriptProperty^ prop in Enumerable::OfType<JavascriptProperty^>(_object->Properties))
		{
			auto wrappedproperty = gcnew JavascriptPropertyWrapper(prop, _object->Id, _browserProcess);
			wrappedproperty->V8Value = javascriptObject;
			wrappedproperty->Bind();

			_wrappedProperties->Add(wrappedproperty);
		}
	}

	BrowserProcessResponse^ JavascriptObjectWrapper::GetProperty(String^ memberName)
	{
		try
		{
			return _browserProcess->GetProperty(_object->Id, memberName);
		}
        catch (Exception^ ex)
        {
            auto parameters = gcnew array<Object^>(1);
            parameters[0] = memberName;

            auto messageString = ExceptionUtils::HandleError("CefSharp::JavascriptObjectWrapper::GetProperty", ex, parameters);

            auto response = gcnew BrowserProcessResponse();
            response->Success = false;
            response->Result = "";
            response->Message = messageString;
            return response;
        }
        catch (...)
        {
            auto parameters = gcnew array<Object^>(1);
            parameters[0] = memberName;

            auto messageString = ExceptionUtils::HandleError("CefSharp::JavascriptObjectWrapper::GetProperty", parameters);

            auto response = gcnew BrowserProcessResponse();
            response->Success = false;
            response->Result = "";
            response->Message = messageString;
            return response;
        }
	};

	BrowserProcessResponse^ JavascriptObjectWrapper::SetProperty(String^ memberName, Object^ value)
	{
		try
		{
			return _browserProcess->SetProperty(_object->Id, memberName, value);
		}
        catch (Exception^ ex)
        {
            auto parameters = gcnew array<Object^>(2);
            parameters[0] = memberName;
            parameters[1] = value;

            auto messageString = ExceptionUtils::HandleError("CefSharp::JavascriptObjectWrapper::SetProperty", ex, parameters);

            auto response = gcnew BrowserProcessResponse();
            response->Success = false;
            response->Result = "";
            response->Message = messageString;
            return response;
        }
        catch (...)
        {
            auto parameters = gcnew array<Object^>(2);
            parameters[0] = memberName;
            parameters[1] = value;

            auto messageString = ExceptionUtils::HandleError("CefSharp::JavascriptObjectWrapper::SetProperty", parameters);

            auto response = gcnew BrowserProcessResponse();
            response->Success = false;
            response->Result = "";
            response->Message = messageString;
            return response;
        }
	};
}
