using System;

public class Program
{
  static public void Main ()
  {
    LMController lmc = new LMController();

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

      lmc.Send(msg_vec);

      float[] r = lmc.Ranges;

      Console.WriteLine("Ranges: ");
      Console.Write(string.Format("{0}, {1}, {2}", r[0], r[1], r[2]));

    }
  }
}

