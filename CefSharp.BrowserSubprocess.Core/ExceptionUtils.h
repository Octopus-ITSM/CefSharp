// Copyright © 2010-2014 The CefSharp Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.

#include "Stdafx.h"
#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

namespace CefSharp
{
    namespace Internals
    {
        public class ExceptionUtils
        {
        public:

            inline static String^ HandleError(String^ method, Exception^ exception, array<Object^>^ parameters)
            {
                try
                {
                    auto messageString = String::Format("{0} error\r\n{1}\r\nStack Trace: {2}\r\nJS Params:{3}", 
                                            method, 
                                            GetExceptionMessage(exception, 0), 
                                            exception->StackTrace, 
                                            String::Join("\r\n", parameters));

                    WriteLogEntry(messageString);

                    return messageString;
                }
                catch (...)
                {
                    return "Error trying to create message string";
                }
            }

            inline static String^ HandleError(String^ method, array<Object^>^ parameters)
            {
                try
                {
                    auto messageString = String::Format("{0} error\r\n{1}", method, String::Join("\r\n", parameters));

                    WriteLogEntry(messageString);

                    return messageString;
                }
                catch (...)
                {
                    return "Error trying to create message string";
                }
            }

        private:

            inline static String^ GetExceptionMessage(Exception^ exception, int level)
            {
                auto exceptionMessage = String::Format("{0}Type: {1} Message: {2}\r\n", gcnew String(' ', level * 3), exception->GetType()->Name, exception->Message);

                if (exception->InnerException != nullptr)
                {
                    exceptionMessage += GetExceptionMessage(exception->InnerException, level + 1);
                }

                return exceptionMessage;
            }

            inline static void WriteLogEntry(String^ messageString)
            {
                try
                {
                    if (!EventLog::SourceExists("CefSharp"))
                    {
                        EventLog::CreateEventSource("CefSharp", "Application");
                    }

                    auto entry = messageString->Length > 31500 ? messageString->Substring(0, 31500) : messageString;
                    EventLog::WriteEntry("CefSharp", entry, EventLogEntryType::Error, 0);
                }
                catch (...)
                {
                    //Cannot afford to crash during logging
                }
            }
        };
    };
}
