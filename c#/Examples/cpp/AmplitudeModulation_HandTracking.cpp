// This example uses the Amplitude Modulation Emitter and a Leap Motion Controller to
// project a basic point onto a moving palm

#include <chrono>
#include <thread>

#include <Leap.h>
#include <UltrahapticsAmplitudeModulation.hpp>

int main(int argc, char *argv[])
{
    // Create an emitter and a leap controller.
    Ultrahaptics::AmplitudeModulation::Emitter emitter;
    Leap::Controller leap_controller;
    // Load the appropriate Leap Motion alignment matrix for this kit
    Ultrahaptics::Alignment alignment = emitter.getDeviceInfo().getDefaultAlignment();

    // Set frequency to 200 Hertz and maximum intensity
    float frequency = 200.0f * Ultrahaptics::Units::hertz;
    float intensity = 1.0f;

    for (;;)
    {
        // Get all the hand positions from the leap and position a focal point on each.
        Leap::Frame frame = leap_controller.frame();
        Leap::HandList hands = frame.hands();

        if (!hands.isEmpty())
        {
            // Get the first visible hand.
            Leap::Hand hand = hands[0];

            // Translate the hand position from leap objects to Ultrahaptics objects.
            Leap::Vector leap_palm_position = hand.palmPosition();
            Leap::Vector leap_palm_normal = hand.palmNormal();
            Leap::Vector leap_palm_direction = hand.direction();

            // Convert to Ultrahaptics vectors, normal is negated as leap normal points down.
            Ultrahaptics::Vector3 uh_palm_position(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);
            Ultrahaptics::Vector3 uh_palm_normal(-leap_palm_normal.x, -leap_palm_normal.y, -leap_palm_normal.z);
            Ultrahaptics::Vector3 uh_palm_direction(leap_palm_direction.x, leap_palm_direction.y, leap_palm_direction.z);

            // Convert to device space from leap space.
            Ultrahaptics::Vector3 device_palm_position = alignment.fromTrackingPositionToDevicePosition(uh_palm_position);
            Ultrahaptics::Vector3 device_palm_normal = alignment.fromTrackingDirectionToDeviceDirection(uh_palm_normal).normalize();
            Ultrahaptics::Vector3 device_palm_direction = alignment.fromTrackingDirectionToDeviceDirection(uh_palm_direction).normalize();

            // These can then be converted to be a unit axis on the palm of the hand.
            Ultrahaptics::Vector3 device_palm_z = device_palm_normal;                             // Unit Z direction.
            Ultrahaptics::Vector3 device_palm_y = device_palm_direction;                          // Unit Y direction.
            Ultrahaptics::Vector3 device_palm_x = device_palm_y.cross(device_palm_z).normalize(); // Unit X direction.

            // Use these to create a point at 2cm x 2cm from the centre of the palm
            Ultrahaptics::Vector3 position = device_palm_position + (2 * Ultrahaptics::Units::cm * device_palm_x) + (2 * Ultrahaptics::Units::cm * device_palm_y);

            // Create the control point and emit.
            Ultrahaptics::ControlPoint point(position, intensity, frequency);
            emitter.update(point);
        }
        else
        {
            // We can't see a hand, don't emit anything.
            emitter.stop();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
