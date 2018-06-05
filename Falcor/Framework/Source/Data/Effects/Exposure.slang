#ifndef _FALCOR_EXPOSURE_H_
#define _FALCOR_EXPOSURE_H_

#include "HostDeviceData.h"

// The two functions below were based on code and explanations provided by Padraic Hennessy (@PadraicHennessy).
// See this for more info: https://placeholderart.wordpress.com/2014/11/21/implementing-a-physically-based-camera-manual-exposure/

/*
* Get an exposure using the Saturation-based Speed method.
*/
float getSaturationBasedExposure(float aperture,
                                 float shutterSpeed,
                                 float iso)
{
    float l_max = (7800.0f / 65.0f) * (aperture * aperture) / (iso * shutterSpeed);
    return 1.0f / l_max;
}
 
/*
* Get an exposure using the Standard Output Sensitivity method.
* Accepts an additional parameter of the target middle grey.
*/
float getStandardOutputBasedExposure(float aperture,
                                     float shutterSpeed,
                                     float iso,
                                     float middleGrey = 0.18f)
{
    float l_avg = (1000.0f / 65.0f) * (aperture * aperture) / (iso * shutterSpeed);
    return middleGrey / l_avg;
}

float computeEV100(float aperture, float shutterSpeed, float ISO)
{
    // EV number is defined as:
    // 2^EV_s = N^2 / t and EV_s = EV_100 + log2(S /100)
    // This gives
    // EV_s = log2(N^2 / t)
    // EV_100 + log2(S /100) = log2(N^2 / t)
    // EV_100 = log2(N^2 / t) - log2(S /100)
    // EV_100 = log2(N^2 / t * 100 / S)
    return log2((aperture * aperture) / shutterSpeed * 100 / ISO);
}

float computeEV100FromAvgLuminance(float avgLuminance)
{
    // We later use the middle gray at 12.7% in order to have
    // a middle gray at 18% with a sqrt (2) room for specular highlights
    // But here we deal with the spot meter measuring the middle gray
    // which is fixed at 12.5 for matching standard camera
    // constructor settings (i.e. calibration constant K = 12.5)
    // Reference : http://en.wikipedia.org/wiki/Film_speed
    return log2(avgLuminance * 100.0f / 12.5f);
}

float convertEV100ToLuminance(float EV100)
{
    return exp2(EV100) * 12.5f / 100.0f;
}

float convertEV100ToExposure_SBS(float EV100)
{
    // Compute the maximum luminance possible with H_sbs sensitivity
    // maxLum = 78 / ( S * q ) * N^2 / t
    // = 78 / ( S * q ) * 2^ EV_100
    // = 78 / (100 * 0.65) * 2^ EV_100
    // = 1.2 * 2^ EV
    // Reference : http://en.wikipedia.org/wiki/Film_speed
    float maxLuminance = 1.2f * pow(2.0f, EV100);
    return 1.0f / maxLuminance;
}

float calcExposure(CameraSettings cameraSettings, float avgLuminance, out float EV100)
{
    float exposure = 0.0f;

    if (cameraSettings.exposureMode == ExposureMode_Manual_SBS)
    {
        exposure = getSaturationBasedExposure(cameraSettings.aperture, cameraSettings.shutterSpeed, cameraSettings.ISO);
        EV100 = computeEV100(cameraSettings.aperture, cameraSettings.shutterSpeed, cameraSettings.ISO);
    }
    else if (cameraSettings.exposureMode == ExposureMode_Manual_SOS)
    {
		float middleGrey = 0.18f;
        exposure = getStandardOutputBasedExposure(cameraSettings.aperture, cameraSettings.shutterSpeed, cameraSettings.ISO, middleGrey);
        EV100 = computeEV100(cameraSettings.aperture, cameraSettings.shutterSpeed, cameraSettings.ISO);
    }
    else
    {
        float autoEV100 = computeEV100FromAvgLuminance(avgLuminance);
        autoEV100 = autoEV100 - cameraSettings.exposureCompensation;
        exposure = convertEV100ToExposure_SBS(autoEV100);
        EV100 = autoEV100;
    }

    return exposure;
}

// Determines the color based on exposure settings
float3 getExposedColor(CameraSettings cameraSettings, float3 color, float avgLuminance, out float exposure, out float EV100)
{
    exposure = calcExposure(cameraSettings, avgLuminance, EV100);
    return exposure * color;
}

#endif	// _FALCOR_EXPOSURE_H_