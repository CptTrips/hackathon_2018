using System;
using System.Net.Sockets;
using System.Net;
using System.Text;


public class WASDController
{
  static public void wasd ()
  {
    var client = new UdpClient();
    IPEndPoint pi = new IPEndPoint(IPAddress.Parse("192.168.16.23"), 7777);
    client.Connect(pi);

    while(true)
    {

      ConsoleKeyInfo msg_cki = Console.ReadKey();

      Byte[] msg = Encoding.UTF8.GetBytes(msg_cki.Key.ToString());

      Console.Write(msg_cki.Key.ToString());

      client.Send(msg, msg.Length);

    }
  }
}
public class LMController
{

  public UdpClient client;
  public LMController()
  {

    client = new UdpClient();

    string pi_ip = "192.168.16.23";

    IPEndPoint pi = new IPEndPoint(IPAddress.Parse(pi_ip), 7777);
    Console.Write("Connecting to pi (" + pi_ip + ")...");
    client.Connect(pi);
    Console.Write("Connected");
  }

  public void Send(float[] dir)
  {

    Byte[] x_byte = BitConverter.GetBytes(dir[0]);

    Byte[] y_byte = BitConverter.GetBytes(dir[1]);

    var msg = new Byte[x_byte.Length + y_byte.Length];

    x_byte.CopyTo(msg, 0);

    y_byte.CopyTo(msg, x_byte.Length);

    client.Send(msg, msg.Length);

  }
}



