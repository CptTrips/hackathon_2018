// This example uses the Timepoint Streaming emitter with two devices to project two alternating
// static control point 20cm above the middle of both arrays.
// The points are modulated using a cosine

#include <cmath>
#include <chrono>
#include <iostream>
#include <string>

#include "UltrahapticsTimePointStreaming.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

using namespace std::chrono_literals;
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

// Structure for passing information on the type of point to create
struct ModulatedPoint
{
    // The position of the control point
    Ultrahaptics::Vector3 positionL;
    Ultrahaptics::Vector3 positionR;

    // The intensity of the control point
    double modulation_intensity;

    // The frequency at which the control point modulates in amplitude
    double modulation_frequency;

    // The waveform's current phase
    double phase;
    bool leftArray = true;
};

// Callback function for filling out complete device output states through time
void my_emitter_callback(const Ultrahaptics::TimePointStreaming::Emitter &timepoint_emitter,
                         Ultrahaptics::TimePointStreaming::OutputInterval &interval,
                         const Ultrahaptics::HostTimePoint &submission_deadline,
                         void *user_pointer)
{
    // Cast the user pointer to the struct that describes the control point behaviour
    ModulatedPoint *my_modulated_point = static_cast<ModulatedPoint*>(user_pointer);

    // Set phase angle interval between control point samples
    const double phase_interval = 2.0 * M_PI * my_modulated_point->modulation_frequency / timepoint_emitter.getSampleRate();

    // Loop through the samples in this interval
    const auto position = my_modulated_point->leftArray ? my_modulated_point->positionL : my_modulated_point->positionR;
    for (auto& sample : interval)
    {
        const auto position = my_modulated_point->leftArray ? my_modulated_point->positionL : my_modulated_point->positionR;
        // Set the position and intensity of the persistent control point to that of the modulated wave at this point in time.
        sample.persistentControlPoint(0).setPosition(position);
        sample.persistentControlPoint(0).setIntensity(my_modulated_point->modulation_intensity * (1.0 - std::cos(my_modulated_point->phase)) * 0.5);

        // Update the phase
        my_modulated_point->phase += phase_interval;
    }

    static auto t = Clock::now();
    if (Clock::now() - t < 2s == 0) {
        my_modulated_point->leftArray = !my_modulated_point->leftArray;
        t = Clock::now();
    }

    // Keep within 2PI range to avoid any floating point issues.
    my_modulated_point->phase = std::fmod(my_modulated_point->phase, 2. * M_PI);
}

int main(int argc, char *argv[])
{
    // Width of the kit, refer to the User Guide of your device
    float width = 0.210f;  // meters
    // affine transformation matrix placing the device half its width to the left in the global space
    const auto left_tr = Ultrahaptics::Transform::translation(-width / 2, 0.0f, 0.0f);
    // affine transformation matrix placing the device half its width to the right in the global space
    const auto right_tr = Ultrahaptics::Transform::translation(width / 2, 0.0f, 0.0f);

    // Create a multi device time point streaming emitter
    // Please make sure to use the correct identifiers for your devices
    Ultrahaptics::TimePointStreaming::Emitter emitter("USX:USX-00000000", left_tr);
    emitter.addDevice("USX:USX-00000001", right_tr);

    // Set the maximum control point count
    emitter.setMaximumControlPointCount(1);

    // Create a structure containing our control point data and fill it in
    struct ModulatedPoint point;

    // Set two control points to 20cm above the middle of both arrays and 5 cm to each side
    point.positionL = Ultrahaptics::Vector3{-0.05f, 0.00f, 0.20f};
    point.positionR = Ultrahaptics::Vector3{ 0.05f, 0.00f, 0.20f};
    // Set the amplitude of the modulation of the wave to one (full modulation depth)
    point.modulation_intensity = 1.0;
    // Set the frequency of the amplitude modulation
    point.modulation_frequency = 200.0 * Ultrahaptics::Units::hertz;
    // This is the initial phase
    point.phase = 0.0;

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
