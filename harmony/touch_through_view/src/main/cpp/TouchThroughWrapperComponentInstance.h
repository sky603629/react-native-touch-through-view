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

#pragma once

#include <arkui/native_type.h>
#include <memory>
#include "RNOH/arkui/UIInputEventHandler.h"
#include "RNOH/arkui/StackNode.h"
#include "RNOH/generated/components/BaseR2RTouchThroughWrapperComponentInstance.h"
#include <cstdint>
#include <string>

namespace rnoh {

class TouchThroughWrapperComponentInstance
    : public BaseR2RTouchThroughWrapperComponentInstance {
 public:
  explicit TouchThroughWrapperComponentInstance(Context context);

  bool canHandleTouch() const override;
  bool containsPoint(facebook::react::Point const& point) const override;
  bool containsPointInBoundingBox(
      facebook::react::Point const& point) override;
  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;
  StackNode& getLocalRootArkUINode() override;
  void onCreate() override;
  void releaseTouchThroughSequence(
      int32_t action,
      float windowX,
      float windowY);

 protected:
  void onArkUINodeTouchIntercept(const ArkUI_UIInputEvent* event) override;

 private:
  bool isTouchThroughHitAtPoint(
      TouchTarget::Shared const& target,
      facebook::react::Point const& point) const;
  bool isMarkerTarget(TouchTarget::Shared const& target) const;
  bool isTouchThroughHit(
      ComponentInstance::Shared const& componentInstance,
      float windowX,
      float windowY) const;
  bool isPointInsideMarker(
      ComponentInstance::Shared const& componentInstance,
      float windowX,
      float windowY) const;
  void setAncestorNativeScrollBlocked(
      ComponentInstance::Shared const& componentInstance,
      bool blocked);
  void setDescendantNativeScrollBlocked(
      ComponentInstance::Shared const& componentInstance,
      bool blocked);

  StackNode m_stackNode;
  bool m_isTouchThroughSequenceActive = false;
  int64_t m_traceSequenceId = 0;
  std::shared_ptr<UIInputEventHandler> m_touchHandler;
  static constexpr const char* NATIVE_RESPONDER_BLOCK_ORIGIN =
      "TOUCH_THROUGH";
};

} // namespace rnoh
