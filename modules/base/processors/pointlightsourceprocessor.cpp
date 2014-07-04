/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2013-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Main file author: Erik Sund�n
 *
 *********************************************************************************/

#include "pointlightsourceprocessor.h"

namespace inviwo {

ProcessorClassIdentifier(PointLightSourceProcessor,  "Point light source");
ProcessorDisplayName(PointLightSourceProcessor,  "Point light source");
ProcessorTags(PointLightSourceProcessor, Tags::None);
ProcessorCategory(PointLightSourceProcessor, "Light source");
ProcessorCodeState(PointLightSourceProcessor, CODE_STATE_EXPERIMENTAL);

PointLightSourceProcessor::PointLightSourceProcessor()
    : Processor()
    , outport_("PointLightSource")
    , lightPowerProp_("lightPower", "Light power (%)", 50.f, 0.f, 100.f)
    , lightSize_("lightSize", "Light size", vec2(1.5f, 1.5f), vec2(0.0f, 0.0f), vec2(3.0f, 3.0f))
    , lightDiffuse_("lightDiffuse", "Color", vec4(1.0f))
    , lightPosition_("lightPosition", "Light Source Position", vec3(1.f, 0.65f, 0.65f), vec3(-1.f), vec3(1.f))
    , lightEnabled_("lightEnabled", "Enabled", true)
    , camera_("camera", "Camera", vec3(0.0f, 0.0f, -2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)){
    addPort(outport_);
    addProperty(lightPosition_);
    addProperty(lightDiffuse_);
    addProperty(lightPowerProp_);
    addProperty(lightSize_);
    addProperty(lightEnabled_);
    addProperty(camera_);
    camera_.setVisible(false);
    // assign lighting properties to property group
    lightPosition_.setGroupID("lighting");
    lightDiffuse_.setGroupID("lighting");
    lightPowerProp_.setGroupID("lighting");
    lightSize_.setGroupID("lighting");
    lightEnabled_.setGroupID("lighting");
    Property::setGroupDisplayName("lighting", "Light Parameters");
    lightPosition_.setSemantics(PropertySemantics::LightPosition);
    lightDiffuse_.setSemantics(PropertySemantics::Color);
    lightSource_ = new PointLight();
    addInteractionHandler(new PointLightInteractationHandler(&lightPosition_, &camera_));
}

PointLightSourceProcessor::~PointLightSourceProcessor() {
    delete lightSource_;
    const std::vector<InteractionHandler*>& interactionHandlers = getInteractionHandlers();
    for(size_t i=0; i<interactionHandlers.size(); ++i) {
        InteractionHandler* handler = interactionHandlers[i];
        removeInteractionHandler(handler);
        delete handler;
    }
}

void PointLightSourceProcessor::process() {
    updatePointLightSource(lightSource_);
    outport_.setData(lightSource_, false);
}

void PointLightSourceProcessor::updatePointLightSource(PointLight* lightSource) {
    vec3 lightPos = vec3(0.5f) - lightPosition_.get();
    vec3 dir = glm::normalize(lightPos - camera_.getLookTo());
    mat4 transformationMatrix = getLightTransformationMatrix(lightPos, dir);
    lightSource->setObjectToTexture(transformationMatrix);
    lightSource->setSize(lightSize_.get());
    vec3 diffuseLight = lightDiffuse_.get().xyz();
    lightSource->setIntensity(lightPowerProp_.get()*diffuseLight);
    lightSource->setPosition(lightPosition_.get());
    lightSource->setEnabled(lightEnabled_.get());
}

PointLightSourceProcessor::PointLightInteractationHandler::PointLightInteractationHandler(FloatVec3Property* pl, CameraProperty* cam) 
    : InteractionHandler()
    , pointLight_(pl)
    , camera_(cam) {
}

void PointLightSourceProcessor::PointLightInteractationHandler::invokeEvent(Event* event){
    GestureEvent* gestureEvent = dynamic_cast<GestureEvent*>(event);
    if (gestureEvent) {
        if(gestureEvent->type() == GestureEvent::PAN && gestureEvent->numFingers() == 3){
            mat4 projectionMatrix = camera_->projectionMatrix();
            float clipW = projectionMatrix[2][3] / (-(projectionMatrix[2][2] / projectionMatrix[3][2]));
            vec4 clipCoords = vec4(((gestureEvent->screenPosNormalized()*2.f)-1.f) * clipW, 0.f, clipW);
            vec4 eyeCoords = camera_->inverseProjectionMatrix() * clipCoords;
            vec4 worldCoords = camera_->inverseViewMatrix() * eyeCoords;
            worldCoords /= worldCoords.w;
            pointLight_->set(worldCoords.xyz());
        }
        return;
    }
}

} // namespace
