# Changelog

## [1.0.4](https://github.com/sentenz/actions/compare/v1.0.3...1.0.4) (2025-12-23)


### Bug Fixes

* refine action inputs and descriptions for clarity and consistency ([7ea128a](https://github.com/sentenz/actions/commit/7ea128a505683509540d53ea011e16b2f7b3a41f))

## [1.0.3](https://github.com/sentenz/actions/compare/v1.0.2...v1.0.3) (2025-12-23)


### Bug Fixes

* resolve Conftest action by replacing unsupported `--config` flag with `--config-file` ([#8](https://github.com/sentenz/actions/issues/8)) ([2a828d4](https://github.com/sentenz/actions/commit/2a828d494aafaa6825ad7d52327cc11c609daa25)), closes [#7](https://github.com/sentenz/actions/issues/7)

## [1.0.2](https://github.com/sentenz/actions/compare/v1.0.1...v1.0.2) (2025-12-23)


### Bug Fixes

* resolve Semgrep Action Docker command execution and configuration error ([#5](https://github.com/sentenz/actions/issues/5)) ([cea167d](https://github.com/sentenz/actions/commit/cea167d687e0c3eb16f83148162609f91f9dcaf3))

## [1.0.1](https://github.com/sentenz/actions/compare/v1.0.0...v1.0.1) (2025-12-22)


### Bug Fixes

* add config-file input to semantic-release action ([697e592](https://github.com/sentenz/actions/commit/697e592f581199518da17d35bf5d1d05cd3659c4))
* rename `config-file` input to `extends` in semantic-release action ([7c59ca2](https://github.com/sentenz/actions/commit/7c59ca27b1cb00a6ffa10a16a3afcc59c1345883))


### Reverts

* remove deprecated `config-file` input and add `.releaserc.json` configuration ([ab536e9](https://github.com/sentenz/actions/commit/ab536e9bdc2f49a08d76ba62d46d9ff51f156e02))

# 1.0.0 (2025-12-21)


### Bug Fixes

* update semantic-release configuration and improve checkout step ([664ebb4](https://github.com/sentenz/actions/commit/664ebb406b4a589b400318b62e540a4862c65504))


### Features

* initial commit ([24b30f6](https://github.com/sentenz/actions/commit/24b30f65df14e3a6f71fc6358db00669097c473a))
