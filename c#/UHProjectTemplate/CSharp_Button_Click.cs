using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using Ultrahaptics;
using Leap;


public struct ButtonWidget
{
    public float radius;
    public float angle;
    public ButtonWidget(float r = 0.025f, float a = 0.0f){
        radius = r;
        angle = a;
    }
}

public static class MathF
{
    public static Func<double, float> Cos = angleR => (float)Math.Cos(angleR);
    public static Func<double, float> Sin = angleR => (float)Math.Sin(angleR);
}

// This example creates a static focal point at a frequency of 200Hz, 20cm above the device.
public class ButtonExample
{

    const float PI = (float)Math.PI;

    public static void Main(string[] args)
    {
        LMController lmc = new LMController();
        // Create an emitter, which connects to the first connected device
        AmplitudeModulationEmitter emitter = new AmplitudeModulationEmitter();

        // Create an aligment object which relates the tracking and device spaces
        Alignment alignment = emitter.getDeviceInfo().getDefaultAlignment();

        // Create a Leap Contoller
        Controller controller = new Controller();

        ButtonWidget button = new ButtonWidget();

        // Set the position of the new control point
        Vector3 position1 = new Vector3(0.0f, 0.0f, 0.2f);
        Vector3 position2 = new Vector3(0.0f, 0.0f, 0.2f);
        Vector3 position3 = new Vector3(0.0f, 0.0f, 0.2f);
        Vector3 position4 = new Vector3(0.0f, 0.0f, 0.2f);
        // Set how intense the feeling at the new control point will be
        float intensity = 1.0f;
        // Set the frequency of the control point, which can change the feeling of the sensation
        float frequency = 200.0f;

        int countr = 0;

        int countl = 0;

        // Define the control point
        AmplitudeModulationControlPoint point1 = new AmplitudeModulationControlPoint (position1, intensity, frequency);
        AmplitudeModulationControlPoint point2 = new AmplitudeModulationControlPoint (position2, intensity, frequency);
        AmplitudeModulationControlPoint point3 = new AmplitudeModulationControlPoint (position3, intensity, frequency);
        AmplitudeModulationControlPoint point4 = new AmplitudeModulationControlPoint (position4, intensity, frequency);
        var points = new List<AmplitudeModulationControlPoint> { point1, point2, point3, point4 };

        // Wait for leap
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

        bool button_on = true;
        new Stopwatch();

        for(;;)
        {
            Frame frame = controller.Frame();
            HandList hands = frame.Hands;


            if(!hands.IsEmpty && button_on)
            {
                Hand hand = hands[0];
                Hand firsthand = hands[0];
        
                FingerList fingers = firsthand.Fingers;
                Finger finger1 = fingers[1];
                Finger finger2 = fingers[2];
                Finger finger3 = fingers[3];
                Finger finger4 = fingers[4];


                Bone bone1 = finger1.Bone(Bone.BoneType.TYPE_DISTAL);
                Bone bone2 = finger1.Bone(Bone.BoneType.TYPE_DISTAL);
                Bone bone3 = finger1.Bone(Bone.BoneType.TYPE_DISTAL);
                Bone bone4 = finger1.Bone(Bone.BoneType.TYPE_DISTAL);



                Vector middle1 = bone1.Center;
                Vector middle2 = bone2.Center;  
                Vector middle3 = bone3.Center;  
                Vector middle4 = bone4.Center;                

                for(int i = 0; i < frame.Gestures().Count; i++)
                {
                    Gesture gesture = frame.Gestures()[i];

                    if(gesture.Type == Gesture.GestureType.TYPE_KEY_TAP)
                    {
                        button_on = false;

                        emitter.stop();
                        break;
                    }
                }
                // position = new Vector3(hand.PalmPosition.x, hand.PalmPosition.y, hand.PalmPosition.z);
                position1 = new Vector3(middle1.x, middle1.y, middle1.z);
                position2 = new Vector3(middle2.x, middle2.y, middle2.z);
                position3 = new Vector3(middle3.x, middle3.y, middle3.z);
                position4 = new Vector3(middle4.x, middle4.y, middle4.z);

                //for(int i = 0; i < 10000; i++)
                //{
                //    float zpos = middle.z + i/1000;
                //    position = new Vector3(middle.x, middle.y, zpos);
                //}

                Console.WriteLine(hand.PalmPosition.x);
                Console.WriteLine(hand.PalmPosition.z-125.0);

                if(hand.PalmPosition.z-125.0 > 60){
                   Console.WriteLine("Backwards");
                   Console.WriteLine(hand.PalmPosition.z-60);
                
                }

                if(hand.PalmPosition.z-125.0 < -60){
                   Console.WriteLine("Forwards");
                   Console.WriteLine(-hand.PalmPosition.z-60);
                
                }
                
                if(hand.PalmPosition.x > 60){
                
                   countr = countr + 1;
                   if(countr > 50){
                       Console.WriteLine("Right");
                       countr = 0;

                   }
                  
                   //Console.WriteLine(hand.PalmPosition.z-60);

                }

                if(hand.PalmPosition.x < -60){
                   countl = countl + 1;
                   if(countl > 50){
                       Console.WriteLine("Left");
                       countl = 0;

                   }
                }

                Vector3 normal = new Vector3(-hand.PalmNormal.x, -hand.PalmNormal.y, -hand.PalmNormal.z);
                Vector3 direction = new Vector3(hand.Direction.x, hand.Direction.y, hand.Direction.z);

                Vector3 device_position1 = alignment.fromTrackingPositionToDevicePosition(position1);
                Vector3 device_position2 = alignment.fromTrackingPositionToDevicePosition(position2);
                Vector3 device_position3 = alignment.fromTrackingPositionToDevicePosition(position3);
                Vector3 device_position4 = alignment.fromTrackingPositionToDevicePosition(position4);
                Vector3 device_normal = alignment.fromTrackingDirectionToDeviceDirection(normal).normalize();
                Vector3 device_direction = alignment.fromTrackingDirectionToDeviceDirection(direction).normalize();
                Vector3 device_palm_x = device_direction.cross(device_normal).normalize();

                
                points[0].setPosition(device_position1);
                points[1].setPosition(device_position2);
                points[2].setPosition(device_position3);
                points[3].setPosition(device_position4);
                // Instruct the device to stop any existing actions and start producing this control point
                //emitter.update(points);
                float cal = 0.1;
                emitter.update(fingers.Skip(1)
                    .Select(f => f.TipPosition)
                    .Select(v => alignment.fromTrackingPositionToDevicePosition(new Vector3(v.x - cal*f.direction.x, v.y - cal*f.direction.y, v.z-cal*f.direction.z)))
                    .Select(v => new AmplitudeModulationControlPoint(v, 1, 140)));
                // The emitter will continue producing this point until instructed to stop

                button.angle += 0.05f;
                button.angle = button.angle % (2.0f * PI);
            }
            else if(!hands.IsEmpty && !button_on)
            {
                emitter.stop();

                for(int i = 0; i < frame.Gestures().Count; i++)
                {
                    Gesture gesture = frame.Gestures()[i];

                    if(gesture.Type == Gesture.GestureType.TYPE_KEY_TAP)
                    {
                        button_on = true;

                        emitter.stop();
                        break;
                    }
                }
            }
            else
            {
                emitter.stop();
            }
            System.Threading.Thread.Sleep(10);

        float[] vec = new float[] {0.0F,1.0F};

        lmc.Send(vec);

        }

        // Dispose/destroy the emitter
        emitter.Dispose();
        emitter = null;

        controller.Dispose ();
    }
}
