// swift-tools-version: 5.9
import PackageDescription

let package = Package(
    name: "G3DAVApp",
    platforms: [
        .macOS(.v13)
    ],
    products: [
        .executable(name: "G3DAVApp", targets: ["G3DAVApp"])
    ],
    targets: [
        .executableTarget(
            name: "G3DAVApp",
            path: "Sources/G3DAVApp"
        )
    ]
)
