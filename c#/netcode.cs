using System;
using System.Net.Sockets;
using System.Net;
using System.Text;


public class WASDController
{
  static public void Main ()
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
  public LMController
  {
    var client = new UdpClient();

    string pi_ip = "192.168.16.23"

    IPEndPoint pi = new IPEndPoint(IPAddress.Parse(pi_ip), 7777);
    Console.Write("Connecting to pi (" + pi_ip + ")...")
    client.Connect(pi)
    Console.Write("Connected")
  }

  public void Send(string dir)
  {
    Byte[] msg = Encoding.UTF8.GetBytes(dir);

    client.Send(msg, msg.Length);

  }
}
