# g3dav WebGL

Static, raw WebGL implementation of the g3dav anaglyphic viewer.

## Run locally

Serve the folder with any static server:

```bash
python3 -m http.server 8080
```

Then open `http://localhost:8080/webgl/`.

## Controls

- Drag to rotate the model.
- Scroll to zoom in/out.
- Use the sliders to adjust eye/view distance.
- Use the color pickers to tune red/cyan filters.

Supported file types: `.obj`, `.pdb`, `.csv`.

## Samples manifest

The sample loader reads `webgl/samples/manifest.json` to populate the dropdown.
Add your `.obj` filenames to the `samples` array to make them show up.
