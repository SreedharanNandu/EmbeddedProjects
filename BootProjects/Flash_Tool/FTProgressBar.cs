using System;
using System.Drawing;
using System.Windows.Forms;

namespace FlashTool
{
    // A progress bar that is drawn by the program itself: the fill color can be chosen (the normal Windows
    // progress bar is always green when visual styles are on and ignores ForeColor), it can be drawn as
    // separate blocks ("bricks") and there is no fill animation, so the bar shows the real value at once.
    //
    // To change the look, set the properties in FTMain.Designer.cs (or in the FTMain constructor), e.g.
    //     this.ProgressBar.BarColor = System.Drawing.Color.Orange;
    //     this.ProgressBar.Segmented = false;                 // one solid bar
    public class FTProgressBar : ProgressBar
    {
        private Color barColor = Color.FromArgb(0, 120, 215);       // Windows blue (default Windows 10 accent color)
        private Color trackColor = Color.FromArgb(230, 230, 230);
        private Color borderColor = Color.FromArgb(188, 188, 188);
        private bool segmented = true;
        private int segmentWidth = 12;                                 // wanted width of one brick in pixels
        private int segmentGap = 3;                                    // gap between the bricks in pixels

        // color of the fill; for the accent color the user has chosen in Windows use SystemColors.Highlight
        public Color BarColor { get { return barColor; } set { barColor = value; Invalidate(); } }
        public Color TrackColor { get { return trackColor; } set { trackColor = value; Invalidate(); } }
        public Color BorderColor { get { return borderColor; } set { borderColor = value; Invalidate(); } }

        // true: separate bricks, false: one solid bar
        public bool Segmented { get { return segmented; } set { segmented = value; Invalidate(); } }
        public int SegmentWidth { get { return segmentWidth; } set { segmentWidth = Math.Max(1, value); Invalidate(); } }
        public int SegmentGap { get { return segmentGap; } set { segmentGap = Math.Max(0, value); Invalidate(); } }

        public FTProgressBar()
        {
            SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer, true);
        }

        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
            // everything is drawn in OnPaint
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Rectangle r = ClientRectangle;
            if (r.Width < 5 || r.Height < 5) return;

            using (SolidBrush track = new SolidBrush(trackColor))
                e.Graphics.FillRectangle(track, r);

            int range = Maximum - Minimum;
            if (range > 0 && Value > Minimum)
            {
                int inner = r.Width - 4;                                // 1 px border + 1 px space on each side
                int height = r.Height - 4;
                double fraction = (double)(Value - Minimum) / range;

                using (SolidBrush fill = new SolidBrush(barColor))
                {
                    if (!segmented)
                    {
                        int width = (int)(fraction * inner);
                        if (width > 0) e.Graphics.FillRectangle(fill, 2, 2, width, height);
                    }
                    else
                    {
                        // the bricks are made a little wider or narrower so that they fill the bar exactly
                        int total = Math.Max(1, (inner + segmentGap) / (segmentWidth + segmentGap));
                        float brick = (inner - (float)segmentGap * (total - 1)) / total;
                        int lit = (int)Math.Round(fraction * total);
                        if (Value >= Maximum) lit = total;
                        for (int i = 0; i < lit; i++)
                        {
                            float x = 2 + i * (brick + segmentGap);
                            e.Graphics.FillRectangle(fill, x, 2, brick, height);
                        }
                    }
                }
            }

            using (Pen border = new Pen(borderColor))
                e.Graphics.DrawRectangle(border, 0, 0, r.Width - 1, r.Height - 1);
        }
    }
}
