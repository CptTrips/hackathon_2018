// This example uses the Time Point Streaming emitter
// It reads in a WAV file, and projects a static point 20cm above the centre of the array
// The point is modulated with the WAV data.

#include <cfloat>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>

#include "UltrahapticsTimePointStreaming.hpp"
#include "WaveformData.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

// Structure type for passing information on the type of point to create
struct ModulatedPoint
{
    Ultrahaptics::Vector3 position;
    WaveformData wavedata;

    size_t sample_number;
    float modulation_intensity;
};

// Callback function for filling out complete device output states through time
void my_emitter_callback(const Ultrahaptics::TimePointStreaming::Emitter &timepoint_emitter,
                         Ultrahaptics::TimePointStreaming::OutputInterval &interval,
                         const Ultrahaptics::HostTimePoint &submission_deadline,
                         void *user_ptr)
{

    // Cast the user pointer to the struct that describes the control point behaviour
    ModulatedPoint *user_data = static_cast<ModulatedPoint*>(user_ptr);

    // Loop through time, setting the intensity of the control point to that of a wave that has been amplitude modulated with the filtered wave.
    for (auto& sample : interval)
    {
        sample.persistentControlPoint(0).setPosition(user_data->position);
        sample.persistentControlPoint(0).setIntensity(user_data->modulation_intensity * user_data->wavedata.at(user_data->sample_number));

        // Increment sample count
        user_data->sample_number++;
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

    // Create a structure containing our control point data and fill it in from the file.
    ModulatedPoint point;

    // Read the input waveform from file and check its status.
    point.wavedata = WaveformData(argv[1]);
    if (!point.wavedata.isWaveformLoaded())
    {
        std::cout << "Failed to load waveform. Quitting...\n";
        return -1;
    }

    // Create a time point streaming emitter
    Ultrahaptics::TimePointStreaming::Emitter emitter;

    // Get the achievable sample_rate for the number of control points we are producing
    unsigned int hardware_sample_rate = emitter.setMaximumControlPointCount(1);

    // Resample the input waveform to device sample rate. This is dependent on the negotiated communication rate.
    point.wavedata.resample(hardware_sample_rate);
    point.sample_number = 0;

    // Set control point to 20cm above the centre of the array with maximum intensity.
    point.position = Ultrahaptics::Vector3(0.0f, 0.0f, 20.0f * Ultrahaptics::Units::centimetres);
    point.modulation_intensity = 1.0f;

    // Set the callback function to the callback written above
    emitter.setEmissionCallback(my_emitter_callback, &point);

    // Start the array
    emitter.start();

    // Wait for user input to exit.
    std::cout << "Hit ENTER to quit..." << std::endl;
    std::string line;
    std::getline(std::cin, line);

    // Stop the array
    emitter.stop();

    return 0;
}
