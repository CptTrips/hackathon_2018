// This example uses the Timepoint Streaming emitter to project a static control
// point 20cm above the centre of the array. The point is modulated using a
// cosine

#include <cmath>
#include <iostream>
#include <vector>
#include <cstddef>
#include <string>

#include "UltrahapticsTimePointStreaming.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

// Structure for passing information over time for our control point
struct UserData
{
    explicit UserData(unsigned int timepointCount)
        : timepoints(std::vector<Ultrahaptics::TimePointStreaming::ControlPoint>(timepointCount))
    {}

    std::size_t next_index = 0;
    std::vector<Ultrahaptics::TimePointStreaming::ControlPoint> timepoints;
};

// Callback function for filling out complete device output states through time
void my_emitter_callback(const Ultrahaptics::TimePointStreaming::Emitter &timepoint_emitter,
                         Ultrahaptics::TimePointStreaming::OutputInterval &interval,
                         const Ultrahaptics::HostTimePoint &submission_deadline,
                         void *user_data_ptr)
{

    // The user pointer allows us to store data we can re-use in our callback.
    // In this case, we've used a custom struct that describes the precomputed control point
    // data for the number of samples required to represent a complete period of our waveform.
    UserData *user_data = static_cast<UserData*>(user_data_ptr);

    // Iterate through all of the time points in the given output interval.
    // We only have one control point, and at each point in time we are updating the control
    // point to the next precomputed state.
    // This outputs a continuous smooth waveform on the array.
    for (auto& sample : interval)
    {
        // Update the control point at this time point with our precomputed value
        sample.persistentControlPoint(0) = user_data->timepoints[user_data->next_index];

        // Update our custom struct so we know which control point we are going to use for the
        // next time point. Because we have precomputed one period of the waveform, we wrap
        // around to the start of our precomputed vector of control points.
        user_data->next_index = (user_data->next_index + 1) % user_data->timepoints.size();
    }
}

int main(int argc, char *argv[])
{
    // Create a time point streaming emitter
    Ultrahaptics::TimePointStreaming::Emitter emitter;

    // Get the achievable sample_rate for the number of control points we are producing
    // and work out how many timepoints we need for 200Hz output.
    unsigned int hardware_sample_rate = emitter.setMaximumControlPointCount(1);
    unsigned int timepoint_count = hardware_sample_rate / (200.0 * Ultrahaptics::Units::hertz);

    UserData user_data(timepoint_count);

    // Set control point to 20cm above the centre of the array.
    Ultrahaptics::Vector3 position(0.0f, 0.0f, 20.0f * Ultrahaptics::Units::centimetres);

    // Modulate the intensity to be a complete cosine waveform over the full set of points.
    for (size_t i = 0; i < timepoint_count; i++)
    {
        float intensity = 0.5 * (1.0 - std::cos((2.0 * M_PI * i) / timepoint_count));
        user_data.timepoints[i].setPosition(position);
        user_data.timepoints[i].setIntensity(intensity);
    }

    // Set the callback function to the callback written above
    emitter.setEmissionCallback(my_emitter_callback, &user_data);

    // Start the array
    emitter.start();

    // Wait for user input to quit.
    std::cout << "Hit ENTER to quit..." << std::endl;
    std::string line;
    std::getline(std::cin, line);

    // Stop the array
    emitter.stop();

    return 0;
}
