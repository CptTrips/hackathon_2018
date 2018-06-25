using System;
using System.Net.Sockets;
using System.Net;
using System.Text;


public class SendHi
{
  static public void Main ()
  {
    var client = new UdpClient();
    IPEndPoint pi = new IPEndPoint(IPAddress.Parse("192.168.16.23"), 7777);
    client.Connect(pi);

    while(true)
    {
      string msg_char = Console.ReadLine(); // Maybe ReadKey()
      Byte[] msg = Encoding.ASCII.GetBytes(msg_char);

      client.Send(msg, msg.Length);

      var rcv_msg = client.Receive(ref pi);

      Console.Write("pi: " + Encoding.ASCII.GetString(rcv_msg));

    }
  }
}
