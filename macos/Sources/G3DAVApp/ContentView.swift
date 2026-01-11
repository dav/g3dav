import SwiftUI

struct ContentView: View {
    @ObservedObject var state: AppState
    @State private var showSettings = false

    var body: some View {
        AnaglyphCanvas(state: state)
            .background(Color.black)
            .toolbar {
                ToolbarItemGroup {
                    Button("Open") {
                        state.openModel()
                    }

                    Button("Zoom In") {
                        state.zoomIn()
                    }

                    Button("Zoom Out") {
                        state.zoomOut()
                    }

                    Button("Reset View") {
                        state.resetView()
                    }

                    Toggle("Rotation Axes", isOn: $state.showRotationAxes)
                    Toggle("Origin Axes", isOn: $state.showOriginAxes)

                    Button("Settings") {
                        showSettings = true
                    }
                }
            }
            .sheet(isPresented: $showSettings) {
                SettingsView(state: state)
            }
    }
}

struct SettingsView: View {
    @ObservedObject var state: AppState

    var body: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Anaglyphic Settings")
                .font(.title2)

            HStack {
                Text("Red")
                    .frame(width: 80, alignment: .leading)
                ColorWell(color: $state.redColor)
            }

            HStack {
                Text("Cyan")
                    .frame(width: 80, alignment: .leading)
                ColorWell(color: $state.cyanColor)
            }

            VStack(alignment: .leading) {
                Text("Eye Distance: \(state.anaglyphic.eyeDistance, specifier: "%.2f")")
                Slider(value: Binding(get: {
                    state.anaglyphic.eyeDistance
                }, set: { newValue in
                    var updated = state.anaglyphic
                    updated.eyeDistance = newValue
                    state.anaglyphic = updated
                }), in: 1...20)
            }

            VStack(alignment: .leading) {
                Text("View Distance: \(state.anaglyphic.viewDistance, specifier: "%.2f")")
                Slider(value: Binding(get: {
                    state.anaglyphic.viewDistance
                }, set: { newValue in
                    var updated = state.anaglyphic
                    updated.viewDistance = newValue
                    state.anaglyphic = updated
                }), in: 10...200)
            }

            Spacer()
        }
        .padding(20)
        .frame(width: 360, height: 280)
    }
}
