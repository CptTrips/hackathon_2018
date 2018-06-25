using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using Ultrahaptics;

// This example creates a static focal point at a frequency of 200Hz, 20cm above the device.

public class AM_MultiDeviceExample
{
    public static void Main(string[] args)
    {
        // Width of the kit, refer to the User Guide of your device
        float width = 0.210f;  // meters
        // affine transformation matrix placing the device half its width to the left in the global space
        Transform left_tr = new Transform();
        left_tr.setOrigin(new Vector3(-width / 2, 0.0f, 0.0f));
        // affine transformation matrix placing the device half its width to the right in the global space
        Transform right_tr = new Transform();
        right_tr.setOrigin(new Vector3(width / 2, 0.0f, 0.0f));

        // Create an emitter, which connects to the first connected device
        // Please make sure to use the correct identifiers for your devices
        AmplitudeModulationEmitter emitter = new AmplitudeModulationEmitter("USX:USX-00000000", left_tr);
        emitter.addDevice("USX:USX-00000001", right_tr);

        // Set the position of the new control point
        Vector3 position1 = new Vector3(-0.05f, 0.0f, 0.2f);
        Vector3 position2 = new Vector3( 0.05f, 0.0f, 0.2f);
        // Set how intense the feeling at the new control point will be
        float intensity = 1.0f;
        // Set the frequency of the control point, which can change the feeling of the sensation
        float frequency = 200.0f;

        // Define the control point
        AmplitudeModulationControlPoint point1 = new AmplitudeModulationControlPoint (position1, intensity, frequency);
        AmplitudeModulationControlPoint point2 = new AmplitudeModulationControlPoint (position2, intensity, frequency);
        var points = new List<AmplitudeModulationControlPoint> { point1, point2 };
        // Instruct the device to stop any existing actions and start producing this control point
        bool isOK = emitter.update(points);
        // The emitter will continue producing this point until instructed to stop

        // Wait until the program is ready to stop
        Console.ReadKey();

        // Stop the emitter
        emitter.stop();

        // Dispose/destroy the emitter
        emitter.Dispose();
        emitter = null;
    }
}
