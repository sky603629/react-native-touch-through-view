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

#include "RNOH/generated/BaseReactNativeTouchThroughViewPackage.h"
#include "TouchThroughViewComponentInstance.h"
#include "TouchThroughWrapperComponentInstance.h"

namespace rnoh {

class TouchThroughViewComponentInstanceFactoryDelegate
    : public ComponentInstanceFactoryDelegate {
 public:
  using ComponentInstanceFactoryDelegate::ComponentInstanceFactoryDelegate;

  ComponentInstance::Shared create(ComponentInstance::Context ctx) override {
    if (ctx.componentName == "R2RTouchThroughView") {
      return std::make_shared<TouchThroughViewComponentInstance>(
          std::move(ctx));
    }
    if (ctx.componentName == "R2RTouchThroughWrapper") {
      return std::make_shared<TouchThroughWrapperComponentInstance>(
          std::move(ctx));
    }
    return nullptr;
  }
};

class TouchThroughViewPackage : public BaseReactNativeTouchThroughViewPackage {
 public:
  using BaseReactNativeTouchThroughViewPackage::
      BaseReactNativeTouchThroughViewPackage;

  ComponentInstanceFactoryDelegate::Shared
  createComponentInstanceFactoryDelegate() override {
    return std::make_shared<TouchThroughViewComponentInstanceFactoryDelegate>();
  }
};

} // namespace rnoh
