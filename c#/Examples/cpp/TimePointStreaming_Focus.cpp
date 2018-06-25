// This example uses the Timepoint Streaming emitter to project a static control
// point 20cm above the centre of the array. The point is modulated using a
// cosine

#include <cmath>
#include <iostream>
#include <string>

#include "UltrahapticsTimePointStreaming.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

using Seconds = std::chrono::duration<float>;

static auto start_time = std::chrono::steady_clock::now();

// Structure for passing information on the type of point to create
struct ModulatedPoint
{
    // The position of the control point
    Ultrahaptics::Vector3 position;

    // The intensity of the control point
    double modulation_intensity;

    // The frequency at which the control point modulates in amplitude
    double modulation_frequency;

    void evaluateAt(Seconds t){
        modulation_intensity = (1.0 - std::cos(2 * M_PI * modulation_frequency * t.count())) * 0.5;
    }
};

// Callback function for filling out complete device output states through time
void my_emitter_callback(const Ultrahaptics::TimePointStreaming::Emitter &timepoint_emitter,
                         Ultrahaptics::TimePointStreaming::OutputInterval &interval,
                         const Ultrahaptics::HostTimePoint &submission_deadline,
                         void *user_pointer)
{
    // Cast the user pointer to the struct that describes the control point behaviour
    ModulatedPoint *my_modulated_point = static_cast<ModulatedPoint*>(user_pointer);

    // Loop through the samples in this interval
    for (auto& sample : interval)
    {
        Seconds t = sample - start_time;

        my_modulated_point->evaluateAt(t);
        
        // Set the position and intensity of the persistent control point to that of the modulated wave at this point in time.
        sample.persistentControlPoint(0).setPosition(my_modulated_point->position);
        sample.persistentControlPoint(0).setIntensity(my_modulated_point->modulation_intensity);
    }
}

int main(int argc, char *argv[])
{
    // Create a time point streaming emitter
    Ultrahaptics::TimePointStreaming::Emitter emitter;

    // Set the maximum control point count
    emitter.setMaximumControlPointCount(1);

    // Create a structure containing our control point data and fill it in
    struct ModulatedPoint point;

    // Set control point to 20cm above the centre of the array
    point.position = Ultrahaptics::Vector3(0.0f, 0.0f, 20.0f * Ultrahaptics::Units::centimetres);
    // Set the amplitude of the modulation of the wave to one (full modulation depth)
    point.modulation_intensity = 1.0;
    // Set the frequency of the amplitude modulation
    point.modulation_frequency = 200.0 * Ultrahaptics::Units::hertz;

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
