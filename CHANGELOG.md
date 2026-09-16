# Changelog

All notable changes to this project will be documented in this file.

This file follows the [Keep a Changelog](https://keepachangelog.com/) format, and version numbers follow Semantic Versioning. For upstream changes, see [Upstream Release Notes](#upstream-release-notes) at the end of this document.

## [Unreleased]

### Added

- Added Autolink configuration to automatically register TouchThroughViewPackage and link rnoh_touch_through_view.

## [1.2.1-beta.1] - 2026-05-22

### Added

- Initial HarmonyOS adaptation based on RNOH 0.82, published under the npm package name `@react-native-ohos/react-native-touch-through-view`. The community baseline is upstream [react-native-touch-through-view](https://github.com/simonhoss/react-native-touch-through-view) (rome2rio master, 0.1.4).
- Implemented the `R2RTouchThroughView` and `R2RTouchThroughWrapper` Fabric native components using ComponentHarness, with Codegen definitions in `specs/` and C++/ArkTS implementations in `harmony/touch_through_view`.
- Added touch pass-through hit testing: gestures starting in a `TouchThroughView` pass-through area are delivered to the underlying content first, while gestures starting in normal areas are handled by the upper scroll container.

### Fixed

- Fixed the upper scroll container requiring a second interaction to respond, enabling immediate scrolling responses in both the upper and lower layers.

### Changed

- Aligned the top-level directory structure with repository submission requirements: moved the upstream demo into `example/` and added project governance files, including `README.OpenSource`, `CHANGELOG.md`, `OAT.xml`, and `README_en.md`.
- Updated metadata to meet repository submission requirements: added `files`, `bugs`, and `publishConfig` to `package.json`; standardized repository links; aligned the version in `harmony/touch_through_view/oh-package.json5` with the root `package.json`; completed version information in `README.md` and `README_en.md`; and added copyright holders to `LICENSE`.

## Upstream Release Notes

> For the history of upstream react-native-touch-through-view, originally authored by rome2rio and currently maintained on npm by [simonhoss](https://github.com/simonhoss/react-native-touch-through-view), with 1.2.0-beta.1 as the latest npm release, refer to the upstream repositories. This changelog records only changes related to the HarmonyOS adaptation.
