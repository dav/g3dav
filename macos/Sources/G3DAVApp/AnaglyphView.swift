import AppKit
import Combine
import SwiftUI

struct AnaglyphCanvas: NSViewRepresentable {
    @ObservedObject var state: AppState

    func makeNSView(context: Context) -> AnaglyphNSView {
        let view = AnaglyphNSView()
        view.state = state
        return view
    }

    func updateNSView(_ nsView: AnaglyphNSView, context: Context) {
        nsView.state = state
        nsView.needsDisplay = true
    }
}

final class AnaglyphNSView: NSView {
    var state: AppState? {
        didSet {
            if oldValue !== state {
                subscribeToState()
            }
        }
    }

    private var cancellable: AnyCancellable?
    private var lastDragPoint: NSPoint?

    override var acceptsFirstResponder: Bool { true }

    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)

        guard let state else {
        NSColor.black.setFill()
        NSBezierPath(rect: dirtyRect).fill()
        return
    }

    NSColor.black.setFill()
    NSBezierPath(rect: dirtyRect).fill()

        guard let model = state.model else {
            drawPlaceholder(in: dirtyRect)
            return
        }

        let bounds = dirtyRect
        let width = max(1, bounds.width)
        let height = max(1, bounds.height)

        let transformed = state.matrix.transform(points: model.vertices)
        let redColor = state.redColor
        let cyanColor = state.cyanColor

        let ctx = NSGraphicsContext.current?.cgContext
        ctx?.setLineWidth(1)

        for line in model.lines {
            guard line.p1 >= 0, line.p2 >= 0,
                  line.p1 < transformed.count, line.p2 < transformed.count else {
                continue
            }

            let p1 = transformed[line.p1]
            let p2 = transformed[line.p2]

            let red = mapToScreen(point: state.anaglyphic.translate(p1).red, world: model.world, width: width, height: height)
            let red2 = mapToScreen(point: state.anaglyphic.translate(p2).red, world: model.world, width: width, height: height)

            ctx?.setStrokeColor(redColor.cgColor)
            ctx?.move(to: red)
            ctx?.addLine(to: red2)
            ctx?.strokePath()

            let cyan = mapToScreen(point: state.anaglyphic.translate(p1).cyan, world: model.world, width: width, height: height)
            let cyan2 = mapToScreen(point: state.anaglyphic.translate(p2).cyan, world: model.world, width: width, height: height)

            ctx?.setStrokeColor(cyanColor.cgColor)
            ctx?.move(to: cyan)
            ctx?.addLine(to: cyan2)
            ctx?.strokePath()
        }

        drawLabels(model: model, transformed: transformed, width: width, height: height, state: state)

        if state.showRotationAxes {
            drawRotationAxes(width: width, height: height)
        }

        if state.showOriginAxes {
            drawOriginAxes(model: model, width: width, height: height, state: state)
        }
    }

    override func mouseDown(with event: NSEvent) {
        lastDragPoint = convert(event.locationInWindow, from: nil)
    }

    override func mouseDragged(with event: NSEvent) {
        guard let state else { return }
        let current = convert(event.locationInWindow, from: nil)
        let last = lastDragPoint ?? current
        let delta = NSPoint(x: last.x - current.x, y: last.y - current.y)
        lastDragPoint = current

        state.applyRotation(deltaX: delta.x, deltaY: delta.y, canvasSize: bounds.size)
    }

    override func scrollWheel(with event: NSEvent) {
        guard let state else { return }
        if event.deltaY > 0 {
            state.zoomIn()
        } else if event.deltaY < 0 {
            state.zoomOut()
        }
    }

    private func subscribeToState() {
        cancellable = state?.objectWillChange.sink { [weak self] _ in
            self?.needsDisplay = true
        }
    }

    private func mapToScreen(point: Point3D, world: WorldBounds, width: CGFloat, height: CGFloat) -> CGPoint {
        let xRange = world.xmax - world.xmin
        let yRange = world.ymax - world.ymin
        let safeXRange = abs(xRange) < 0.0001 ? (xRange < 0 ? -0.0001 : 0.0001) : xRange
        let safeYRange = abs(yRange) < 0.0001 ? (yRange < 0 ? -0.0001 : 0.0001) : yRange
        let x = ((point.x - world.xmin) / safeXRange) * width
        let y = height - ((point.y - world.ymin) / safeYRange) * height
        return CGPoint(x: x, y: y)
    }

    private func drawPlaceholder(in rect: NSRect) {
        let text = "Open an OBJ, PDB, or CSV file to begin."
        let attrs: [NSAttributedString.Key: Any] = [
            .foregroundColor: NSColor(white: 0.6, alpha: 1),
            .font: NSFont.systemFont(ofSize: 14, weight: .medium)
        ]
        let size = text.size(withAttributes: attrs)
        let point = CGPoint(x: rect.midX - size.width / 2, y: rect.midY - size.height / 2)
        text.draw(at: point, withAttributes: attrs)
    }

    private func drawLabels(model: Model3D, transformed: [Point3D], width: CGFloat, height: CGFloat, state: AppState) {
        let redColor = state.redColor
        let cyanColor = state.cyanColor
        let attrsRed: [NSAttributedString.Key: Any] = [
            .foregroundColor: redColor,
            .font: NSFont.monospacedSystemFont(ofSize: 10, weight: .regular)
        ]
        let attrsCyan: [NSAttributedString.Key: Any] = [
            .foregroundColor: cyanColor,
            .font: NSFont.monospacedSystemFont(ofSize: 10, weight: .regular)
        ]

        for label in model.labels {
            guard label.point >= 0, label.point < transformed.count else { continue }
            let point = transformed[label.point]
            let translated = state.anaglyphic.translate(point)
            let red = mapToScreen(point: translated.red, world: model.world, width: width, height: height)
            let cyan = mapToScreen(point: translated.cyan, world: model.world, width: width, height: height)
            label.label.draw(at: red, withAttributes: attrsRed)
            label.label.draw(at: cyan, withAttributes: attrsCyan)
        }
    }

    private func drawRotationAxes(width: CGFloat, height: CGFloat) {
        let ctx = NSGraphicsContext.current?.cgContext
        ctx?.setStrokeColor(NSColor.white.cgColor)
        ctx?.setLineWidth(1)
        ctx?.move(to: CGPoint(x: width / 2, y: 0))
        ctx?.addLine(to: CGPoint(x: width / 2, y: height))
        ctx?.strokePath()

        ctx?.move(to: CGPoint(x: 0, y: height / 2))
        ctx?.addLine(to: CGPoint(x: width, y: height / 2))
        ctx?.strokePath()
    }

    private func drawOriginAxes(model: Model3D, width: CGFloat, height: CGFloat, state: AppState) {
        guard model.originAxes.count >= 6 else { return }
        let transformed = state.matrix.transform(points: model.originAxes)
        let pairs = [(0, 1, "X"), (2, 3, "Y"), (4, 5, "Z")]

        for (start, end, label) in pairs {
            let p1 = transformed[start]
            let p2 = transformed[end]
            let translated1 = state.anaglyphic.translate(p1)
            let translated2 = state.anaglyphic.translate(p2)

            let red1 = mapToScreen(point: translated1.red, world: model.world, width: width, height: height)
            let red2 = mapToScreen(point: translated2.red, world: model.world, width: width, height: height)
            let cyan1 = mapToScreen(point: translated1.cyan, world: model.world, width: width, height: height)
            let cyan2 = mapToScreen(point: translated2.cyan, world: model.world, width: width, height: height)

            let ctx = NSGraphicsContext.current?.cgContext
            ctx?.setLineWidth(1)
            ctx?.setStrokeColor(state.redColor.cgColor)
            ctx?.move(to: red1)
            ctx?.addLine(to: red2)
            ctx?.strokePath()

            ctx?.setStrokeColor(state.cyanColor.cgColor)
            ctx?.move(to: cyan1)
            ctx?.addLine(to: cyan2)
            ctx?.strokePath()

            let attrsRed: [NSAttributedString.Key: Any] = [
                .foregroundColor: state.redColor,
                .font: NSFont.monospacedSystemFont(ofSize: 12, weight: .bold)
            ]
            let attrsCyan: [NSAttributedString.Key: Any] = [
                .foregroundColor: state.cyanColor,
                .font: NSFont.monospacedSystemFont(ofSize: 12, weight: .bold)
            ]
            label.draw(at: red2, withAttributes: attrsRed)
            label.draw(at: cyan2, withAttributes: attrsCyan)
        }
    }
}
