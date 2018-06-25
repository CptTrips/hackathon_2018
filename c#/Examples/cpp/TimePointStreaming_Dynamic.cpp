// This example uses the Timepoint Streaming emitter and a Leap Motion Controller.
// It projects a circle on to the hand and varies its radius depending on the height of the hand.

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

#include "WaveformData.hpp"
#include "UltrahapticsTimePointStreaming.hpp"

using Seconds = std::chrono::duration<float>;

static auto start_time = std::chrono::steady_clock::now();

// Structure to represent output from the Leap listener
struct LeapOutput
{
    LeapOutput() noexcept {}

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
        const Leap::Frame frame = controller.frame();
        const Leap::HandList hands = frame.hands();
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

            // Convert to Ultrahaptics vectors, normal is negated as leap normal points down.
            Ultrahaptics::Vector3 uh_palm_position(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);

            // Convert from leap space to device space.
            Ultrahaptics::Vector3 device_palm_position = alignment.fromTrackingPositionToDevicePosition(uh_palm_position);

            local_hand_data.palm_position = device_palm_position;
            local_hand_data.hand_present = true;
        }
        atomic_local_hand_data.store(local_hand_data);
    }

    LeapOutput getLeapOutput()
    {
        return atomic_local_hand_data.load();
    }

private:
    std::atomic <LeapOutput> atomic_local_hand_data;
    Ultrahaptics::Alignment alignment;
};

// Structure for passing information on the type of point to create
struct Circle
{
    // Create the structure, passing the appropriate alignment through to the LeapListening class
    Circle(const Ultrahaptics::Alignment& align) : hand(align)
    {
    }

    // Hand data
    LeapListening hand;

    // The position of the control point
    Ultrahaptics::Vector3 position;

    // the intensity of the control point
    float intensity;

    // The radius of the inner circle
    float minimum_radius;

    // The radius of the outer circle
    float maximum_radius;

    // Height at which radius begins to change
    float bottom_height;

    // Height after which radius no longer changes
    float top_height;

    // The frequency at which the control point goes around the circle of large radius
    float low_frequency;

    // The frequency at which the control point goes around the circle of small radius
    float high_frequency;


    const Ultrahaptics::Vector3 evaluateAt(Seconds t){

        // Get a copy of the Leap data
        LeapOutput leapOutput = hand.getLeapOutput();

        if(!leapOutput.hand_present){
            intensity = 0;
            return Ultrahaptics::Vector3(0.0f, 0.0f, 0.0f);
        }

        intensity = 1;

        const float height = leapOutput.palm_position.z;

        //Set the radius variation parameters
        const float rad_gradient   = (minimum_radius - maximum_radius)/
                                       (top_height - bottom_height);

        const float rad_intercept  = (maximum_radius * top_height -
                                       minimum_radius * bottom_height)/
                                      (top_height - bottom_height);

        //Set the frequency variation parameters
        const float freq_gradient  = top_height * bottom_height * (low_frequency - high_frequency)/
                                      (top_height - bottom_height);

        const float freq_intercept = (low_frequency * bottom_height -
                                       high_frequency * top_height)/
                                      (bottom_height - top_height);

        float radius, frequency;
        //Set the radius and frequency dependent on the hand height
        if(height < bottom_height)
        {
            radius = maximum_radius;
            frequency = low_frequency;
        }
        else if(height >= bottom_height && height <= top_height)
        {
            radius = rad_gradient * height + rad_intercept;
            frequency = freq_gradient / height + freq_intercept;
        }
        else
        {
            radius = minimum_radius;
            frequency = high_frequency;
        }

        // Calculate the x and y positions of the circle and set the height
        position.x = std::cos(2 * M_PI * frequency * t.count()) * radius;
        position.y = std::sin(2 * M_PI * frequency * t.count()) * radius;
        position.z = height;
	return position;
    }
};

// Callback function for filling out complete device output states through time
void my_emitter_callback(const Ultrahaptics::TimePointStreaming::Emitter &timepoint_emitter,
                         Ultrahaptics::TimePointStreaming::OutputInterval &interval,
                         const Ultrahaptics::HostTimePoint &submission_deadline,
                         void *user_pointer)
{
    // Cast the user pointer to the struct that describes the control point behaviour
    Circle *circle = static_cast<Circle*>(user_pointer);

    // Loop through time, setting control points
    for (auto& sample : interval)
    {
        const Seconds t = sample - start_time;
        const Ultrahaptics::Vector3 position = circle->evaluateAt(t);

        sample.persistentControlPoint(0).setPosition(position);
        sample.persistentControlPoint(0).setIntensity(circle->intensity);
    }
}

int main(int argc, char *argv[])
{
    // Create a time point streaming emitter and a leap controller
    Ultrahaptics::TimePointStreaming::Emitter emitter;
    Leap::Controller leap_control;

    // Set the leap motion to listen for background frames.
    leap_control.setPolicyFlags(Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);

    // We will be using 1 control point
    emitter.setMaximumControlPointCount(1);

    // Create a structure containing our control point data
    // Also pass the Leap conversion class the appropriate alignment for the device we're using
    Circle circle(emitter.getDeviceInfo().getDefaultAlignment());

    // Update our structure with data from our Leap listener
    leap_control.addListener(circle.hand);

    // Set the lower limit of the radius of the circle that the point is traversing (m)
    circle.minimum_radius = 0.01f;  
    // Set the upper limit of the radius of the circle that the point is traversing (m)
    circle.maximum_radius = 0.04f;
    // Set the height at which radius begins to change (m)
    circle.bottom_height = 0.15f;
    // Set the height after which radius no longer changes (m)
    circle.top_height = 0.30f; 
    // Set how many times the point traverses the smallest circle every second
    circle.high_frequency = 100.0f;
    // Set how many times the point traverses the largest circle every second
    circle.low_frequency = 50.0f;

    // Set the callback function to the callback written above
    emitter.setEmissionCallback(my_emitter_callback, &circle);

    // Start the array
    emitter.start();

    // Wait for enter key to be pressed.
    std::cout << "Hit ENTER to quit..." << std::endl;
    std::string line;
    std::getline(std::cin, line);

    // Stop the array
    emitter.stop();

    // Stop asking for data from the Leap controller
    leap_control.removeListener(circle.hand);

    return 0;
}
