using System;
using System.Text;
using System.Threading;
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;
using System.Net;
using System.Collections.Generic;

namespace _17_606
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        private SerialPort sp1 = new SerialPort();
        private SerialPort sp2 = new SerialPort();
        private int timercount = 0;
        private string currentDate, screwBatchNo = "", workBatchNo = "";
        delegate void Display(byte[] buffer);

        private void Form1_Load(object sender, EventArgs e)
        {
            //取得現在日期時間
            DateTime dt = new DateTime();
            dt = DateTime.Now;
            //把年月日當作檔案名稱
            currentDate = dt.ToString("yyyyMMdd");
            try
            {
                sp1.BaudRate = 115200;
                sp1.DataBits = 8;
                sp1.Parity = Parity.Even;
                sp1.StopBits = StopBits.One;
                sp1.PortName = "COM1";
                sp1.DataReceived += Sp1_DataReceived;
                sp1.Open();

                sp2.BaudRate = 9600;
                sp2.DataBits = 8;
                sp2.Parity = Parity.None;
                sp2.StopBits = StopBits.One;
                sp2.PortName = "COM2";
                sp2.DataReceived += Sp2_DataReceived;
                sp2.Open();

                timer1.Enabled = true;
            }
            catch (Exception error)
            {
                tbErrorMessage.Text = error.ToString();
            }
        }

        private void Sp1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte[] buffer = new byte[4096];
            Thread.Sleep(500);
            Int32 length = (sender as SerialPort).Read(buffer, 0, buffer.Length);
            Array.Resize(ref buffer, length);
            Display d = new Display(DisplayTextHand);
            this.Invoke(d, new Object[] { buffer });
        }

        private void Sp2_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte[] buffer = new byte[4096];
            Thread.Sleep(500);
            Int32 length = (sender as SerialPort).Read(buffer, 0, buffer.Length);
            Array.Resize(ref buffer, length);
            Display d = new Display(DisplayTextFix);
            this.Invoke(d, new Object[] { buffer });
        }

        private void DisplayTextHand(byte[] buffer)
        {
            char c1;
            string s1 = "";
            foreach (byte number in buffer)
            {
                c1 = Convert.ToChar(number);
                s1 = s1 + c1.ToString();
            }
            tbReceiveDataHand.Clear();
            tbReceiveDataHand.Text = s1;
            screwBatchNo = tbReceiveDataHand.Text;
        }

        private void DisplayTextFix(byte[] buffer)
        {
            char c1;
            string s1 = "";
            foreach (byte number in buffer)
            {
                c1 = Convert.ToChar(number);
                s1 = s1 + c1.ToString();
            }
            tbReceiveFix.Clear();
            tbReceiveFix.Text = s1;
            screwBatchNo = tbReceiveFix.Text;
            fx3uPLC.Open();
            int i = 1;
            fx3uPLC.WriteDeviceRandom("D153", 1, ref i);
            fx3uPLC.Close();
        }

        private void pbTrigFix_Click(object sender, EventArgs e)
        {
            byte[] writeData = new byte[3];
            //帝商條碼機觸發讀取命令
            writeData[0] = 0x1B;//ESC
            writeData[1] = 0x5A;//Z
            writeData[2] = 0x0D;//CR
            sp2.Write(writeData, 0, 3);
        }

        private void pbTimer1_Click(object sender, EventArgs e)
        {
            timer1.Enabled = !timer1.Enabled;
        }

        private void pbGetData_Click(object sender, EventArgs e)
        {
            string filename1 = @"D:\" + currentDate + @"_1.csv", filename2 = @"D:\" + currentDate + @"_2.csv",
                filename3 = @"D:\" + currentDate + @"_3.csv";
            try
            {
                screwBatchNo = tbReceiveDataHand.Text;
                workBatchNo = tbReceiveFix.Text;

                //axis1
                AxisEvent(1,"10.5.2.30", @"D:\servo\Spindle1rundowns.csv",filename1,screwBatchNo,workBatchNo);
                //axis2
                AxisEvent(2, "10.5.2.31", @"D:\servo\Spindle2rundowns.csv", filename2, screwBatchNo, workBatchNo);
                //axis3
                AxisEvent(3, "10.5.2.32", @"D:\servo\Spindle3rundowns.csv", filename3, screwBatchNo, workBatchNo);
            }
            catch (Exception error)
            {
                tbErrorMessage.Text = error.ToString();
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            timercount++;
            pbTimer1.Text = timercount.ToString();
            int result, iRet, DValue = 0, DValue2 = 1, D160 = 0, D161 = 0, D162 = 0, D163 = 0, D100 = 0;
            string filename1 = @"D:\" + currentDate + @"_1.csv", filename2 = @"D:\" + currentDate + @"_2.csv",
                filename3 = @"D:\" + currentDate + @"_3.csv";
            try
            {
                result = fx3uPLC.Open();
                iRet = fx3uPLC.WriteDeviceRandom("D150", 1, ref DValue);
                iRet = fx3uPLC.WriteDeviceRandom("D152", 1, ref DValue2);

                iRet = fx3uPLC.ReadDeviceRandom("D160", 1, out D160);
                iRet = fx3uPLC.ReadDeviceRandom("D161", 1, out D161);
                iRet = fx3uPLC.ReadDeviceRandom("D162", 1, out D162);
                iRet = fx3uPLC.ReadDeviceRandom("D163", 1, out D163);
                iRet = fx3uPLC.ReadDeviceRandom("D100", 1, out D100);

                label7.Text = D160.ToString();
                label8.Text = D161.ToString();
                label1.Text = D162.ToString();
                label2.Text = D163.ToString();
                textBox4.Text = D100.ToString();

                if (D160 == 1 | D161 == 1)
                {
                    screwBatchNo = tbReceiveDataHand.Text;
                    workBatchNo = tbReceiveFix.Text;

                    //axis1
                    AxisEvent(1, "10.5.2.30", @"D:\servo\Spindle1rundowns.csv", filename1, screwBatchNo, workBatchNo);
                    iRet = fx3uPLC.WriteDeviceRandom("D150", 1, ref DValue);
                    iRet = fx3uPLC.WriteDeviceRandom("D152", 1, ref DValue2);
                    //axis2
                    AxisEvent(2, "10.5.2.31", @"D:\servo\Spindle2rundowns.csv", filename2, screwBatchNo, workBatchNo);
                    iRet = fx3uPLC.WriteDeviceRandom("D150", 1, ref DValue);
                    iRet = fx3uPLC.WriteDeviceRandom("D152", 1, ref DValue2);
                    //axis3
                    AxisEvent(3, "10.5.2.32", @"D:\servo\Spindle3rundowns.csv", filename3, screwBatchNo, workBatchNo);
                    iRet = fx3uPLC.WriteDeviceRandom("D150", 1, ref DValue);
                    iRet = fx3uPLC.WriteDeviceRandom("D152", 1, ref DValue2);
                    //**************************************************************************************************
                    int YValue = 0;
                    fx3uPLC.WriteDeviceRandom("D160", 1, ref YValue);
                    fx3uPLC.WriteDeviceRandom("D161", 1, ref YValue);
                }
                if (D162 == 1 & D163 == 0)
                {
                    byte[] writeData = new byte[10];
                    //帝商條碼機觸發讀取命令
                    writeData[0] = 0x1B;//ESC
                    writeData[1] = 0x5A;//Z
                    writeData[2] = 0x0D;//CR
                    Array.Resize(ref writeData, writeData.Length);
                    sp2.Write(writeData, 0, writeData.Length);
                    int i = 0;
                    fx3uPLC.WriteDeviceRandom("D162", 1, ref i);
                }
                if (D163 == 1)
                {
                    byte[] writeData = new byte[10];
                    //帝商條碼機觸發停止命令
                    writeData[0] = 0x1B;//ESC
                    writeData[1] = 0x59;//Y
                    writeData[2] = 0x0D;//CR
                    Array.Resize(ref writeData, writeData.Length);
                    sp2.Write(writeData, 0, writeData.Length);
                    int i = 0;
                    fx3uPLC.WriteDeviceRandom("D162", 1, ref i);
                }
            }
            catch (Exception error)
            {
                tbErrorMessage.Text = error.ToString();
            }
            fx3uPLC.Close();
            timer1.Enabled = true;
        }

        

        private bool CheckDatafileExist(int Axis,string datestring)
        {
            bool result_checkfileexist;
            string filename = @"D:\" + datestring + @"_" + Axis.ToString() + @".csv";
            //檢查檔案是否存在
            result_checkfileexist = File.Exists(filename);
            //檔案不存在,建立新的檔案
            if (!result_checkfileexist)
            {
                string title = "ID," + "Date,"+"Time," + "Job," + "Task," + "Status," + "SOC," + "Job Count," + "Tsak Count," +
                    "Temperature," + "Part ID," + "Units," + "Torque," + "Torque Status," + "Angle," + "Angle Status," + "Current," +
                    "Current Status," + "Mult ID," + "Step," + "Screw No," + "Work No ";
                File.WriteAllText(filename, title);
            }
            return result_checkfileexist;
        }

        private void AxisEvent(int AxisNo, string ipaddress,string rundownFilename,string dayFilename,string screwno,string workno)
        {
            string webString = @"http://" + ipaddress + @"/system/spindle1/controller/data/rundowns.csv";
            HttpWebRequest wr = (HttpWebRequest)WebRequest.Create(webString);   //定義一個HTTP的請求
            wr.Timeout = 3000; //設置逾時時間
            wr.Headers.Set("Pragma", "no-cache");
            HttpWebResponse response = (HttpWebResponse)wr.GetResponse();
            Stream streamReceive = response.GetResponseStream();
            Encoding encoding = Encoding.GetEncoding("utf-8");
            StreamReader streamReader = new StreamReader(streamReceive, encoding);
            string resultstring = streamReader.ReadToEnd();
            //輸出處理
            if (AxisNo==1)
            {
                tbResultDataAx1.Clear();
                tbResultDataAx1.Text = resultstring;
            }
            if (AxisNo == 2)
            {
                tbResultDataAx2.Clear();
                tbResultDataAx2.Text = resultstring;
            }
            if (AxisNo == 3)
            {
                tbResultDataAx3.Clear();
                tbResultDataAx3.Text = resultstring;
            }
            //rundown寫入
            StreamWriter sw = new StreamWriter(@"D:\servo\Spindle"+AxisNo.ToString()+@"rundowns.csv");
            sw.Write(resultstring);
            response.Close();
            streamReceive.Close();
            streamReader.Close();
            sw.Close();
            //檢查建立day file
            bool fileexist = CheckDatafileExist(AxisNo,currentDate);
            //讀出rundown file
            string[] rundowntext = File.ReadAllLines(rundownFilename);
            //讀出day file
            string[] daytext = File.ReadAllLines(dayFilename);
            //把daytext轉換成list
            List<string> daylist = new List<string>();
            foreach (var x in daytext)
            {
                daylist.Add(x);
            }
            //插入新資料
            string insertString = rundowntext[1] + "," + workno + "," + screwno;
            daylist.Add(insertString);
            //寫入day file
            TextWriter tw = new StreamWriter(dayFilename);
            foreach (string x in daylist)
            {
                tw.WriteLine(x);
            }
            tw.Dispose();
        }
    }
}
