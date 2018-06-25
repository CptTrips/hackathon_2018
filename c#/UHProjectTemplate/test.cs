using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using Ultrahaptics;
using Leap;

public class Test
{
    const float PI = (float)Math.PI;
    public static void Main(string[] args)
    {
        AmplitudeModulationEmitter emitter = new AmplitudeModulationEmitter();

        Controller controller = new Controller();

        if(!controller.IsConnected)
        {
            Console.WriteLine("Waiting for Leap");
            while (!controller.IsConnected)
            {
                System.Threading.Thread.Sleep(1000);
                Console.WriteLine(".");
            }
            Console.WriteLine("\n");
        }

        controller.EnableGesture (Gesture.GestureType.TYPE_KEY_TAP);

        if(controller.Config.SetFloat("Gesture.Swipe.MinDistance", 30) &&
            controller.Config.SetFloat("Gesture.Swipe.MinDownVelocity", 30) &&
            controller.Config.SetFloat("Gesture.Swipe.MinSeconds", 0.01f))
        {
            controller.Config.Save();
        }
        
        for(;;)
        {

            Frame frame = controller.Frame ();
            HandList hands = frame.Hands;

            Gesture gesture = frame.Gesture;

            Hand firsthand = hands[0];
        
            FingerList fingers = firsthand.Fingers;

            Vector position = firsthand.PalmPosition;

            Console.WriteLine(position);
            Console.WriteLine(gesture);
        }
    }    
}