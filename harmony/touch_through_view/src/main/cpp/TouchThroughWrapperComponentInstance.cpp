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

#include "TouchThroughWrapperComponentInstance.h"

#include <arkui/native_node.h>
#include <arkui/ui_input_event.h>
#include <glog/logging.h>
#include <RNOHCorePackage/ComponentInstances/ScrollViewComponentInstance.h>
#include "TouchThroughTrace.h"
#include "TouchThroughViewComponentInstance.h"

namespace rnoh {

namespace {

bool isPointInsideNodeBounds(
    ArkUI_NodeHandle nodeHandle,
    float windowX,
    float windowY) {
  ArkUI_IntOffset offset {0, 0};
  ArkUI_IntSize size {0, 0};
  if (OH_ArkUI_NodeUtils_GetLayoutPositionInWindow(nodeHandle, &offset) != 0) {
    return false;
  }
  if (OH_ArkUI_NodeUtils_GetLayoutSize(nodeHandle, &size) != 0) {
    return false;
  }
  return windowX >= offset.x && windowX <= offset.x + size.width &&
      windowY >= offset.y && windowY <= offset.y + size.height;
}

bool isNativeScrollLikeComponent(
    ComponentInstance::Shared const& componentInstance) {
  if (componentInstance == nullptr) {
    return false;
  }

  if (std::dynamic_pointer_cast<ScrollViewComponentInstance>(componentInstance) !=
      nullptr) {
    return true;
  }

  const auto componentName = componentInstance->getComponentName();
  return componentName == "ScrollView" ||
      componentName == "AndroidHorizontalScrollView" ||
      componentName == "ScrollContentView";
}

} // namespace

class TouchThroughWrapperTouchHandler : public UIInputEventHandler {
 public:
  explicit TouchThroughWrapperTouchHandler(
      TouchThroughWrapperComponentInstance* owner)
      : UIInputEventHandler(owner->getLocalRootArkUINode()), m_owner(owner) {}

  void onTouchEvent(ArkUI_UIInputEvent* event) override {
    const auto action = OH_ArkUI_UIInputEvent_GetAction(event);
    if (action != UI_TOUCH_EVENT_ACTION_UP &&
        action != UI_TOUCH_EVENT_ACTION_CANCEL) {
      return;
    }
    const auto windowX = OH_ArkUI_PointerEvent_GetWindowX(event);
    const auto windowY = OH_ArkUI_PointerEvent_GetWindowY(event);
    m_owner->releaseTouchThroughSequence(action, windowX, windowY);
  }

 private:
  TouchThroughWrapperComponentInstance* m_owner;
};

TouchThroughWrapperComponentInstance::TouchThroughWrapperComponentInstance(
    Context context)
    : BaseR2RTouchThroughWrapperComponentInstance(context),
      m_stackNode(context.arkUINodeContext) {}

void TouchThroughWrapperComponentInstance::onCreate() {
  BaseR2RTouchThroughWrapperComponentInstance::onCreate();
  m_touchHandler = std::make_shared<TouchThroughWrapperTouchHandler>(this);
  touchthroughtrace::logLifecycle(
      "onCreate",
      m_traceSequenceId,
      getTag(),
      getComponentName(),
      m_stackNode.getArkUINodeHandle());
}

bool TouchThroughWrapperComponentInstance::canHandleTouch() const {
  return false;
}

bool TouchThroughWrapperComponentInstance::containsPoint(
    facebook::react::Point const& point) const {
  for (const auto& child : getChildren()) {
    if (child == nullptr) {
      continue;
    }
    auto childPoint = computeChildPoint(point, child);
    if (!child->containsPointInBoundingBox(childPoint)) {
      continue;
    }
    if (isTouchThroughHitAtPoint(child, childPoint)) {
      return false;
    }
  }
  return BaseR2RTouchThroughWrapperComponentInstance::containsPoint(point);
}

bool TouchThroughWrapperComponentInstance::containsPointInBoundingBox(
    facebook::react::Point const& point) {
  for (const auto& child : getChildren()) {
    if (child == nullptr) {
      continue;
    }
    auto childPoint = computeChildPoint(point, child);
    if (!child->containsPointInBoundingBox(childPoint)) {
      continue;
    }
    if (isTouchThroughHitAtPoint(child, childPoint)) {
      return false;
    }
  }
  return BaseR2RTouchThroughWrapperComponentInstance::containsPointInBoundingBox(
      point);
}

void TouchThroughWrapperComponentInstance::onChildInserted(
    ComponentInstance::Shared const& childComponentInstance,
    std::size_t index) {
  BaseR2RTouchThroughWrapperComponentInstance::onChildInserted(
      childComponentInstance, index);
  m_stackNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
}

void TouchThroughWrapperComponentInstance::onChildRemoved(
    ComponentInstance::Shared const& childComponentInstance) {
  BaseR2RTouchThroughWrapperComponentInstance::onChildRemoved(
      childComponentInstance);
  m_stackNode.removeChild(childComponentInstance->getLocalRootArkUINode());
}

StackNode& TouchThroughWrapperComponentInstance::getLocalRootArkUINode() {
  return m_stackNode;
}

void TouchThroughWrapperComponentInstance::onArkUINodeTouchIntercept(
    const ArkUI_UIInputEvent* event) {
  const auto action = OH_ArkUI_UIInputEvent_GetAction(event);
  // The DOWN event decides routing for the entire gesture. MOVE only needs
  // to preserve that decision, without another tree walk or per-event logging.
  if (action == UI_TOUCH_EVENT_ACTION_MOVE) {
    if (m_isTouchThroughSequenceActive) {
      OH_ArkUI_PointerEvent_SetInterceptHitTestMode(
          event, HitTestMode::HTM_TRANSPARENT);
    } else {
      BaseR2RTouchThroughWrapperComponentInstance::onArkUINodeTouchIntercept(event);
    }
    return;
  }

  const auto localX = OH_ArkUI_PointerEvent_GetX(event);
  const auto localY = OH_ArkUI_PointerEvent_GetY(event);
  const auto windowX = OH_ArkUI_PointerEvent_GetWindowX(event);
  const auto windowY = OH_ArkUI_PointerEvent_GetWindowY(event);
  const auto isTouchThroughAtLocalPoint =
      isTouchThroughHitAtPoint(shared_from_this(), {.x = localX, .y = localY});
  const auto isTouchThroughDown =
      action == UI_TOUCH_EVENT_ACTION_DOWN &&
      isTouchThroughAtLocalPoint;

  if (action == UI_TOUCH_EVENT_ACTION_DOWN && !m_isTouchThroughSequenceActive) {
    ++m_traceSequenceId;
  }

  if (action == UI_TOUCH_EVENT_ACTION_DOWN && m_isTouchThroughSequenceActive) {
    touchthroughtrace::logSequenceTransition(
        "staleReleaseOnNextDown",
        m_traceSequenceId,
        getTag(),
        getComponentName(),
        action,
        windowX,
        windowY);
    releaseTouchThroughSequence(UI_TOUCH_EVENT_ACTION_CANCEL, windowX, windowY);
    ++m_traceSequenceId;
  }

  touchthroughtrace::logInterceptProbe(
      m_traceSequenceId,
      getTag(),
      getComponentName(),
      action,
      localX,
      localY,
      windowX,
      windowY,
      isTouchThroughAtLocalPoint,
      m_isTouchThroughSequenceActive);

  if (isTouchThroughDown && !m_isTouchThroughSequenceActive) {
    m_isTouchThroughSequenceActive = true;
    touchthroughtrace::logSequenceTransition(
        "activate",
        m_traceSequenceId,
        getTag(),
        getComponentName(),
        action,
        windowX,
        windowY);
    setAncestorNativeScrollBlocked(shared_from_this(), true);
    setDescendantNativeScrollBlocked(shared_from_this(), true);
  }

  if (m_isTouchThroughSequenceActive) {
    touchthroughtrace::logSequenceTransition(
        "intercept",
        m_traceSequenceId,
        getTag(),
        getComponentName(),
        action,
        windowX,
        windowY);
    OH_ArkUI_PointerEvent_SetInterceptHitTestMode(
        event, HitTestMode::HTM_TRANSPARENT);
    return;
  }

  BaseR2RTouchThroughWrapperComponentInstance::onArkUINodeTouchIntercept(event);
}

bool TouchThroughWrapperComponentInstance::isTouchThroughHitAtPoint(
    TouchTarget::Shared const& target,
    facebook::react::Point const& point) const {
  if (isMarkerTarget(target) && target->containsPoint(point)) {
    return true;
  }

  for (const auto& child : target->getTouchTargetChildren()) {
    if (child == nullptr) {
      continue;
    }
    auto childPoint = target->computeChildPoint(point, child);
    if (!child->containsPointInBoundingBox(childPoint)) {
      continue;
    }
    if (isTouchThroughHitAtPoint(child, childPoint)) {
      return true;
    }
  }
  return false;
}

bool TouchThroughWrapperComponentInstance::isMarkerTarget(
    TouchTarget::Shared const& target) const {
  return std::dynamic_pointer_cast<TouchThroughViewComponentInstance>(target) !=
      nullptr;
}

bool TouchThroughWrapperComponentInstance::isTouchThroughHit(
    ComponentInstance::Shared const& componentInstance,
    float windowX,
    float windowY) const {
  if (isPointInsideMarker(componentInstance, windowX, windowY)) {
    return true;
  }

  for (const auto& child : componentInstance->getChildren()) {
    if (child != nullptr && isTouchThroughHit(child, windowX, windowY)) {
      return true;
    }
  }
  return false;
}

bool TouchThroughWrapperComponentInstance::isPointInsideMarker(
    ComponentInstance::Shared const& componentInstance,
    float windowX,
    float windowY) const {
  auto marker =
      std::dynamic_pointer_cast<TouchThroughViewComponentInstance>(componentInstance);
  if (marker == nullptr) {
    return false;
  }
  return isPointInsideNodeBounds(
      marker->getLocalRootArkUINode().getArkUINodeHandle(), windowX, windowY);
}

void TouchThroughWrapperComponentInstance::setAncestorNativeScrollBlocked(
    ComponentInstance::Shared const& componentInstance,
    bool blocked) {
  if (componentInstance == nullptr) {
    return;
  }

  auto parent = componentInstance->getParent().lock();
  while (parent != nullptr) {
    if (isNativeScrollLikeComponent(parent)) {
      touchthroughtrace::logScrollBlock(
          "ancestorBlock",
          m_traceSequenceId,
          getTag(),
          parent->getTag(),
          parent->getComponentName(),
          blocked);
      parent->setNativeResponderBlocked(blocked, NATIVE_RESPONDER_BLOCK_ORIGIN);
    }
    parent = parent->getParent().lock();
  }
}

void TouchThroughWrapperComponentInstance::setDescendantNativeScrollBlocked(
    ComponentInstance::Shared const& componentInstance,
    bool blocked) {
  if (componentInstance == nullptr) {
    return;
  }

  if (isNativeScrollLikeComponent(componentInstance)) {
    touchthroughtrace::logScrollBlock(
        "descendantBlock",
        m_traceSequenceId,
        getTag(),
        componentInstance->getTag(),
        componentInstance->getComponentName(),
        blocked);
    componentInstance->setNativeResponderBlocked(
        blocked, NATIVE_RESPONDER_BLOCK_ORIGIN);
  }

  for (const auto& child : componentInstance->getChildren()) {
    if (child == nullptr) {
      continue;
    }

    setDescendantNativeScrollBlocked(child, blocked);
  }
}

void TouchThroughWrapperComponentInstance::releaseTouchThroughSequence(
    int32_t action,
    float windowX,
    float windowY) {
  if (!m_isTouchThroughSequenceActive) {
    return;
  }

  touchthroughtrace::logSequenceTransition(
      "release",
      m_traceSequenceId,
      getTag(),
      getComponentName(),
      action,
      windowX,
      windowY);
  setAncestorNativeScrollBlocked(shared_from_this(), false);
  setDescendantNativeScrollBlocked(shared_from_this(), false);
  m_isTouchThroughSequenceActive = false;
}

} // namespace rnoh
