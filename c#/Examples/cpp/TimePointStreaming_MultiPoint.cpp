// This example uses the Timepoint Streaming emitter and a Leap Motion Controller.
// It projects two lines (drawn with two separate control points) onto the palm.

#include <atomic>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <Leap.h>

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

#include "UltrahapticsTimePointStreaming.hpp"

// Structure to represent output from the Leap listener
struct LeapOutput
{
    LeapOutput() noexcept
    {}

    Ultrahaptics::Vector3 palm_position;
    bool hand_present = false;
};

// Leap listener class - tracking the hand position and creating data structure for use by Ultrahaptics API
class LeapListening : public Leap::Listener
{
public:
    LeapListening(const Ultrahaptics::Alignment& align)
      : alignment(align)
    {
    }

    ~LeapListening() = default;

    LeapListening(const LeapListening &other) = delete;
    LeapListening &operator=(const LeapListening &other) = delete;

    void onFrame(const Leap::Controller &controller) override
    {
        // Get all the hand positions from the leap and position a focal point on each.
        Leap::Frame frame = controller.frame();
        Leap::HandList hands = frame.hands();
        LeapOutput local_hand_data;

        if (hands.isEmpty())
        {
            local_hand_data.palm_position = Ultrahaptics::Vector3();
            local_hand_data.hand_present = false;
        }
        else
        {
            Leap::Hand hand = hands[0];

            // Translate the hand position from leap objects to Ultrahaptics objects.
            Leap::Vector leap_palm_position = hand.palmPosition();
            Leap::Vector leap_palm_normal = hand.palmNormal();
            Leap::Vector leap_palm_direction = hand.direction();

            // Convert to Ultrahaptics vectors, normal is negated as leap normal points down.
            Ultrahaptics::Vector3 uh_palm_position(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);
            Ultrahaptics::Vector3 uh_palm_normal(-leap_palm_normal.x, -leap_palm_normal.y, -leap_palm_normal.z);
            Ultrahaptics::Vector3 uh_palm_direction(leap_palm_direction.x, leap_palm_direction.y, leap_palm_direction.z);

            // Convert from leap space to device space.
            Ultrahaptics::Vector3 device_palm_position = alignment.fromTrackingPositionToDevicePosition(uh_palm_position);
            Ultrahaptics::Vector3 device_palm_normal = alignment.fromTrackingDirectionToDeviceDirection(uh_palm_normal).normalize();
            Ultrahaptics::Vector3 device_palm_direction = alignment.fromTrackingDirectionToDeviceDirection(uh_palm_direction).normalize();

            // Check palm position is within 14cm either side of the centre of array.
            if (device_palm_position.x >= -14.0f * Ultrahaptics::Units::cm && device_palm_position.x <= 14.0f * Ultrahaptics::Units::cm)
            {
                local_hand_data.palm_position = device_palm_position;
                local_hand_data.hand_present = true;
            }
            else
            {
                local_hand_data.palm_position = Ultrahaptics::Vector3();
                local_hand_data.hand_present = false;
            }
        }
        atomic_local_hand_data.store(local_hand_data);
    }

    LeapOutput getLeapOutput()
    {
        return atomic_local_hand_data.load();
    }

private:
    std::atomic<LeapOutput> atomic_local_hand_data;
    Ultrahaptics::Alignment alignment;
};

// Structure for passing information on the type of point to create
struct ModulatedPoint
{
    // Create the structure, passing the appropriate alignment through to the LeapListening class
    ModulatedPoint(const Ultrahaptics::Alignment& align) : hand(align)
    {
    }

    // Hand data
    LeapListening hand;

    // The position of the control point
    Ultrahaptics::Vector3 position;

    // The length of the lines to draw
    double line_length;

    // How often the line is drawn every second
    double line_draw_frequency;

    // The offset of the control point at the last sample time
    double offset = 0.0f;

    // This allows us to easily reverse the direction of the point
    // It can be -1 or 1.
    int direction = 1;
};

// Callback function for filling out complete device output states through time
void my_emitter_callback(const Ultrahaptics::TimePointStreaming::Emitter &timepoint_emitter,
                         Ultrahaptics::TimePointStreaming::OutputInterval &interval,
                         const Ultrahaptics::HostTimePoint &submission_deadline,
                         void *user_pointer)
{
    // Cast the user pointer to the struct that describes the control point behaviour
    ModulatedPoint *my_modulated_point = static_cast<ModulatedPoint*>(user_pointer);

    // Set interval offset between control point samples
    double interval_offset = my_modulated_point->line_length * my_modulated_point->line_draw_frequency / timepoint_emitter.getSampleRate();

    // Get a copy of the Leap data
    LeapOutput leapOutput = my_modulated_point->hand.getLeapOutput();
    Ultrahaptics::Vector3 palm_offset = Ultrahaptics::Vector3(-2.0f * Ultrahaptics::Units::cm, 2.0f * Ultrahaptics::Units::cm, 0.0f);

    // Loop through time, setting control points
    for (auto& sample : interval)
    {
        if (!leapOutput.hand_present) {
            // no hand -> don't output anything
            sample.persistentControlPoint(0).setIntensity(0.0f);
            sample.persistentControlPoint(1).setIntensity(0.0f);
            continue;
        }
        // Points move back and forth diagonally across the palm. Only tracking palm in z-axis.
        my_modulated_point->position.x = my_modulated_point->offset;
        my_modulated_point->position.y = my_modulated_point->offset;
        my_modulated_point->position.z = leapOutput.palm_position.z;

        // Use the calculated line data to project two diagonal lines onto the palm. The lines are slightly offset from the center of the palm.
        sample.persistentControlPoint(0).setPosition(my_modulated_point->position + palm_offset);
        sample.persistentControlPoint(0).setIntensity(1.0f);
        sample.persistentControlPoint(1).setPosition(my_modulated_point->position - palm_offset);
        sample.persistentControlPoint(1).setIntensity(1.0f);

		// Check if we reached the edge of the line width and reverse direction if so
		if (fabs(my_modulated_point->offset) > (my_modulated_point->line_length / 2.0))
		{
			my_modulated_point->direction = -my_modulated_point->direction;
		}

        // Add the interval to calculate the sample offset for the next point in time
        my_modulated_point->offset += (interval_offset * my_modulated_point->direction);
    }

}

int main(int argc, char *argv[])
{
    // Create a time point streaming emitter and a leap controller
    Ultrahaptics::TimePointStreaming::Emitter emitter;
    Leap::Controller leap_control;

    // Set the leap motion to listen for background frames.
    leap_control.setPolicyFlags(Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);

    // We will be using 2 control points
    emitter.setMaximumControlPointCount(2);

    // Create a structure containing our control point data
    // Also pass the Leap conversion class the appropriate alignment for the device we're using
    ModulatedPoint point(emitter.getDeviceInfo().getDefaultAlignment());

    // Update our structure with data from our Leap listener
    leap_control.addListener(point.hand);

    // Set the length of the lines to draw
    point.line_length = 5.0 * Ultrahaptics::Units::centimetres;

    // Set how many times the lines are drawn every second
    point.line_draw_frequency = 100.0;

    // Set the callback function to the callback written above
    emitter.setEmissionCallback(my_emitter_callback, &point);

    // Start the array
    emitter.start();

    // Wait for enter key to be pressed.
    std::cout << "Hit ENTER to quit..." << std::endl;
    std::string line;
    std::getline(std::cin, line);

    // Stop the array
    emitter.stop();

    return 0;
}
