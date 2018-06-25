// This example uses the Amplitude Modulation Emitter with multiple devices and a Leap Motion
// Controller to project a basic point onto a moving palm, it will automatically switch between the
// two devices based on the direction of the palm.
// The devices is chosen based on the direction of the hand.
// Leaps coordinate system is used as global space.

#include <chrono>
#include <thread>

#include <Leap.h>
#include <UltrahapticsAmplitudeModulation.hpp>

using namespace Ultrahaptics;

int main()
{
    // rotation matrix for a Leap located between the two devices
    Ultrahaptics::Matrix3x3 leap_rot{
        1,  0, 0,
        0,  0, 1,
        0, -1, 0,
    };

    // scale to device units (mm -> m)
    leap_rot *= 0.001;

    // Width of the kit, refer to the User Guide of your device
    float width = 210.0f;  // millimeters (because the global space (Leap space) is in mm)
    float height = 210.0f; // without the Leap cradle
    float leap_height = 30.7f;
    // affine transformation matrix placing the device half its width to the left in the global space
    const auto left_tr  = Ultrahaptics::Transform(leap_rot, {-width / 2, 0.0f, height / 2 + leap_height / 2});
    // affine transformation matrix placing the device half its width to the right in the global space
    const auto right_tr = Ultrahaptics::Transform(leap_rot, { width / 2, 0.0f, height / 2 + leap_height / 2});

    // Create a multi device AmplitudeModulation emitter
    // Please make sure to use the correct identifiers for your devices
    AmplitudeModulation::Emitter emitter("USX:USX-00000000", left_tr);
    emitter.addDevice("USX:USX-00000001", right_tr);
    emitter.setDeviceSelectionMode(DeviceSelectionMode::Direction);

    // Set frequency to 200 Hertz and maximum intensity
    float frequency = 200.0f;
    float intensity = 1.0f;

    // Create an alignment and leap controller.
    Leap::Controller leap_controller;
    for (;;)
    {
        // Get all the hand positions from the leap and position a focal point on each.
        Leap::Frame frame = leap_controller.frame();
        Leap::HandList hands = frame.hands();

        if (!hands.isEmpty())
        {
            // Get the first visible hand.
            Leap::Hand hand = hands[0];

            // Get the palm position and direction
            Leap::Vector leap_palm_position = hand.palmPosition();
            // Invert leap's palm normal because it points out of the palm, whereas the direction of
            // a focalpoint points away from the device.
            Leap::Vector leap_palm_normal = -hand.palmNormal();

            // Create the control point
            const auto p1 = Vector3(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);
            const auto d1 = Vector3(leap_palm_normal.x, leap_palm_normal.y, leap_palm_normal.z);
            AmplitudeModulation::ControlPoint cp = {p1, d1, intensity, frequency};

            // emit
            emitter.update(cp);
        }
        else
        {
            // We can't see a hand, don't emit anything.
            emitter.stop();
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
    }

    return 0;
}
