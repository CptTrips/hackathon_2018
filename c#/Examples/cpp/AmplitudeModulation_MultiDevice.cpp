#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include <UltrahapticsAmplitudeModulation.hpp>

using namespace Ultrahaptics;

int main()
{
    // Width of the kit, refer to the User Guide of your device
    float width = 0.210f;  // meters
    // affine transformation matrix placing the device half its width to the left in the global space
    const auto left_tr = Ultrahaptics::Transform::translation(-width / 2, 0.0f, 0.0f);
    // affine transformation matrix placing the device half its width to the right in the global space
    const auto right_tr = Ultrahaptics::Transform::translation(width / 2, 0.0f, 0.0f);

    // Create a multi device AmplitudeModulation emitter
    // Please make sure to use the correct identifiers for your devices
    AmplitudeModulation::Emitter emitter("USX:USX-00000000", left_tr);
    emitter.addDevice("USX:USX-00000001", right_tr);

    // Set frequency to 200 Hertz and maximum intensity
    float frequency = 200.0 * Units::hertz;
    float intensity = 1.0f;

    bool leftArray = true;
    for (;;)
    {
        Vector3 position = leftArray ? Vector3{-0.10f, 0.05f, 0.12f}
                                     : Vector3{ 0.10f, 0.05f, 0.12f};
        AmplitudeModulation::ControlPoint point(position, intensity, frequency);

        emitter.update(point);

        leftArray = !leftArray;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2s);
    }

    return 0;
}
