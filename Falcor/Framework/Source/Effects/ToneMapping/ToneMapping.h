/***************************************************************************
# Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#pragma once
#include "Graphics/FullScreenPass.h"
#include "API/ConstantBuffer.h"
#include "API/FBO.h"
#include "API/Sampler.h"
#include "Utils/Gui.h"
#include "Data/HostDeviceData.h"

namespace Falcor
{
    /** Tone-mapping effect
    */
    class ToneMapping
    {
    public:
        using UniquePtr = std::unique_ptr<ToneMapping>;
        /** Destructor
        */
        ~ToneMapping();

        /** The tone-mapping operator to use
        */
        enum class Operator
        {
            Clamp,              ///< Clamp to [0, 1]. Just like LDR
            Linear,             ///< Linear mapping
            Reinhard,           ///< Reinhard operator
            ReinhardModified,   ///< Reinhard operator with maximum white intensity
            HejiHableAlu,       ///< John Hable's ALU approximation of Jim Heji's filmic operator
            HableUc2,           ///< John Hable's filmic tone-mapping used in Uncharted 2
            Aces,               ///< Aces Filmic Tone-Mapping
        };

        enum class ShutterSpeed
        {
            ShutterSpeed1Over1 = 0,
            ShutterSpeed1Over2,
            ShutterSpeed1Over4,
            ShutterSpeed1Over8,
            ShutterSpeed1Over15,
            ShutterSpeed1Over30,
            ShutterSpeed1Over60,
            ShutterSpeed1Over125,
            ShutterSpeed1Over250,
            ShutterSpeed1Over500,
            ShutterSpeed1Over1000,
            ShutterSpeed1Over2000,
            ShutterSpeed1Over4000,
        };

        enum class FStop
        {
            FStop1Point8 = 0,
            FStop2Point0,
            FStop2Point2,
            FStop2Point5,
            FStop2Point8,
            FStop3Point2,
            FStop3Point5,
            FStop4Point0,
            FStop4Point5,
            FStop5Point0,
            FStop5Point6,
            FStop6Point3,
            FStop7Point1,
            FStop8Point0,
            FStop9Point0,
            FStop10Point0,
            FStop11Point0,
            FStop13Point0,
            FStop14Point0,
            FStop16Point0,
            FStop18Point0,
            FStop20Point0,
            FStop22Point0,
        };

        enum class ISORating
        {
            ISO100 = 0,
            ISO200,
            ISO400,
            ISO800,
        };

        /** Create a new object
        */
        static UniquePtr create(Operator op);

        /** Render UI elements
            \param[in] pGui GUI instance to render UI with
            \param[in] uiGroup Name for the group to render UI elements within
        */
        void renderUI(Gui* pGui, const char* uiGroup);

        /** Run the tone-mapping program
            \param pRenderContext Render-context to use
            \param pSrc The source FBO
            \param pDst The destination FBO
        */
        void execute(RenderContext* pRenderContext, Fbo::SharedPtr pSrc, Fbo::SharedPtr pDst);

        /** Set a new operator. Triggers shader recompilation if operator has not been set on this instance before.
        */
        void setOperator(Operator op);

        /** Sets the middle-gray luminance used for normalizing each pixel's luminance. 
            Middle gray is usually in the range of [0.045, 0.72].
            Lower values maximize contrast. Useful for night scenes.
            Higher values minimize contrast, resulting in brightly lit objects.
        */
        void setExposureKey(float exposureKey);

        /** Sets the maximal luminance to be consider as pure white.
            Only valid if the operator is ReinhardModified
        */
        void setWhiteMaxLuminance(float maxLuminance);

        /** Sets the luminance texture LOD to use when fetching average luminance values.
            Lower values will result in a more localized effect
        */
        void setLuminanceLod(float lod);

        /** Sets the white-scale used in Uncharted 2 tone mapping.
        */
        void setWhiteScale(float whiteScale);

    private:
        ToneMapping(Operator op);
        void createLuminanceFbo(Fbo::SharedPtr pSrcFbo);

        Operator mOperator;
        FullScreenPass::UniquePtr mpToneMapPass;
        FullScreenPass::UniquePtr mpLuminancePass;
        FullScreenPass::UniquePtr mpLuminanceReductionPass;
        std::vector<Fbo::SharedPtr> mpLuminanceFbo;
        GraphicsVars::SharedPtr mpToneMapVars;
        GraphicsVars::SharedPtr mpLuminanceVars;
        GraphicsVars::SharedPtr mpLuminanceReductionVars;
        ConstantBuffer::SharedPtr mpToneMapCBuffer;
        Sampler::SharedPtr mpPointSampler;
        Sampler::SharedPtr mpLinearSampler;

        StructuredBuffer::SharedPtr mpExposureStats;

        struct PassBindLocations
        {
            ParameterBlockReflection::BindLocation luminanceSampler;
            ParameterBlockReflection::BindLocation colorSampler;
            ParameterBlockReflection::BindLocation colorTex;
            ParameterBlockReflection::BindLocation luminanceTex;
            ParameterBlockReflection::BindLocation statsBuffer;
        } mBindLocations;

        struct
        {
            float exposureKey = 0.042f;
            float whiteMaxLuminance = 1.0f;
            float whiteScale = 11.2f;
            float dummy = 0.0f;;

            CameraSettings camSettings;
        } mConstBufferData;

        uint32_t mExposureMode;
        ShutterSpeed mShutterSpeed;
        FStop mAperture;
        ISORating mISO;
        float mExposureCompensation = 0.0f;

        void createToneMapPass(Operator op);
        void createLuminancePass();
    };
}