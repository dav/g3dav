import Foundation

struct Anaglyphic {
    var viewDistance: Double = 50
    var eyeDistance: Double = 5

    func translate(_ point: Point3D) -> (red: Point3D, cyan: Point3D) {
        let z = point.z
        let denom = (viewDistance - z)
        let safeDenom = abs(denom) < 0.0001 ? (denom < 0 ? -0.0001 : 0.0001) : denom

        let redY = viewDistance * point.y / safeDenom
        let redX = (viewDistance * point.x - eyeDistance * z / 2) / safeDenom

        let cyanY = viewDistance * point.y / safeDenom
        let cyanX = (viewDistance * point.x + eyeDistance * z / 2) / safeDenom

        return (
            red: Point3D(x: redX, y: redY, z: z),
            cyan: Point3D(x: cyanX, y: cyanY, z: z)
        )
    }
}
