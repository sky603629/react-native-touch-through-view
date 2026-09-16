/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TouchThroughViewComponentInstance.h"

#include "TouchThroughTrace.h"
#include "RNOH/arkui/NativeNodeApi.h"
#include <stdexcept>

namespace rnoh {

namespace {

void disableHitTesting(StackNode& node) {
  // Set the native attribute directly: some RNOH versions leave setHitTestMode empty.
  ArkUI_NumberValue value[] = {{.i32 = ARKUI_HIT_TEST_MODE_NONE}};
  ArkUI_AttributeItem item = {.value = value, .size = 1};
  auto status = NativeNodeApi::getInstance()->setAttribute(
      node.getArkUINodeHandle(), NODE_HIT_TEST_BEHAVIOR, &item);
  if (status != 0) {
    throw std::runtime_error("Failed to set TouchThroughView hit test mode");
  }
}

} // namespace

TouchThroughViewComponentInstance::TouchThroughViewComponentInstance(Context context)
    : BaseR2RTouchThroughViewComponentInstance(context),
      m_stackNode(context.arkUINodeContext) {
  disableHitTesting(m_stackNode);
}

void TouchThroughViewComponentInstance::onCreate() {
  BaseR2RTouchThroughViewComponentInstance::onCreate();
  touchthroughtrace::logLifecycle(
      "markerOnCreate",
      0,
      getTag(),
      getComponentName(),
      m_stackNode.getArkUINodeHandle());
  touchthroughtrace::logMarkerState(
      "markerHitTestInit", getTag(), getComponentName(), "NONE");
}

void TouchThroughViewComponentInstance::onChildInserted(
    ComponentInstance::Shared const& childComponentInstance,
    std::size_t index) {
  BaseR2RTouchThroughViewComponentInstance::onChildInserted(
      childComponentInstance, index);
  m_stackNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
}

void TouchThroughViewComponentInstance::onChildRemoved(
    ComponentInstance::Shared const& childComponentInstance) {
  BaseR2RTouchThroughViewComponentInstance::onChildRemoved(
      childComponentInstance);
  m_stackNode.removeChild(childComponentInstance->getLocalRootArkUINode());
}

StackNode& TouchThroughViewComponentInstance::getLocalRootArkUINode() {
  return m_stackNode;
}

void TouchThroughViewComponentInstance::onPropsChanged(
    SharedConcreteProps const& props) {
  BaseR2RTouchThroughViewComponentInstance::onPropsChanged(props);
  disableHitTesting(m_stackNode);
  touchthroughtrace::logMarkerState(
      "markerPropsApplied", getTag(), getComponentName(), "NONE");
}

} // namespace rnoh
