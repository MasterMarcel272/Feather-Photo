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

Push to `main` or start the workflow manually to build the installer as a single Actions artifact. Push a version tag such as `v0.1.0` to build it and attach only `FeatherPhoto-Setup.exe` as a release asset.

GitHub also generates source-code archives for releases automatically; these are separate from the installer asset.

## Feather Manager

`FeatherManager/` is a separate Qt application. Its app list is in the root `apps.json` catalog. Each entry points to a public GitHub repository and the installer asset name expected in its latest Release. The Manager checks GitHub's release API, downloads the installer, verifies GitHub's SHA-256 asset digest, then starts the installer when the user clicks Install or Update.

The Manager has its own Windows workflow. Push to `main` to build a downloadable Actions artifact, or push a tag such as `manager-v0.1.0` to publish a Manager Release. Photo releases use tags such as `v0.1.0` and contain `FeatherPhoto-Setup.exe`.
