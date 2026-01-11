#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_NAME="G3DAVApp"
BUILD_DIR="${ROOT_DIR}/build"
APP_BUNDLE="${BUILD_DIR}/${APP_NAME}.app"
EXECUTABLE="${ROOT_DIR}/.build/release/${APP_NAME}"
ICON_SOURCE="${ROOT_DIR}/assets/G3DAVApp.icns"
ICON_NAME="G3DAVApp.icns"

swift build --package-path "${ROOT_DIR}" -c release

rm -rf "${APP_BUNDLE}"
mkdir -p "${APP_BUNDLE}/Contents/MacOS" "${APP_BUNDLE}/Contents/Resources"
cp "${EXECUTABLE}" "${APP_BUNDLE}/Contents/MacOS/${APP_NAME}"
if [[ -f "${ICON_SOURCE}" ]]; then
  cp "${ICON_SOURCE}" "${APP_BUNDLE}/Contents/Resources/${ICON_NAME}"
fi

cat > "${APP_BUNDLE}/Contents/Info.plist" <<'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDisplayName</key>
    <string>G3DAV</string>
    <key>CFBundleExecutable</key>
    <string>G3DAVApp</string>
    <key>CFBundleIdentifier</key>
    <string>com.g3dav.app</string>
    <key>CFBundleIconFile</key>
    <string>G3DAVApp.icns</string>
    <key>CFBundleName</key>
    <string>G3DAV</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSMinimumSystemVersion</key>
    <string>13.0</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
</dict>
</plist>
EOF

echo "Built ${APP_BUNDLE}"
