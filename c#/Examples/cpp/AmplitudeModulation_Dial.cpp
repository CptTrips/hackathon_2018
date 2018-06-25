// This example uses the Amplitude Modulation Emitter and a Leap Motion Controller to
// project a rotating dial onto the palm.
// Change the value of the dial by rotating your hand clockwise or anticlockwise.
// You will feel the dial begin to spin faster as you increase the value.

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <map>
#include <cmath>
#include <thread>

#include <Leap.h>
#include <UltrahapticsAmplitudeModulation.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

struct DialWidget
{
    double radius = 25.0 * Ultrahaptics::Units::mm;
    double angle = 0.0;
    double max = 100.0;
    double value = 50.0;
};

int main(int argc, char *argv[])
{
    Ultrahaptics::AmplitudeModulation::Emitter emitter;

    // Load the appropriate Leap Motion alignment matrix for this kit
    Ultrahaptics::Alignment alignment = emitter.getDeviceInfo().getDefaultAlignment();
    Leap::Controller leap_controller;
    DialWidget dial;

    const float intensity = 1.0f;
    const float frequency = 200.0 * Ultrahaptics::Units::hertz;

    int circle_id = -1;
    double circle_progress = 0;

    if (!leap_controller.isConnected())
    {
        std::cout << "Waiting for Leap" << std::flush;
        while (!leap_controller.isConnected())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "." << std::flush;
        }
        std::cout << std::endl;
    }

    // Register to look for a circle gesture.
    leap_controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);

    // Register gesture constraints.
    if (leap_controller.config().setFloat("Gesture.Circle.MinRadius", 10.0)
        && leap_controller.config().setFloat("Gesture.Circle.MinArc", 0.5))
    {
        leap_controller.config().save();
    }

    // Get the current time.
    auto circle_last_seen = std::chrono::steady_clock::now();

    for (;;)
    {
        // Get the list of hands from the frame.
        Leap::Frame frame = leap_controller.frame();
        Leap::HandList hands = frame.hands();

        if (!hands.isEmpty())
        {
            // Get the first visible hand.
            Leap::Hand hand = hands[0];

            // Detect Gesturing.
            // The Leap motion reports the status of every gesture on each frame. It reports an individual gesture
            // for each detected circling finger, so if the whole hand rotates we get 5 circle gestures.
            // Track just the first one and make a note of its ID to link it to future updates in frames.
            for (auto& gesture : frame.gestures())
            {
                if (gesture.type() == Leap::Gesture::TYPE_CIRCLE)
                {
                    Leap::CircleGesture circle(gesture);

                    auto period = std::chrono::steady_clock::now() - circle_last_seen;
                    double lastSeenTime = std::chrono::duration_cast<std::chrono::milliseconds>(period).count();
                    // We can't rely on seeing a STATE_STOP so time out gestures and reset.
                    if (circle_id != -1 && lastSeenTime >= 200)
                    {
                        circle_id = -1;
                        circle_progress = 0;
                    }

                    if (circle.state() == Leap::Gesture::STATE_START || circle.state() == Leap::Gesture::STATE_UPDATE)
                    {
                        // Only track a single gesture.
                        if (circle_id == -1)
                        {
                            circle_id = circle.id();
                        }
                        else if (circle.id() != circle_id)
                        {
                            continue; // Ignore extra gestures.
                        }

                        // Work out circle direction.
                        const int direction = (circle.pointable().direction().angleTo(circle.normal()) <= M_PI / 2.0) ? 1 : -1;

                        // Clamp the change to prevent erroneous jumps.
                        const double oldProgress = circle_progress;
                        const double newProgress = (circle.progress()) * direction;
                        double change = std::max(std::min(newProgress - oldProgress, 0.3), -0.3);
                        circle_progress = newProgress;
                        change *= circle.durationSeconds();

                        if (std::abs(newProgress) > std::abs(oldProgress))
                        {
                            // Apply the change to the dial speed with suitable scaling.
                            dial.value = std::min(std::max(dial.value + (change * 10.0), 0.0), dial.max);
                        }

                        // Update last seen time.
                        circle_last_seen = std::chrono::steady_clock::now();
                    }
                    else if (circle.state() == Leap::Gesture::STATE_STOP)
                    {
                        if (circle.id() == circle_id)
                        {
                            circle_id = -1;
                            circle_progress = 0.0;
                        }
                    }
                }
            }

            // Project the widget onto the palm
            Leap::Vector leap_palm_position3D = hand.palmPosition();
            Leap::Vector leap_palm_position2D = hand.stabilizedPalmPosition();
            Leap::Vector leap_palm_normal = hand.palmNormal();
            Leap::Vector leap_palm_direction = hand.direction();

            Ultrahaptics::Vector3 uh_palm_position(leap_palm_position2D.x, leap_palm_position2D.y, leap_palm_position3D.z);
            Ultrahaptics::Vector3 uh_palm_normal(leap_palm_normal.x, leap_palm_normal.y, leap_palm_normal.z);
            Ultrahaptics::Vector3 uh_palm_direction(leap_palm_direction.x, leap_palm_direction.y, leap_palm_direction.z);

            // Convert to device space from leap space.
            Ultrahaptics::Vector3 device_palm_position = alignment.fromTrackingPositionToDevicePosition(uh_palm_position);
            Ultrahaptics::Vector3 device_palm_z = alignment.fromTrackingDirectionToDeviceDirection(uh_palm_normal).normalize();
            Ultrahaptics::Vector3 device_palm_y = alignment.fromTrackingDirectionToDeviceDirection(uh_palm_direction).normalize();
            Ultrahaptics::Vector3 device_palm_x = device_palm_y.cross(device_palm_z).normalize();

            // Work out the position of the control point.
            Ultrahaptics::Vector3 position = device_palm_position + (device_palm_y * std::cos(dial.angle) + device_palm_x * std::sin(dial.angle)) * dial.radius;

            // Create the control point and emit.
            Ultrahaptics::AmplitudeModulation::ControlPoint point(position, intensity, frequency);
            emitter.update(point);

            // Update the dial position.
            dial.angle -= (dial.value / dial.max) * 0.1;
            dial.angle = fmod(dial.angle, 2.0 * M_PI);
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
