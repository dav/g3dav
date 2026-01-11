# G3DAV macOS

This folder contains a SwiftUI macOS recreation of the original g3dav app.

## Open in Xcode

1. Open `macos/Package.swift` in Xcode.
2. Select the `G3DAVApp` scheme.
3. Run the app.

## Build an app bundle

Run the helper script to produce a `.app` you can drop into `/Applications`:

```bash
./macos/build_app.sh
```

The app bundle is created at `macos/build/G3DAVApp.app`.

## App icon

Place an `icns` file at `macos/assets/G3DAVApp.icns` to embed a custom icon in the app bundle.

## Controls

- Drag with the left mouse button to rotate the model.
- Scroll to zoom.
- Use the toolbar or View menu to toggle axes and reset the view.
- Use Settings to adjust anaglyphic colors and parameters.

Supported file types: `.obj`, `.pdb`, `.csv`.
