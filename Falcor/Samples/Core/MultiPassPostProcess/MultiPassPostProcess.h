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
#include "Falcor.h"
#include "SampleTest.h"

using namespace Falcor;

class MultiPassPostProcess : public Renderer
{
public:
    void onLoad(SampleCallbacks* pSample, RenderContext::SharedPtr pContext) override;
    void onFrameRender(SampleCallbacks* pSample, RenderContext::SharedPtr pContext, Fbo::SharedPtr pTargetFbo) override;
    void onShutdown(SampleCallbacks* pSample) override;
    bool onKeyEvent(SampleCallbacks* pSample, const KeyboardEvent& keyEvent) override;
    void onGuiRender(SampleCallbacks* pSample, Gui* pGui) override;
private:
    Texture::SharedPtr mpImage;
    Fbo::SharedPtr mpTempFB;

    FullScreenPass::UniquePtr mpLuminance;
    GaussianBlur::UniquePtr mpGaussianBlur;
    FullScreenPass::UniquePtr mpBlit;
    GraphicsVars::SharedPtr mpProgVars;

    bool mEnableGaussianBlur = false;
    bool mEnableGrayscale = false;
    void loadImage(SampleCallbacks* pSample);
    void loadImageFromFile(SampleCallbacks* pSample, std::string filename);
    
    //testing 
    void onInitializeTesting(SampleCallbacks* pSample) override;
};