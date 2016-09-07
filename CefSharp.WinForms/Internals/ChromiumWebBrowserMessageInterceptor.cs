﻿using CefSharp.Internals;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Windows.Forms;

namespace CefSharp.WinForms.Internals
{
    public class ChromiumWebBrowserMessageInterceptor : NativeWindow, IDisposable
    {
        private ChromiumWebBrowser _browser;
        private Action<Message> _onMessageCallback;

        public ChromiumWebBrowserMessageInterceptor(ChromiumWebBrowser browser, Action<Message> onMessageCallback)
        {
            _browser = browser;
            _onMessageCallback = onMessageCallback;

            if (_browser.IsBrowserInitialized)
            {
                AssignHandle();
            }
            else
            {
                _browser.IsBrowserInitializedChanged += _browser_IsBrowserInitializedChanged;
            }
        }

        private void AssignHandle()
        {
            List<IntPtr> childWindows = new List<IntPtr>();
            User32.GetSubWindows(_browser.Handle, childWindows);
            var cefWindowHandle = childWindows.Last();
            AssignHandle(cefWindowHandle);
        }

        private void _browser_IsBrowserInitializedChanged(object sender, IsBrowserInitializedChangedEventArgs e)
        {            
            try
            {
                _browser.IsBrowserInitializedChanged -= _browser_IsBrowserInitializedChanged;
                _browser.Invoke(new MethodInvoker(AssignHandle));
            }
            catch (TargetInvocationException ex)
            {
                if (ex.InnerException is ObjectDisposedException)
                {
                }
                else
                {
                    throw;
                }
            }
            catch (ObjectDisposedException)
            {
            }
            catch
            {
               if( _browser == null )
               {
                  return;
               }
               
               throw;
            }
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);
            _onMessageCallback(m);
        }

        public void Dispose()
        {
            _browser.IsBrowserInitializedChanged -= _browser_IsBrowserInitializedChanged;

            ReleaseHandle();
            _browser = null;
            _onMessageCallback = null;
        }
    }
}