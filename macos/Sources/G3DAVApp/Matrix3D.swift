import Foundation

struct Matrix3D {
    var xx: Double = 1
    var xy: Double = 0
    var xz: Double = 0
    var xo: Double = 0

    var yx: Double = 0
    var yy: Double = 1
    var yz: Double = 0
    var yo: Double = 0

    var zx: Double = 0
    var zy: Double = 0
    var zz: Double = 1
    var zo: Double = 0

    static var identity: Matrix3D { Matrix3D() }

    mutating func reset() {
        self = Matrix3D.identity
    }

    mutating func uniformScale(_ factor: Double) {
        xx *= factor
        xy *= factor
        xz *= factor
        xo *= factor
        yx *= factor
        yy *= factor
        yz *= factor
        yo *= factor
        zx *= factor
        zy *= factor
        zz *= factor
        zo *= factor
    }

    mutating func translate(x: Double, y: Double, z: Double) {
        xo += x
        yo += y
        zo += z
    }

    mutating func rotateY(_ thetaDegrees: Double) {
        let theta = thetaDegrees * Double.pi / 180
        let ct = cos(theta)
        let st = sin(theta)

        let nxx = xx * ct + zx * st
        let nxy = xy * ct + zy * st
        let nxz = xz * ct + zz * st
        let nxo = xo * ct + zo * st

        let nzx = zx * ct - xx * st
        let nzy = zy * ct - xy * st
        let nzz = zz * ct - xz * st
        let nzo = zo * ct - xo * st

        xo = nxo
        xx = nxx
        xy = nxy
        xz = nxz
        zo = nzo
        zx = nzx
        zy = nzy
        zz = nzz
    }

    mutating func rotateX(_ thetaDegrees: Double) {
        let theta = thetaDegrees * Double.pi / 180
        let ct = cos(theta)
        let st = sin(theta)

        let nyx = yx * ct + zx * st
        let nyy = yy * ct + zy * st
        let nyz = yz * ct + zz * st
        let nyo = yo * ct + zo * st

        let nzx = zx * ct - yx * st
        let nzy = zy * ct - yy * st
        let nzz = zz * ct - yz * st
        let nzo = zo * ct - yo * st

        yo = nyo
        yx = nyx
        yy = nyy
        yz = nyz
        zo = nzo
        zx = nzx
        zy = nzy
        zz = nzz
    }

    mutating func rotateZ(_ thetaDegrees: Double) {
        let theta = thetaDegrees * Double.pi / 180
        let ct = cos(theta)
        let st = sin(theta)

        let nyx = yx * ct + xx * st
        let nyy = yy * ct + xy * st
        let nyz = yz * ct + xz * st
        let nyo = yo * ct + xo * st

        let nxx = xx * ct - yx * st
        let nxy = xy * ct - yy * st
        let nxz = xz * ct - yz * st
        let nxo = xo * ct - yo * st

        yo = nyo
        yx = nyx
        yy = nyy
        yz = nyz
        xo = nxo
        xx = nxx
        xy = nxy
        xz = nxz
    }

    mutating func multiply(by rhs: Matrix3D) {
        let lxx = xx * rhs.xx + yx * rhs.xy + zx * rhs.xz
        let lxy = xy * rhs.xx + yy * rhs.xy + zy * rhs.xz
        let lxz = xz * rhs.xx + yz * rhs.xy + zz * rhs.xz
        let lxo = xo * rhs.xx + yo * rhs.xy + zo * rhs.xz + rhs.xo

        let lyx = xx * rhs.yx + yx * rhs.yy + zx * rhs.yz
        let lyy = xy * rhs.yx + yy * rhs.yy + zy * rhs.yz
        let lyz = xz * rhs.yx + yz * rhs.yy + zz * rhs.yz
        let lyo = xo * rhs.yx + yo * rhs.yy + zo * rhs.yz + rhs.yo

        let lzx = xx * rhs.zx + yx * rhs.zy + zx * rhs.zz
        let lzy = xy * rhs.zx + yy * rhs.zy + zy * rhs.zz
        let lzz = xz * rhs.zx + yz * rhs.zy + zz * rhs.zz
        let lzo = xo * rhs.zx + yo * rhs.zy + zo * rhs.zz + rhs.zo

        xx = lxx
        xy = lxy
        xz = lxz
        xo = lxo
        yx = lyx
        yy = lyy
        yz = lyz
        yo = lyo
        zx = lzx
        zy = lzy
        zz = lzz
        zo = lzo
    }

    func transform(points: [Point3D]) -> [Point3D] {
        var result: [Point3D] = []
        result.reserveCapacity(points.count)

        let lxx = xx, lxy = xy, lxz = xz, lxo = xo
        let lyx = yx, lyy = yy, lyz = yz, lyo = yo
        let lzx = zx, lzy = zy, lzz = zz, lzo = zo

        for point in points {
            let x = point.x * lxx + point.y * lxy + point.z * lxz + lxo
            let y = point.x * lyx + point.y * lyy + point.z * lyz + lyo
            let z = point.x * lzx + point.y * lzy + point.z * lzz + lzo
            result.append(Point3D(x: x, y: y, z: z))
        }

        return result
    }
}
