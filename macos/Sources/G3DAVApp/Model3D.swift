import Foundation

struct Point3D: Hashable {
    var x: Double
    var y: Double
    var z: Double
}

struct LineSegment: Hashable {
    var p1: Int
    var p2: Int
}

struct Label3D: Hashable {
    var point: Int
    var label: String
}

struct Bounds3D {
    var xmin: Double
    var xmax: Double
    var ymin: Double
    var ymax: Double
    var zmin: Double
    var zmax: Double
}

struct WorldBounds {
    var xmin: Double
    var xmax: Double
    var ymin: Double
    var ymax: Double
}

struct Model3D {
    var name: String?
    var vertices: [Point3D]
    var lines: [LineSegment]
    var labels: [Label3D]
    var originAxes: [Point3D]
    var bounds: Bounds3D
    var world: WorldBounds

    init(name: String? = nil) {
        self.name = name
        self.vertices = []
        self.lines = []
        self.labels = []
        self.originAxes = []
        self.bounds = Bounds3D(xmin: 0, xmax: 0, ymin: 0, ymax: 0, zmin: 0, zmax: 0)
        self.world = WorldBounds(xmin: -1, xmax: 1, ymin: -1, ymax: 1)
    }

    mutating func addVertex(_ point: Point3D) -> Int {
        vertices.append(point)
        return vertices.count - 1
    }

    mutating func addLine(_ p1: Int, _ p2: Int) {
        lines.append(LineSegment(p1: p1, p2: p2))
    }

    mutating func addLabel(_ point: Int, _ label: String) {
        labels.append(Label3D(point: point, label: label))
    }

    mutating func computeBounds() {
        guard let first = vertices.first else {
            return
        }

        var xmin = first.x
        var xmax = first.x
        var ymin = first.y
        var ymax = first.y
        var zmin = first.z
        var zmax = first.z

        for point in vertices.dropFirst() {
            xmin = min(xmin, point.x)
            xmax = max(xmax, point.x)
            ymin = min(ymin, point.y)
            ymax = max(ymax, point.y)
            zmin = min(zmin, point.z)
            zmax = max(zmax, point.z)
        }

        bounds = Bounds3D(xmin: xmin, xmax: xmax, ymin: ymin, ymax: ymax, zmin: zmin, zmax: zmax)
    }

    mutating func resetWorldBounds() {
        let xrange = bounds.xmax - bounds.xmin
        let yrange = bounds.ymax - bounds.ymin
        let zrange = bounds.zmax - bounds.zmin
        var maxrange = max(xrange, max(yrange, zrange))
        maxrange *= 1.5
        maxrange /= 2

        let xmid = (bounds.xmax + bounds.xmin) / 2
        let ymid = (bounds.ymax + bounds.ymin) / 2

        world = WorldBounds(
            xmin: xmid - maxrange,
            xmax: xmid + maxrange,
            ymin: ymid - maxrange,
            ymax: ymid + maxrange
        )

        if originAxes.isEmpty {
            let xAbs = max(abs(bounds.xmin), abs(bounds.xmax))
            originAxes.append(Point3D(x: -(xAbs * 2), y: 0, z: 0))
            originAxes.append(Point3D(x: xAbs * 2, y: 0, z: 0))

            let yAbs = max(abs(bounds.ymin), abs(bounds.ymax))
            originAxes.append(Point3D(x: 0, y: -(yAbs * 2), z: 0))
            originAxes.append(Point3D(x: 0, y: yAbs * 2, z: 0))

            let zAbs = max(abs(bounds.zmin), abs(bounds.zmax))
            originAxes.append(Point3D(x: 0, y: 0, z: -(zAbs * 2)))
            originAxes.append(Point3D(x: 0, y: 0, z: zAbs * 2))
        }
    }
}
