// This example uses the Leap Motion Controller to place a point on the user's hand.
// The emitter will stop when a hand is not detected or the Leap Motion Controller is not connected.

using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Ultrahaptics;

public class Unity_AMHandTracking : MonoBehaviour
{
    AmplitudeModulationEmitter _emitter;
    Alignment _alignment;
    Leap.Controller _leap;

    void Start()
    {
        // Initialize the emitter
        _emitter = new AmplitudeModulationEmitter();
        _emitter.initialize();
        _leap = new Leap.Controller();

        // NOTE: This example uses the Ultrahaptics.Alignment class to convert Leap coordinates to the Ultrahaptics device space.
        // You can either use this as-is for Ultrahaptics development kits, create your own alignment files for custom devices,
        // or replace the Alignment references in this example with your own coordinate conversion system.

        // Load the appropriate alignment file for the currently-used device
        _alignment = _emitter.getDeviceInfo().getDefaultAlignment();
        // Load a custom alignment file (absolute path, or relative path from current working directory)
        // _alignment = new Alignment("my_custom.alignment.xml");
    }


    // Converts a Leap Vector directly to a UH Vector3
    Ultrahaptics.Vector3 LeapToUHVector(Leap.Vector vec)
    {
        return new Ultrahaptics.Vector3 (vec.x, vec.y, vec.z);
    }


    // Update on every frame
    void Update()
    {
        if (_leap.IsConnected)
        {
            var frame = _leap.Frame ();
            if (frame.Hands.Count > 0)
            {
                // The Leap Motion can see a hand, so get its palm position
                Leap.Vector leapPalmPosition = frame.Hands[0].PalmPosition;
                // Convert to our vector class, and then convert to our coordinate space
                Ultrahaptics.Vector3 uhPalmPosition = _alignment.fromTrackingPositionToDevicePosition(LeapToUHVector(leapPalmPosition));
                // Create a control point object using this position, with full intensity, at 200Hz
                AmplitudeModulationControlPoint point = new AmplitudeModulationControlPoint(uhPalmPosition, 1.0f, 200.0f);
                // Output this point
                _emitter.update(new List<AmplitudeModulationControlPoint> { point });
            }
            else
            {
                Debug.LogWarning ("No hands detected");
                _emitter.stop();
            }
        }
        else
        {
            Debug.LogWarning ("No Leap connected");
            _emitter.stop();
        }
    }


    // Ensure the emitter is stopped when disabled
    void OnDisable()
    {
        _emitter.stop();
    }

    // Ensure the emitter is immediately disposed when destroyed
    void OnDestroy()
    {
        _emitter.Dispose();
        _emitter = null;
        _alignment.Dispose();
        _alignment = null;
    }
}
