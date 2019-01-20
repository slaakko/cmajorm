using System;
using System.Windows.Forms;
using System.Drawing;
using System.Runtime.InteropServices;

namespace cmdevenv
{
    public class XTabControl : System.Windows.Forms.TabControl
    {
        public XTabControl()
            : base()
        {
            TabStop = false;
            scroller.ScrollLeft += new EventHandler(scroller_ScrollLeft);
            scroller.ScrollRight += new EventHandler(scroller_ScrollRight);
            scroller.TabClose += new EventHandler(scroller_TabClose);
            scroller.Hide();
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (UpDown != null)
                {
                    UpDown.ReleaseHandle();
                }
            }
            base.Dispose(disposing);
        }

        [DllImport("user32", CallingConvention = CallingConvention.Cdecl)]
        private static extern int RealGetWindowClass(IntPtr hwnd, System.Text.StringBuilder pszType, int cchType);

        [DllImport("user32")]
        private static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [DllImport("user32.dll")]
        private static extern IntPtr SendMessage(IntPtr hWnd, int Msg, IntPtr wParam, IntPtr lParam);

        private const int WM_CREATE = 0x1;
        private const int WM_PARENTNOTIFY = 0x210;
        private const int WM_HSCROLL = 0x114;
        private NativeUpDown UpDown = null;
        private TabScroller scroller = new TabScroller();

        private int ScrollPosition
        {
            get
            {
                int multiplier = -1;
                Rectangle tabRect;
                do
                {
                    tabRect = GetTabRect(multiplier + 1);
                    multiplier++;
                }
                while (tabRect.Left < 0 && multiplier < TabCount);
                return multiplier;
            }
        }
/*
        protected override void WndProc(ref System.Windows.Forms.Message m)
        {
            if (m.Msg == WM_PARENTNOTIFY)
            {
                if ((ushort)(m.WParam.ToInt32() & 0xFFFF) == WM_CREATE)
                {
                    System.Text.StringBuilder WindowName = new System.Text.StringBuilder(16);
                    RealGetWindowClass(m.LParam, WindowName, 16);
                    if (WindowName.ToString() == "msctls_updown32")
                    {
                        if (UpDown != null)
                        {
                            UpDown.ReleaseHandle();
                        }
                        UpDown = new NativeUpDown();
                        UpDown.AssignHandle(m.LParam);
                    }
                }
            }
            base.WndProc(ref m);
        }
*/
        protected override void OnHandleCreated(System.EventArgs e)
        {
            base.OnHandleCreated(e);
            if (!Multiline)
            {
                scroller.Font = new Font("Marlett", Font.Size, FontStyle.Regular, GraphicsUnit.Pixel, Font.GdiCharSet);
                SetParent(scroller.Handle, Handle);
            }
            OnFontChanged(EventArgs.Empty);
        }

        protected override void OnFontChanged(System.EventArgs e)
        {
            base.OnFontChanged(e);
            scroller.Font = new Font("Marlett", Font.SizeInPoints, FontStyle.Regular, GraphicsUnit.Point);
            scroller.Height = ItemSize.Height;
            scroller.Width = ItemSize.Height * 3;
            OnResize(EventArgs.Empty);
        }

        public void ShowHideScrollers()
        {
            int tabWidths = 0;
            int n = TabPages.Count;
            for (int i = 0; i < n; ++i)
            {
                tabWidths += GetTabRect(i).Width;
            }
            if (tabWidths + scroller.Width > Width)
            {
                scroller.ShowScrollers();
            }
            else
            {
                scroller.HideScrollers();
            }
        }

        protected override void OnResize(System.EventArgs e)
        {
            if (Multiline)
            {
                return;
            }
            base.OnResize(e);
            ShowHideScrollers();
            Invalidate(true);
            if (Alignment == TabAlignment.Top)
            {
                scroller.Location = new Point(Width - scroller.Width, 2);
            }
            else
            {
                scroller.Location = new Point(Width - scroller.Width, Height - scroller.Height - 2);
            }
        }

        protected override void OnControlAdded(ControlEventArgs e)
        {
            base.OnControlAdded(e);
            if (TabPages.Count > 0)
            {
                scroller.Show();
            }
            else
            {
                scroller.Hide();
            }
        }

        protected override void OnControlRemoved(ControlEventArgs e)
        {
            base.OnControlRemoved(e);
            if (TabPages.Count <= 1)
            {
                scroller.Hide();
            }
        }

        private void scroller_ScrollLeft(Object sender, System.EventArgs e)
        {
            if (TabCount > 0)
            {
                int scrollPos = Math.Max(0, (ScrollPosition - 1) * 0x10000);
                SendMessage(Handle, WM_HSCROLL, (IntPtr)(scrollPos | 0x4), IntPtr.Zero);
                SendMessage(Handle, WM_HSCROLL, (IntPtr)(scrollPos | 0x8), IntPtr.Zero);
            }
        }


        private void scroller_ScrollRight(Object sender, System.EventArgs e)
        {
            if (TabCount > 0 || GetTabRect(TabCount - 1).Right <= scroller.Left)
            {
                int scrollPos = Math.Max(0, (ScrollPosition + 1) * 0x10000);
                SendMessage(Handle, WM_HSCROLL, (IntPtr)(scrollPos | 0x4), IntPtr.Zero);
                SendMessage(Handle, WM_HSCROLL, (IntPtr)(scrollPos | 0x8), IntPtr.Zero);
            }
        }

        public delegate bool TabClosingEventHandler(EventArgs e);

        public event TabClosingEventHandler TabClosing;

        private void scroller_TabClose(Object sender, System.EventArgs e)
        {
            if (TabClosing != null)
            {
                if (!TabClosing(new EventArgs()))
                {
                    return;
                }
            }
            if (SelectedTab != null)
            {
                TabPages.Remove(SelectedTab);
                ShowHideScrollers();
            }
            if (TabPages.Count == 0)
            {
                scroller.Hide();
            }
        }
    }

    internal class TabScroller : System.Windows.Forms.Control
    {
        public TabScroller()
            : base()
        {
            InitializeComponent();
            scrollersVisible = true;
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        private System.ComponentModel.IContainer components = null;

        internal System.Windows.Forms.Button leftScroller;
        internal System.Windows.Forms.Button rightScroller;
        internal System.Windows.Forms.Button closeButton;

        [System.Diagnostics.DebuggerStepThrough()]
        private void InitializeComponent()
        {
            this.leftScroller = new System.Windows.Forms.Button();
            this.rightScroller = new System.Windows.Forms.Button();
            this.closeButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            //
            //leftScroller
            //
            this.leftScroller.Dock = System.Windows.Forms.DockStyle.Right;
            this.leftScroller.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.leftScroller.Location = new System.Drawing.Point(0, 0);
            this.leftScroller.Name = "leftScroller";
            this.leftScroller.Size = new System.Drawing.Size(40, 40);
            this.leftScroller.TabIndex = 0;
            this.leftScroller.Text = "3";
            this.leftScroller.Click += new EventHandler(LeftScroller_Click);
            //
            //rightScroller
            //
            this.rightScroller.Dock = System.Windows.Forms.DockStyle.Right;
            this.rightScroller.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.rightScroller.Location = new System.Drawing.Point(40, 0);
            this.rightScroller.Name = "rightScroller";
            this.rightScroller.Size = new System.Drawing.Size(40, 40);
            this.rightScroller.TabIndex = 1;
            this.rightScroller.Text = "4";
            this.rightScroller.Click += new EventHandler(RightScroller_Click);
            //
            //closeButton
            //
            this.closeButton.Dock = System.Windows.Forms.DockStyle.Right;
            this.closeButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.closeButton.Location = new System.Drawing.Point(80, 0);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(40, 40);
            this.closeButton.TabIndex = 2;
            this.closeButton.Text = "r";
            this.closeButton.Click += new EventHandler(CloseButton_Click);
            //
            //TabScroller
            //
            this.Controls.Add(this.leftScroller);
            this.Controls.Add(this.rightScroller);
            this.Controls.Add(this.closeButton);
            this.Font = new System.Drawing.Font("Marlett", 8.25f, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, (byte)2);
            this.Name = "TabScroller";
            this.Size = new System.Drawing.Size(120, 40);
            this.Resize += new EventHandler(TabScroller_Resize);
            this.ResumeLayout(false);
        }

        public void HideScrollers()
        {
            if (scrollersVisible)
            {
                leftScroller.Hide();
                rightScroller.Hide();
                scrollersVisible = false;
                Invalidate();
            }
        }

        public void ShowScrollers()
        {
            if (!scrollersVisible)
            {
                leftScroller.Show();
                rightScroller.Show();
                scrollersVisible = true;
                Invalidate();
            }
        }

        private bool scrollersVisible;
        public event EventHandler TabClose;
        public event EventHandler ScrollLeft;
        public event EventHandler ScrollRight;

        private void TabScroller_Resize(Object sender, System.EventArgs e)
        {
            leftScroller.Width = Width / 3;
            rightScroller.Width = Width / 3;
            closeButton.Width = Width / 3;
        }

        private void LeftScroller_Click(Object sender, System.EventArgs e)
        {
            if (ScrollLeft != null)
            {
                ScrollLeft(this, EventArgs.Empty);
            }
        }

        private void RightScroller_Click(Object sender, System.EventArgs e)
        {
            if (ScrollRight != null)
            {
                ScrollRight(this, EventArgs.Empty);
            }
        }

        private void CloseButton_Click(Object sender, System.EventArgs e)
        {
            if (TabClose != null)
            {
                TabClose(this, EventArgs.Empty);
            }
        }
    }

    internal class NativeUpDown : NativeWindow
    {
        public NativeUpDown() : base() { }

        private const int WM_DESTROY = 0x2;
        private const int WM_NCDESTROY = 0x82;
        private const int WM_WINDOWPOSCHANGING = 0x46;

        [StructLayout(LayoutKind.Sequential)]
        private struct WINDOWPOS
        {
            public IntPtr hwnd, hwndInsertAfter;
            public int x, y, cx, cy, flags;
        }

        protected override void WndProc(ref System.Windows.Forms.Message m)
        {
            if (m.Msg == WM_DESTROY || m.Msg == WM_NCDESTROY)
            {
                ReleaseHandle();
            }
            else if (m.Msg == WM_WINDOWPOSCHANGING)
            {
                //Move the updown control off the edge so it's not visible
                WINDOWPOS wp = (WINDOWPOS)(m.GetLParam(typeof(WINDOWPOS)));
                wp.x += wp.cx;
                Marshal.StructureToPtr(wp, m.LParam, true);
                bounds = new Rectangle(wp.x, wp.y, wp.cx, wp.cy);
            }
            base.WndProc(ref m);
        }

        private Rectangle bounds;
        internal Rectangle Bounds
        {
            get { return bounds; }
        }
    }
}
