EnTT (https://github.com/skypjack/entt) third-party vendored headers go here.

Recommended layout (any of these will be automatically added to PrivateIncludePaths by UEcs.Build.cs):

- single_include/entt/entt.hpp
- include/entt/ (...full header tree...)

To update:
1) Preferred: Download a release archive from EnTT and extract the 'single_include' folder here so that the header is at:
   Plugins/UEcs/ThirdParty/entt/single_include/entt/entt.hpp
2) Alternatively, clone the repository and copy the 'src/entt' into 'include/entt'.

This dependency is intentionally private to the UEcs plugin so EnTT symbols/headers are not leaked to other modules unless they explicitly include them via UEcs or add their own paths.
