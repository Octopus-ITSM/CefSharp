using CefSharp.Internals;
using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Reflection;

namespace CefSharp.WinForms.Internals
{

    public class ChromiumWebBrowserMessageInterceptor : NativeWindow, IDisposable
    {
        private ChromiumWebBrowser _browser;

        public ChromiumWebBrowserMessageInterceptor(ChromiumWebBrowser browser)
        {
            _browser = browser;
            
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
            _browser.Invoke( new MethodInvoker(()=>
            {
                List<IntPtr> childWindows = new List<IntPtr>();
                User32.GetSubWindows(_browser.Handle, childWindows);
                var lastChildWindow = childWindows[1];
                AssignHandle(lastChildWindow);
            }));
        }

        private void _browser_IsBrowserInitializedChanged(object sender, IsBrowserInitializedChangedEventArgs e)
        {
            _browser.IsBrowserInitializedChanged -= _browser_IsBrowserInitializedChanged;
            AssignHandle();
        }

        private static int WM_SETFOCUS = 0x0007;
        private static int WM_KILLFOCUS = 0x0008;

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);

            if (m.Msg == WM_SETFOCUS)
            {
                Activate(_browser);
            }
            else if (m.Msg == WM_KILLFOCUS)
            {
                int a = 1;
            }
        }

        public void Dispose()
        {
            ReleaseHandle();
            _browser = null;
        }

        private static void Activate(Control control)
        {
            //Notify WinForms world that inner browser window got focus. This will trigger Leave event to previous focused control
            var containerControl = control.GetContainerControl();

            if (containerControl != null)
            {
                containerControl.ActivateControl(control);
            }
        }
    }

	internal class DefaultFocusHandler : IFocusHandler
	{
		private readonly ChromiumWebBrowser _browser;
        private readonly ChromiumWebBrowserMessageInterceptor _messageInterceptor;

		public DefaultFocusHandler(ChromiumWebBrowser browser)
		{
			_browser = browser;
            _messageInterceptor = new ChromiumWebBrowserMessageInterceptor(_browser);
		}

		public void OnGotFocus()
		{
        }

		public bool OnSetFocus(CefFocusSource source)
		{
            if (source == CefFocusSource.FocusSourceNavigation)
            {
                return true; //Do not let the browser take focus when a Load method has been called
            }
            else
            {
                return false;
            }
		}

		public void OnTakeFocus(bool next)
		{
            _browser.BeginInvoke(new MethodInvoker(() => SelectNextControl(_browser, next)));
		}

        private static void SelectNextControl(Control control, bool next)
        {
            var containerControl = control.GetContainerControl();

            while (containerControl != null)
            {
                var containerControlAsControl = containerControl as Control;
                if (containerControlAsControl == null)
                {
                    break;
                }

                var activeControl = containerControl.ActiveControl;
                if (containerControlAsControl.SelectNextControl(activeControl, next, true, true, false))
                {
                    break;
                }

                if (containerControlAsControl.Parent == null)
                {
                    break;
                }

                containerControl = containerControlAsControl.Parent.GetContainerControl();
            }
        }
	}
}
