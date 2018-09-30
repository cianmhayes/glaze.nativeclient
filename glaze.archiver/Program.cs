using System;
using System.Net.Sockets;
using System.Net;
using System.Drawing;
using System.IO;

namespace glaze.archiver
{
    class Program
    {
        static void Main(string[] args)
        {
            int minTargetDimension = 1200;
            UdpClient receiver = new UdpClient(50001);
            IPEndPoint broadcast = new IPEndPoint(IPAddress.Broadcast, 50001);

            DateTime now = DateTime.Now;
            string directory = Directory.CreateDirectory(String.Format("./Steam_{0:yyyyMMdd}_{0:HHmm}/", now)).FullName;

            while(true)
            {
                byte[] packet = receiver.Receive(ref broadcast);

                // header
                // payload size : uint32 : 4
                // frameNumber : uint 32 : 4
                // frameType : uint8 : 1
                // bytes per pixel : uint8 : 1
                // width : uint32 : 4
                // height: uint32 : 4
                // frameRateNumerator : unint32 : 4
                // frameRateDenominator: uint32 : 4
                //
                // datagram payload length = 26 + frame payload

                UInt32 frameNumber = BitConverter.ToUInt32(new byte[] { packet[7], packet[6], packet[5], packet[4] }, 0);
                byte frameType = packet[8];
                byte bytesPerPixel = packet[9];
                UInt32 frameWidth = BitConverter.ToUInt32(new byte[] { packet[13], packet[12], packet[11], packet[10] }, 0);
                UInt32 frameHeight = BitConverter.ToUInt32(new byte[] { packet[17], packet[16], packet[15], packet[14] }, 0);
                
                UInt32 cellSize = (uint)Math.Ceiling(((double)minTargetDimension) / Math.Min(frameWidth, frameHeight));
                UInt32 targetWidth = frameWidth * cellSize;
                UInt32 targetHeight = frameHeight * cellSize;

                Bitmap bmp = new Bitmap((int)targetWidth, (int)targetHeight);
                Graphics g = Graphics.FromImage(bmp);
                int pixelDataOffset = 26;
                for (int y = 0; y < frameHeight; y++)
                {
                    for (int x = 0; x < frameWidth; x++)
                    {
                        byte sourceByte = packet[pixelDataOffset + (y * frameWidth) + x];
                        Color c;
                        switch(frameType)
                        {
                            default:
                                c = Color.FromArgb(sourceByte, sourceByte, sourceByte);
                                break;
                        }

                        SolidBrush brush = new SolidBrush(c);
                        Rectangle r = new Rectangle((int)(x * cellSize), (int)(y * cellSize), (int)cellSize, (int)cellSize);
                        g.FillRectangle(brush, r);
                    }
                }

                bmp.Save(Path.Combine(directory, $"frame_{frameNumber}.bmp"));
            }
        }
    }
}
