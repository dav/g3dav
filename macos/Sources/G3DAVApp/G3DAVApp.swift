import AppKit
import SwiftUI

final class AppDelegate: NSObject, NSApplicationDelegate {
    func applicationDidFinishLaunching(_ notification: Notification) {
        NSApp.setActivationPolicy(.regular)
        NSApp.activate(ignoringOtherApps: true)
    }
}

@main
struct G3DAVApp: App {
    @StateObject private var state = AppState()
    @NSApplicationDelegateAdaptor(AppDelegate.self) private var appDelegate

    var body: some Scene {
        WindowGroup {
            ContentView(state: state)
                .frame(minWidth: 640, minHeight: 640)
        }
        .commands {
            CommandGroup(after: .newItem) {
                Button("Open Model") {
                    state.openModel()
                }
                .keyboardShortcut("o")
            }

            CommandMenu("View") {
                Button("Zoom In") {
                    state.zoomIn()
                }
                .keyboardShortcut("=")

                Button("Zoom Out") {
                    state.zoomOut()
                }
                .keyboardShortcut("-")

                Button("Reset View") {
                    state.resetView()
                }
                .keyboardShortcut("v")

                Toggle("Rotation Axes", isOn: $state.showRotationAxes)
                Toggle("Origin Axes", isOn: $state.showOriginAxes)
            }
        }
    }
}
