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

__import Helpers;

SamplerState gColorSampler;
Texture2D gColorTex;

float meteringFunc(float2 texC)
{
    return 1.0;
}

float4 main(float2 texC  : TEXCOORD,
            float4 posH  : SV_POSITION) : SV_TARGET0
{
    float texWidth, texHeight;
    gColorTex.GetDimensions(texWidth, texHeight);

    int2 iPos = int2(posH.xy);
    float2 coords[4] = {iPos * 2, iPos * 2 + int2(1, 0), iPos * 2 + int2(0, 1), iPos * 2 + int2(1, 1)};

    float logLuminance = 0;
    float weight = 0;
    float cnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (coords[i].x < texWidth && coords[i].y < texHeight)
        {
            float4 color = gColorTex[coords[i]];

#if defined _CALC_LOG2_LUMINANCE

            float luminance = max(0.0001, calcLuminance(color.rgb));
            float w = meteringFunc(coords[i] / (texWidth, texHeight));

            logLuminance += log2(luminance) * w;
            weight += w;
            cnt += 1;

#elif defined _LUMINANCE_REDUCTION

            float localLogLuma = color.r;
            float localWeight = color.g;
            if (localWeight > 0)
            {
                logLuminance += localLogLuma;
                weight += localWeight;
                cnt += 1;
            }

#else
#error nothing
#endif
        }
    }

    if (cnt > 0)
    {
        logLuminance /= cnt;
        weight /= cnt;
    }

    return float4(logLuminance * weight, weight, 0, 1);
}
