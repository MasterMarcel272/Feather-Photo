# GitHub build and installer

The Windows workflow builds a Release version, deploys the Qt runtime and image-format plugins, then creates one `FeatherPhoto-Setup.exe` installer.

## Add these files to the GitHub repository

Copy/merge this project into the repository, including:

- `.github/workflows/build-installer.yml`
- `packaging/FeatherPhoto.iss`
- `assets/FeatherPhoto.png`, `assets/FeatherPhoto.ico`, `assets/FeatherPhoto.qrc`, and `assets/FeatherPhoto.rc`
- the `src/main.cpp` change that sets the Qt window icon
- `.gitignore`
- the updated `CMakeLists.txt`

The icon is embedded in the Windows executable and shown in the app window, installer, Start menu, and optional desktop shortcut.

The workflow installs Qt 6.8.3 and Inno Setup on a Windows runner. It uses the Qt `win64_msvc2022_64` build and packages the MSVC runtime with the app.

## Create a release

Push a version tag such as `v0.1.0`. GitHub Actions will build the installer and attach only `FeatherPhoto-Setup.exe` as a release asset. A manual workflow run builds the same installer and makes it available as a single Actions artifact.

GitHub also generates source-code archives for releases automatically; these are separate from the installer asset.
