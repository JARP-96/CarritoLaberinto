using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace RutaRobotLaberinto
{
    public partial class Form1 : Form
    {
        //SerialPort sp = new SerialPort("COM6");
        SerialPort sp = new SerialPort("COM1",
      9600, Parity.None, 8, StopBits.One);
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            int result = serialPort1.ReadChar();
            char message = (char)result;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            sp.Open();
            while (!sp.IsOpen) ;
            //System.Threading.Thread.Sleep(500);
            char[] buffer = { 'S' };
            int offset=0;
            int count=1;
            sp.Write(buffer,offset,count);
            HacerTodo();
        }
        private void HacerTodo()
        {
            List<char> instrucciones = new List<char>();

            while (!sp.IsOpen) ;
            //System.Threading.Thread.Sleep(500);
            char recived;
            do
            {  
                recived = (char)sp.ReadChar();
                //if(recived != '?')
                instrucciones.Add(recived);
                //MessageBox.Show(recived.ToString());
            } while (recived != 'F');
            
            //instrucciones.Add('F');
            // Inicialización de Variables
            int width = 50, height = 50;
            int xAct = 25, yAct = 25;

            //char [] instrucciones= new char [] {'A', 'P', 'P', 'D', 'P', 'A', 'P', 'P', 'I', 'P', 'A', 'P', 'P', 'D', 'P', 'P', 'P', 'B', 'P', 'P' };
            char instruccion = 'A';

            // Se inicializa la imágen
            Bitmap bmp = new Bitmap(width, height);

            int[,] mapa;
            mapa = new int[width, height];
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    mapa[x, y] = 0;
                }
            }
            //mapa[15, 15] = 1;
            //mapa[15, 16] = 1;
            //mapa[16, 16] = 1;
            //mapa[16, 17] = 1;
            //mapa[16, 18] = 1;
            //mapa[15, 18] = 1;

            for (int i = 0; i < instrucciones.Count; i++)
            {
                // Dar Paso
                if (instrucciones[i] == 'P')
                {
                    if (instruccion == 'A')
                    {
                        yAct++;
                        mapa[xAct, yAct] = 1;
                    }
                    if (instruccion == 'B')
                    {
                        yAct--;
                        mapa[xAct, yAct] = 1;
                    }
                    if (instruccion == 'D')
                    {
                        xAct++;
                        mapa[xAct, yAct] = 1;
                    }
                    if (instruccion == 'I')
                    {
                        xAct--;
                        mapa[xAct, yAct] = 1;
                    }
                }
                // Cambiar orientación hacia Adelante
                if (instrucciones[i] == 'A')
                {
                    instruccion = 'A';
                }
                // Cambiar orientación hacia la Izquierda
                if (instrucciones[i] == 'I')
                {
                    instruccion = 'I';
                }
                // Cambiar orientación hacia la Derecha
                if (instrucciones[i] == 'D')
                {
                    instruccion = 'D';
                }
                // Cambiar orientación hacia aBajo
                if (instrucciones[i] == 'B')
                {
                    instruccion = 'B';
                }
                // Esto dentro de recibir por Serial
                if (instrucciones[i] == 'F')
                {
                    break;
                }
            }

            /*
             * Imprime el Mapa Final
            */

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    if (mapa[x, y] == 0)
                    {
                        bmp.SetPixel(x, y, Color.Black);
                    }
                    if (mapa[x, y] == 1)
                    {
                        bmp.SetPixel(x, y, Color.White);
                    }
                }
            }

            pictureBox1.Image = bmp;

            bmp.Save("D:/Downloads/Debug/RandomImage.png");
        }
    }
}
