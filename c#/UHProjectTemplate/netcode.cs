using System;
using System.Net.Sockets;
using System.Net;
using System.Text;


public class WASDController
{
  static public void WASD ()
  {
    var client = new UdpClient();
    IPEndPoint pi = new IPEndPoint(IPAddress.Parse("192.168.16.23"), 7777);
    client.Connect(pi);

    while(true)
    {

      ConsoleKeyInfo msg_cki = Console.ReadKey();

      string msg_key = msg_cki.Key.ToString();

      Console.WriteLine(msg_key);

      // Switch on the keystroke and convert to float[2] then to bytes

      float[] msg_vec;

      switch (msg_key)
      {
        case "W":
          msg_vec = new float[] {0.0F, 1.0F};
          break;
        case "A":
          msg_vec = new float[] {-1.0F, 0.0F};
          break;
        case "S":
          msg_vec = new float[] {0.0F, -1.0F};
          break;
        case "D":
          msg_vec = new float[] {1.0F, 0.0F};
          break;
        default:
          msg_vec = new float[] {0.0F, 0.0F};
          break;
      }

      Byte[] x_byte = BitConverter.GetBytes(msg_vec[0]);

      Byte[] y_byte = BitConverter.GetBytes(msg_vec[1]);

      var msg = new Byte[x_byte.Length + y_byte.Length];

      x_byte.CopyTo(msg, 0);

      y_byte.CopyTo(msg, x_byte.Length);

      Console.WriteLine(BitConverter.ToString(msg));

      client.Send(msg, msg.Length);

    }
  }
}

public class LMController
{

  public UdpClient client;

  public IPEndPoint pi;

  public float[] ranges;

  public LMController()
  {

    client = new UdpClient();

    string pi_ip = "192.168.42.1";

    pi = new IPEndPoint(IPAddress.Parse(pi_ip), 7777);
    Console.WriteLine("Connecting to pi (" + pi_ip + ")...");
    client.Connect(pi);
    Console.WriteLine("Connected");

    ranges = new float[] {1.0F, 1.0F, 1.0F};
  }

  public void Send(float[] dir)
  {

    Byte[] x_byte = BitConverter.GetBytes(dir[0]);

    Byte[] y_byte = BitConverter.GetBytes(dir[1]);

    var msg = new Byte[x_byte.Length + y_byte.Length];

    x_byte.CopyTo(msg, 0);

    y_byte.CopyTo(msg, x_byte.Length);

    Console.WriteLine(BitConverter.ToString(msg));

    client.Send(msg, msg.Length);

  }

  private void Listen()
  {
    bool done = false;

    try
    {

      while (!done)
      {
        byte[] feedback = client.Receive(ref pi);
        // Decode the feedback and write it to property
        float range_front = BitConverter.ToSingle(feedback, 0);
        float range_left = BitConverter.ToSingle(feedback, 4);
        float range_right = BitConverter.ToSingle(feedback, 8);

        Console.WriteLine("Received ranges: " + range_front.ToString("0.00") +
                                          "," + range_front.ToString("0.00") +
                                          "," + range_front.ToString("0.00"));

        ranges = new float[] {range_front, range_left, range_right};


      }
    }
    catch (Exception e)
    {
      Console.WriteLine(e.ToString());
    }
    finally
    {
      client.Close();
    }
  }

  public float[] Ranges
  {
    get
    {
      return ranges;
    }
  }
}



