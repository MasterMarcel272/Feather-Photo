# Feather Manager

Feather Manager lists apps from the repository's root `apps.json`, checks each app's latest public GitHub Release, and downloads the configured installer. Before launching it, the Manager verifies the SHA-256 digest returned by GitHub's release API. Installers are started only after the user clicks Install or Update.

## Add an app

Add an entry to `apps.json` with:

- a stable `id` containing only letters, numbers, `_` or `-`;
- a display `name` and `description`;
- a public GitHub `repository` in `owner/name` format;
- the exact `installerAsset` file name attached to the app's latest Release;
- the Windows uninstall registry key whose `DisplayVersion` identifies the installed version.

The installer asset must be a Windows `.exe` and should be published in a GitHub Release so the API provides its SHA-256 digest.

## Build

Configure the `FeatherManager` directory with CMake, Qt 6 Widgets and Qt 6 Network. The `build-manager.yml` workflow builds a Windows installer and publishes it for `manager-v*` tags.
