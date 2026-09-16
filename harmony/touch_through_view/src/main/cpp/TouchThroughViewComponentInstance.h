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

#include "RNOH/arkui/StackNode.h"
#include "RNOH/generated/components/BaseR2RTouchThroughViewComponentInstance.h"

namespace rnoh {

class TouchThroughViewComponentInstance
    : public BaseR2RTouchThroughViewComponentInstance {
 public:
  explicit TouchThroughViewComponentInstance(Context context);
  void onCreate() override;

  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;
  StackNode& getLocalRootArkUINode() override;

 protected:
  void onPropsChanged(SharedConcreteProps const& props) override;

 private:
  StackNode m_stackNode;
};

} // namespace rnoh
