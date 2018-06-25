// This example uses the Time Point Streaming emitter and a Leap Motion Controller.
// It reads in a WAV file and projects a point focussed onto the height
// of the palm. The intensity of the point is modulated with the WAV data.

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
            // Get the data for the first hand
            Leap::Hand hand = hands[0];

            // Translate the hand position from leap objects to Ultrahaptics objects.
            Leap::Vector leap_palm_position = hand.palmPosition();

            // Convert to Ultrahaptics vectors, normal is negated as leap normal points down.
            Ultrahaptics::Vector3 uh_palm_position(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);

            // Convert to device space from leap space.
            Ultrahaptics::Vector3 device_palm_position = alignment.fromTrackingPositionToDevicePosition(uh_palm_position);

            // Update the hand data with the current information
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
struct ModulatedPoint
{
    // Create the structure, passing the appropriate alignment through to the LeapListening class
    ModulatedPoint(const Ultrahaptics::Alignment& align) : hand(align)
    {
    }

    // Hand data
    LeapListening hand;

    // The freeform wave to output
    WaveformData wavedata;

    // Sample number
    size_t sample_number;
};

// Callback function for filling out complete device output states through time
void my_emitter_callback(const Ultrahaptics::TimePointStreaming::Emitter &timepoint_emitter,
                         Ultrahaptics::TimePointStreaming::OutputInterval &interval,
                         const Ultrahaptics::HostTimePoint &submission_deadline,
                         void *user_pointer)
{
    // Cast the user pointer to the struct that describes the control point behaviour
    ModulatedPoint *my_modulated_point = static_cast<ModulatedPoint*>(user_pointer);

    // Get a copy of the hand data.
    LeapOutput leapOutput = my_modulated_point->hand.getLeapOutput();

    // Loop through time, setting control point data
    for (auto& sample : interval)
    {
        if (!leapOutput.hand_present) {
            sample.persistentControlPoint(0).setIntensity(0.0f);
            continue;
        }
        // Project the control point onto the palm
        sample.persistentControlPoint(0).setPosition(leapOutput.palm_position);

        // Set the intensity of the point using the waveform. If the hand is not present, intensity is 0.
        float control_point_intensity = my_modulated_point->wavedata.at(my_modulated_point->sample_number);
        sample.persistentControlPoint(0).setIntensity(control_point_intensity);

        // Increment sample count
        my_modulated_point->sample_number++;
    }
}

int main(int argc, char *argv[])
{
    // Attempt to open an input file as an argument.
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <filename.wav>" << std::endl;
        return 1;
    }
    std::cout << "Opening file '" << argv[1] << "'..." << std::endl;

    // Create a time point streaming emitter and a leap controller
    Ultrahaptics::TimePointStreaming::Emitter emitter;
    Leap::Controller leap_control;

    // Create a structure containing our control point data and fill it in from the file.
    // Also pass the Leap conversion class the appropriate alignment for the device we're using
    ModulatedPoint point(emitter.getDeviceInfo().getDefaultAlignment());

    // Read the input waveform from file and check its status.
    point.wavedata = WaveformData(argv[1]);
    if (!point.wavedata.isWaveformLoaded())
    {
        std::cout << "Failed to load waveform. Quitting...\n";
        return -1;
    }

    // Set the leap motion to listen for background frames.
    leap_control.setPolicyFlags(Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);

    // Update our structure with data from our Leap listener
    leap_control.addListener(point.hand);

    // Get the achievable sample_rate for the number of control points we are producing
    unsigned int hardware_sample_rate = emitter.setMaximumControlPointCount(1);

    // Resample waveform to device sample rate
    point.wavedata.resample(hardware_sample_rate);
    point.sample_number = 0;

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
