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

      Byte[] msg = Encoding.ASCII.GetBytes(msg_cki.Key.ToString());

      client.Send(msg, msg.Length);

    }
  }
}
