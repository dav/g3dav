import Foundation

enum ModelLoader {
    static func load(url: URL) throws -> Model3D {
        let ext = url.pathExtension.lowercased()
        switch ext {
        case "obj":
            return try loadOBJ(url: url)
        case "pdb":
            return try loadPDB(url: url)
        case "csv":
            return try loadCSV(url: url)
        default:
            throw ModelLoaderError.unsupportedExtension(ext)
        }
    }

    private static func loadOBJ(url: URL) throws -> Model3D {
        let contents = try String(contentsOf: url)
        var model = Model3D(name: url.lastPathComponent)

        for rawLine in contents.components(separatedBy: .newlines) {
            let line = rawLine.trimmingCharacters(in: .whitespaces)
            if line.isEmpty || line.hasPrefix("#") {
                continue
            }

            if line.hasPrefix("v ") {
                let parts = Array(line.split(separator: " ").dropFirst())
                if parts.count >= 3,
                   let x = Double(parts[0]),
                   let y = Double(parts[1]),
                   let z = Double(parts[2]) {
                    _ = model.addVertex(Point3D(x: x, y: y, z: z))
                }
            } else if line.hasPrefix("f ") || line.hasPrefix("fo ") {
                let parts = Array(line.split(separator: " ").dropFirst())
                var indices: [Int] = []
                for part in parts {
                    let token = part.split(separator: "/").first ?? part
                    if let idx = Int(token) {
                        indices.append(idx - 1)
                    }
                }

                if indices.count >= 2 {
                    for i in 1..<indices.count {
                        model.addLine(indices[i - 1], indices[i])
                    }
                    model.addLine(indices.last ?? 0, indices.first ?? 0)
                }
            } else if line.hasPrefix("l ") {
                let parts = Array(line.split(separator: " ").dropFirst())
                if parts.count >= 2,
                   let p1 = Int(parts[0]),
                   let p2 = Int(parts[1]) {
                    model.addLine(p1 - 1, p2 - 1)
                }
            } else if line.hasPrefix("t ") {
                let parts = Array(line.split(separator: " ", maxSplits: 2).dropFirst())
                if parts.count >= 2,
                   let p1 = Int(parts[0]) {
                    let label = String(parts[1])
                    model.addLabel(p1 - 1, label)
                }
            }
        }

        finalizeModel(&model)
        return model
    }

    private static func loadPDB(url: URL) throws -> Model3D {
        let contents = try String(contentsOf: url)
        var model = Model3D(name: url.lastPathComponent)

        for rawLine in contents.components(separatedBy: .newlines) {
            if rawLine.hasPrefix("ATOM") {
                let x = parseFixedWidth(rawLine, start: 30, length: 8)
                let y = parseFixedWidth(rawLine, start: 38, length: 8)
                let z = parseFixedWidth(rawLine, start: 46, length: 8)

                if let xVal = Double(x), let yVal = Double(y), let zVal = Double(z) {
                    let idx = model.addVertex(Point3D(x: xVal, y: yVal, z: zVal))
                    let label = parseFixedWidth(rawLine, start: 12, length: 4)
                    if !label.trimmingCharacters(in: .whitespaces).isEmpty {
                        model.addLabel(idx, label.trimmingCharacters(in: .whitespaces))
                    }
                }
            } else if rawLine.hasPrefix("CONECT") {
                let tokens = rawLine.split(separator: " ")
                if tokens.count >= 3 {
                    guard let start = Int(tokens[1]) else { continue }
                    for token in tokens.dropFirst(2) {
                        if let node = Int(token) {
                            model.addLine(start - 1, node - 1)
                        }
                    }
                }
            }
        }

        finalizeModel(&model)
        return model
    }

    private static func loadCSV(url: URL) throws -> Model3D {
        let contents = try String(contentsOf: url)
        var model = Model3D(name: url.lastPathComponent)
        let lines = contents.components(separatedBy: .newlines)

        guard let header = lines.first, header.contains(",") else {
            throw ModelLoaderError.invalidFormat("Missing CSV header")
        }

        let headerParts = header.split(separator: ",", maxSplits: 2).map { String($0).trimmingCharacters(in: .whitespaces) }
        let label1 = headerParts.indices.contains(0) ? headerParts[0] : "X"
        let label2 = headerParts.indices.contains(1) ? headerParts[1] : "Y"
        let label3 = headerParts.indices.contains(2) ? headerParts[2] : "Z"

        for rawLine in lines.dropFirst() {
            if !rawLine.contains(",") {
                continue
            }

            let parts = rawLine.split(separator: ",", maxSplits: 2)
            if parts.count >= 3,
               let x = Double(parts[0].trimmingCharacters(in: .whitespaces)),
               let y = Double(parts[1].trimmingCharacters(in: .whitespaces)),
               let z = Double(parts[2].trimmingCharacters(in: .whitespacesAndNewlines)) {
                let idx = model.addVertex(Point3D(x: x, y: y, z: z))
                model.addLine(idx, idx)
            }
        }

        finalizeModel(&model)

        let xMin = model.bounds.xmin
        let xMax = model.bounds.xmax
        let yMin = model.bounds.ymin
        let yMax = model.bounds.ymax
        let zMin = model.bounds.zmin
        let zMax = model.bounds.zmax

        let xMinIdx = model.addVertex(Point3D(x: xMin, y: 0, z: 0))
        let xMaxIdx = model.addVertex(Point3D(x: xMax, y: 0, z: 0))
        model.addLine(xMinIdx, xMaxIdx)
        model.addLabel(xMaxIdx, label1)

        let yMinIdx = model.addVertex(Point3D(x: 0, y: yMin, z: 0))
        let yMaxIdx = model.addVertex(Point3D(x: 0, y: yMax, z: 0))
        model.addLine(yMinIdx, yMaxIdx)
        model.addLabel(yMaxIdx, label2)

        let zMinIdx = model.addVertex(Point3D(x: 0, y: 0, z: zMin))
        let zMaxIdx = model.addVertex(Point3D(x: 0, y: 0, z: zMax))
        model.addLine(zMinIdx, zMaxIdx)
        model.addLabel(zMaxIdx, label3)

        return model
    }

    private static func finalizeModel(_ model: inout Model3D) {
        model.computeBounds()
        model.resetWorldBounds()
    }

    private static func parseFixedWidth(_ line: String, start: Int, length: Int) -> String {
        guard line.count >= start else { return "" }
        let startIndex = line.index(line.startIndex, offsetBy: min(start, line.count))
        let endIndex = line.index(startIndex, offsetBy: min(length, line.count - start), limitedBy: line.endIndex) ?? line.endIndex
        return String(line[startIndex..<endIndex]).trimmingCharacters(in: .whitespaces)
    }
}

enum ModelLoaderError: LocalizedError {
    case unsupportedExtension(String)
    case invalidFormat(String)

    var errorDescription: String? {
        switch self {
        case .unsupportedExtension(let ext):
            return "Unsupported file extension: \(ext)"
        case .invalidFormat(let message):
            return "Invalid model format: \(message)"
        }
    }
}
