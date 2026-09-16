> 文档模板：v0.4.2

<p align="center">
  <h1 align="center"> <code>react-native-touch-through-view</code> </h1>
</p>

本项目基于 [react-native-touch-through-view](https://github.com/simonhoss/react-native-touch-through-view) 开发。

版本所属关系如下：

| 三方库名称    | 三方库版本（npm地址）    | 发布信息     | 支持RN版本    | Autolink     | 编译API版本     | 社区基线版本    | 源码地址   |
| ------------ | ------------ | ------------------------------ | ------------- | ------------- | ------------------------ | ------------- | ------------- |
| @react-native-ohos/react-native-touch-through-view | [1.2.1(开发中)](https://www.npmjs.com/package/@react-native-ohos/react-native-touch-through-view) | [GitHub Releases](https://github.com/react-native-oh-library/react-native-touch-through-view/releases) |`0.72.* / 0.77.* /0.82.* / 0.84.*`| 是 |  partially(0.72/0.82/0.84)） | 1.2.1-beta.1 | [master](https://github.com/react-native-oh-library/react-native-touch-through-view/tree/master) |

## 简介

用于 React Native 的触摸透传组件。<br/>
通过 TouchThroughWrapper 包裹上层滚动容器，使用 TouchThroughView 标记透传区域，使该区域内的触摸事件传递到底层可交互内容。

## 下载安装

进入到工程目录并输入以下命令：

**npm**

```bash
npm install @react-native-ohos/react-native-touch-through-view
```

**yarn**

```bash
yarn add @react-native-ohos/react-native-touch-through-view
```

## Link

|                                      | 是否支持autolink | RN框架版本 |
|--------------------------------------|----------------|-----------|
| 1.2.1                           | partially(0.72/0.82/0.84) | 0.72/0.82/0.84 |

使用AutoLink的工程需要根据该文档配置，Autolink框架指导文档：https://gitcode.com/CPF-RN/ohos_react_native/blob/master/docs/zh-cn/Autolinking.md

如您使用的版本支持 Autolink，并且工程已接入 Autolink，可跳过ManualLink配置。

<details>
  <summary>ManualLink: 此步骤为手动配置原生依赖项的指导</summary>

首先需要使用 DevEco Studio 打开项目里的 HarmonyOS 工程 `harmony`。

### 1. Overrides RN SDK

为了让工程依赖同一个版本的 RN SDK，需要在工程根目录的 `oh-package.json5` 添加 overrides 字段，指向工程需要使用的 RN SDK 版本。替换的版本既可以是一个具体的版本号，也可以是一个模糊版本，还可以是本地存在的 HAR 包或源码目录。

关于该字段的作用请阅读[官方说明](https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V5/ide-oh-package-json5-V5#zh-cn_topic_0000001792256137_overrides)

```json
{
  "overrides": {
    "@rnoh/react-native-openharmony": "./react_native_openharmony" // 指向工程使用的 RN SDK 源码路径
    // "@rnoh/react-native-openharmony" : "./react_native_openharmony.har" // 指向本地 har 包的路径
  }
}
```

### 2. 引入原生端代码

目前有两种方法：

- 通过 har 包引入；
- 直接链接源码。

方法一：通过 har 包引入（推荐）

> [!TIP] har 包位于三方库安装路径的 `harmony` 文件夹下。

打开 `entry/oh-package.json5`，添加以下依赖

```json
"dependencies": {
    "@react-native-ohos/react-native-touch-through-view": "file:../../node_modules/@react-native-ohos/react-native-touch-through-view/harmony/touch_through_view.har"
  }
```

点击右上角的 `sync` 按钮

或者在命令行终端执行：

```bash
cd entry
ohpm install
```

方法二：直接链接源码

> [!TIP] 如需使用直接链接源码，请参考[直接链接源码说明](https://gitcode.com/CPF-RN/usage-docs/blob/master/zh-cn/link-source-code.md)

### 3. 配置 CMakeLists 和引入 TouchThroughViewPackage

打开 `entry/src/main/cpp/CMakeLists.txt`，添加：

```diff
project(rnapp)
cmake_minimum_required(VERSION 3.13)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(RNOH_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(NODE_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../node_modules")
+ set(OH_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules")
set(RNOH_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../react-native-harmony/harmony/cpp")
set(LOG_VERBOSITY_LEVEL 1)
set(CMAKE_ASM_FLAGS "-Wno-error=unused-command-line-argument -Qunused-arguments")
set(CMAKE_CXX_FLAGS "-fstack-protector-strong -Wl,-z,relro,-z,now,-z,noexecstack -s -fPIE -pie")
set(WITH_HITRACE_SYSTRACE 1) # for other CMakeLists.txt files to use
add_compile_definitions(WITH_HITRACE_SYSTRACE)

add_subdirectory("${RNOH_CPP_DIR}" ./rn)

# RNOH_BEGIN: manual_package_linking_1
add_subdirectory("../../../../sample_package/src/main/cpp" ./sample-package)
+ add_subdirectory("${OH_MODULES}/@react-native-ohos/react-native-touch-through-view/src/main/cpp" ./touch_through_view)
# RNOH_END: manual_package_linking_1

file(GLOB GENERATED_CPP_FILES "./generated/*.cpp")

add_library(rnoh_app SHARED
    ${GENERATED_CPP_FILES}
    "./PackageProvider.cpp"
    "${RNOH_CPP_DIR}/RNOHAppNapiBridge.cpp"
)
target_link_libraries(rnoh_app PUBLIC rnoh)

# RNOH_BEGIN: manual_package_linking_2
target_link_libraries(rnoh_app PUBLIC rnoh_sample_package)
+ target_link_libraries(rnoh_app PUBLIC rnoh_touch_through_view)
# RNOH_END: manual_package_linking_2
```

打开 `entry/src/main/cpp/PackageProvider.cpp`，添加：

```diff
#include "RNOH/PackageProvider.h"
#include "SamplePackage.h"
+ #include "TouchThroughViewPackage.h"

using namespace rnoh;

std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(Package::Context ctx) {
    return {
      std::make_shared<SamplePackage>(ctx),
+     std::make_shared<TouchThroughViewPackage>(ctx)
    };
}
```

### 4. 在 ArkTs 侧引入 TouchThroughViewPackage

打开 `entry/src/main/ets/RNPackagesFactory.ts`，添加：

```diff
  ...
+ import TouchThroughViewPackage from '@react-native-ohos/react-native-touch-through-view';

export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [
+    new TouchThroughViewPackage(ctx)
  ];
}
```
</details>

### 运行

点击右上角的 `sync` 按钮

或者在命令行终端执行：

```bash
cd entry
ohpm install
```

然后编译、运行即可。

## 约束与限制

### 兼容性

本文档内容基于以下环境验证通过：

1. RNOH: 0.84.1; SDK: HarmonyOS 6.0.0 Release SDK; IDE: DevEco Studio 6.0.0.858; ROM: 6.0.0.112;
2. RNOH: 0.82.1; SDK: HarmonyOS 6.0.0 Release SDK; IDE: DevEco Studio 6.0.0.858; ROM: 6.0.0.112;
3. RNOH: 0.77.18; SDK: HarmonyOS 6.0.0 Release SDK; IDE: DevEco Studio 6.0.0.858; ROM: 6.0.0.112;
4. RNOH: 0.72.96; SDK: HarmonyOS 6.0.0 Release SDK; IDE: DevEco Studio 6.0.0.858; ROM: 6.0.0.112;

### 权限要求

无。

### 编译运行API要求

仓库编译元数据的目标版本为 `API22`，最低兼容版本声明为 `API12`；最低编译版本及低版本 ROM 运行兼容性未记录。

## 使用示例

下面的代码展示了这个库的基本使用场景：

> [!WARNING] 使用时 import 的库名不变。

```javascript
import React from "react";
import { Button, ScrollView, StyleSheet, Text, View } from "react-native";
import {
  TouchThroughView,
  TouchThroughWrapper,
} from "react-native-touch-through-view";

const App = () => {
  return (
    <View style={{ flex: 1 }}>
      <View style={{ height: 200, justifyContent: "center" }}>
        <Button title="底层按钮" onPress={() => console.log("底层按钮被点击")} />
      </View>
      <TouchThroughWrapper style={StyleSheet.absoluteFill}>
        <ScrollView>
          <TouchThroughView style={{ height: 200 }} />
          <View style={{ height: 1000, backgroundColor: "white" }}>
            <Text>上层滚动内容</Text>
          </View>
        </ScrollView>
      </TouchThroughWrapper>
    </View>
  );
};

export default App;
```

## 使用说明

**透传区域设置**

使用 TouchThroughWrapper 包裹 ScrollView 或列表，在需要透传的位置放置 TouchThroughView，并通过 style 设置区域大小。

```javascript
<TouchThroughWrapper style={StyleSheet.absoluteFill}>
  <ScrollView>
    <TouchThroughView style={{ height: 200 }} />
    <View style={{ height: 1000 }} />
  </ScrollView>
</TouchThroughWrapper>
```

从透传区域开始的手势优先交给底层组件；从普通区域开始的手势由上层滚动容器处理。

## 接口说明

> [!TIP] "Platform"列表示该属性在原三方库上支持的平台。

> [!TIP] "OpenHarmony Support"列为 yes 表示 OpenHarmony平台支持 该属性；no 则表示不支持；partially 表示部分支持。使用方法跨平台一致，效果对标 iOS 或 Android 的效果。

### 组件

| 名称 | 参数类型 | 必填 | 平台 | OpenHarmony平台支持 | 描述 |
| ------------ | ------------ | ------------ | ------------ | ------------ | ------------ |
| TouchThroughView | ViewProps | yes | iOS, Android | yes | 标记触摸透传区域。 |
| TouchThroughWrapper | ViewProps | yes | iOS, Android | yes | 包裹上层滚动容器，配合 TouchThroughView 将触摸传递到底层组件。 |

### 属性

ViewProps

两个组件均继承 ViewProps，无自定义属性。

### API

无

## 遗留问题

无

## 其他

无

## 目录结构

````
/react-native-touch-through-view  # 项目根目录
├── harmony                      # 鸿蒙适配代码
│   └── touch_through_view        # 鸿蒙适配核心代码
│       ├── build-profile.json5   # 模块构建配置
│       ├── oh-package.json5      # 模块包配置
│       └── src/main
│           ├── ets
│           │   └── Index.ets     # ArkTS 包入口
│           └── cpp
│               ├── TouchThroughViewPackage.h                     # 原生包注册
│               ├── TouchThroughViewComponentInstance.h           # 透传区域声明
│               ├── TouchThroughViewComponentInstance.cpp         # 透传区域实现
│               ├── TouchThroughWrapperComponentInstance.h        # 透传容器声明
│               ├── TouchThroughWrapperComponentInstance.cpp      # 透传容器实现
│               └── TouchThroughTrace.h                           # 触摸日志
├── components                   # RN 组件
│   ├── TouchThroughView.js       # 透传区域组件
│   └── TouchThroughWrapper.js    # 透传容器组件
├── specs                        # Codegen 类型文件
│   ├── TouchThroughViewNativeComponent.ts
│   └── TouchThroughWrapperNativeComponent.ts
├── example                      # 示例工程（RNOH 0.82 模板 + 上游 demo）
│   ├── src
│   │   ├── App.tsx               # 上游 demo 页面（含透传/缩放演示）
│   │   ├── album.jpg             # 示例图片资源
│   │   └── index.tsx             # 示例根组件（直接显示 demo）
│   ├── harmony                   # 示例鸿蒙工程（entry/AppScope 等）
│   ├── package.json              # 示例依赖（file:../ 本库 tgz）
│   └── index.js / metro.config.js / tsconfig.json
├── index.js                     # RN 入口文件
├── index.d.ts                   # 类型声明
├── buildEnv.sh                  # 示例工程位置声明（CI 构建用）
├── README.md / README_en.md     # 中英文说明文档
├── README.OpenSource            # 开源元数据（上游版本/Owner/上游 URL）
├── CHANGELOG.md                 # 变更记录
├── OAT.xml                      # 开源合规扫描清单
├── LICENSE                      # 许可证
└── package.json                 # 包元数据
````

## 贡献代码

使用过程中发现任何问题都可以提交 [Issue](https://github.com/react-native-oh-library/react-native-touch-through-view/issues)，当然，也非常欢迎提交 [PR](https://github.com/react-native-oh-library/react-native-touch-through-view/pulls) 。

## 开源协议

本项目基于 [MIT License](https://github.com/simonhoss/react-native-touch-through-view/blob/master/package.json#L24) ，请自由地享受和参与开源。
