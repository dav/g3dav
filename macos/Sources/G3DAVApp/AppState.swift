import AppKit
import Combine
import UniformTypeIdentifiers

final class AppState: ObservableObject {
    @Published var model: Model3D?
    @Published var matrix: Matrix3D = .identity
    @Published var anaglyphic: Anaglyphic = Anaglyphic()
    @Published var redColor: NSColor = .red
    @Published var cyanColor: NSColor = .cyan
    @Published var showRotationAxes: Bool = false
    @Published var showOriginAxes: Bool = false

    func openModel() {
        let panel = NSOpenPanel()
        panel.allowedContentTypes = [
            UTType(filenameExtension: "obj"),
            UTType(filenameExtension: "pdb"),
            UTType(filenameExtension: "csv")
        ].compactMap { $0 }
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false

        panel.begin { [weak self] response in
            guard response == .OK, let url = panel.url else {
                return
            }

            do {
                var newModel = try ModelLoader.load(url: url)
                self?.matrix = .identity
                self?.centerModel(&newModel)
                self?.model = newModel
            } catch {
                self?.presentError(error)
            }
        }
    }

    func resetView() {
        guard var model = model else { return }
        model.resetWorldBounds()
        self.model = model
    }

    func zoomIn() {
        guard var model = model else { return }
        let rangeX = model.world.xmax - model.world.xmin
        let rangeY = model.world.ymax - model.world.ymin
        let deltaX = rangeX * 0.25
        let deltaY = rangeY * 0.25

        model.world.xmin += deltaX
        model.world.xmax -= deltaX
        model.world.ymin += deltaY
        model.world.ymax -= deltaY
        self.model = model
    }

    func zoomOut() {
        guard var model = model else { return }
        let rangeX = model.world.xmax - model.world.xmin
        let rangeY = model.world.ymax - model.world.ymin
        let deltaX = rangeX * 0.5
        let deltaY = rangeY * 0.5

        model.world.xmin -= deltaX
        model.world.xmax += deltaX
        model.world.ymin -= deltaY
        model.world.ymax += deltaY
        self.model = model
    }

    func applyRotation(deltaX: Double, deltaY: Double, canvasSize: CGSize) {
        guard model != nil else { return }
        let xtheta = (deltaY) * (360.0 / max(1, canvasSize.width))
        let ytheta = (deltaX) * (360.0 / max(1, canvasSize.height))

        var updated = matrix
        updated.rotateY(ytheta)
        updated.rotateX(xtheta)
        matrix = updated
    }

    private func centerModel(_ model: inout Model3D) {
        let xmid = (model.bounds.xmax + model.bounds.xmin) / 2
        let ymid = (model.bounds.ymax + model.bounds.ymin) / 2
        let zmid = (model.bounds.zmax + model.bounds.zmin) / 2

        var updatedMatrix = matrix
        updatedMatrix.translate(x: -xmid, y: -ymid, z: -zmid)
        matrix = updatedMatrix

        model.world.xmin -= xmid
        model.world.xmax -= xmid
        model.world.ymin -= ymid
        model.world.ymax -= ymid
    }

    private func presentError(_ error: Error) {
        let alert = NSAlert(error: error)
        alert.runModal()
    }
}
