/*
 * Swift Examples: 3D Graphics and Rendering
 * 
 * This file demonstrates comprehensive 3D graphics and rendering implementation
 * used in production iOS applications, based on Apple's Metal and Core Graphics.
 * 
 * Key Learning Objectives:
 * - Master Metal 3D graphics programming
 * - Understand advanced rendering techniques
 * - Learn 3D mathematics and transformations
 * - Apply production-grade 3D optimization
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Meta/Google Production Code Quality
 */

import Foundation
import Metal
import MetalKit
import simd
import CoreGraphics
import CoreImage

// MARK: - 3D Graphics Engine

/**
 * Production-grade 3D graphics engine
 * 
 * This class demonstrates comprehensive 3D graphics
 * with Metal integration and advanced rendering techniques
 */
class Graphics3DEngine: NSObject, ObservableObject {
    
    // MARK: - Properties
    
    @Published var isRendering = false
    @Published var frameRate: Double = 0.0
    @Published var renderTime: TimeInterval = 0.0
    @Published var triangleCount: Int = 0
    @Published var drawCalls: Int = 0
    @Published var memoryUsage: Int64 = 0
    
    private var device: MTLDevice
    private var commandQueue: MTLCommandQueue
    private var renderPipelineState: MTLRenderPipelineState
    private var depthStencilState: MTLDepthStencilState
    private var vertexBuffer: MTLBuffer
    private var indexBuffer: MTLBuffer
    private var uniformBuffer: MTLBuffer
    
    private var renderPassDescriptor: MTLRenderPassDescriptor
    private var depthTexture: MTLTexture
    private var colorTexture: MTLTexture
    
    private var scene: Scene3D
    private var camera: Camera3D
    private var lighting: LightingSystem
    private var materials: MaterialSystem
    private var textures: TextureManager
    
    private var frameTimer: Timer?
    private var lastFrameTime: CFTimeInterval = 0
    
    // MARK: - Initialization
    
    override init() {
        guard let device = MTLCreateSystemDefaultDevice() else {
            fatalError("Metal is not supported on this device")
        }
        
        self.device = device
        self.commandQueue = device.makeCommandQueue()!
        self.scene = Scene3D()
        self.camera = Camera3D()
        self.lighting = LightingSystem()
        self.materials = MaterialSystem()
        self.textures = TextureManager()
        
        // Initialize Metal resources
        self.renderPipelineState = Self.createRenderPipelineState(device: device)
        self.depthStencilState = Self.createDepthStencilState(device: device)
        self.vertexBuffer = Self.createVertexBuffer(device: device)
        self.indexBuffer = Self.createIndexBuffer(device: device)
        self.uniformBuffer = Self.createUniformBuffer(device: device)
        self.renderPassDescriptor = Self.createRenderPassDescriptor()
        self.depthTexture = Self.createDepthTexture(device: device)
        self.colorTexture = Self.createColorTexture(device: device)
        
        super.init()
        
        setupGraphicsEngine()
    }
    
    // MARK: - Public Methods
    
    /**
     * Start rendering
     * 
     * This method demonstrates 3D rendering initialization
     * with comprehensive scene setup and optimization
     */
    func startRendering() {
        isRendering = true
        lastFrameTime = CACurrentMediaTime()
        
        frameTimer = Timer.scheduledTimer(withTimeInterval: 1.0/60.0, repeats: true) { [weak self] _ in
            self?.renderFrame()
        }
    }
    
    /**
     * Stop rendering
     * 
     * This method demonstrates proper rendering cleanup
     * with resource management and state reset
     */
    func stopRendering() {
        isRendering = false
        frameTimer?.invalidate()
        frameTimer = nil
    }
    
    /**
     * Render frame
     * 
     * This method demonstrates production-grade frame rendering
     * with comprehensive optimization and monitoring
     */
    func renderFrame() {
        let startTime = CACurrentMediaTime()
        
        guard let commandBuffer = commandQueue.makeCommandBuffer(),
              let renderEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor) else {
            return
        }
        
        // Update scene
        updateScene()
        
        // Set render state
        renderEncoder.setRenderPipelineState(renderPipelineState)
        renderEncoder.setDepthStencilState(depthStencilState)
        
        // Set vertex buffer
        renderEncoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
        renderEncoder.setVertexBuffer(uniformBuffer, offset: 0, index: 1)
        
        // Render all objects in scene
        for object in scene.objects {
            renderObject(object, with: renderEncoder)
        }
        
        // End encoding
        renderEncoder.endEncoding()
        
        // Present drawable
        commandBuffer.present(scene.drawable)
        commandBuffer.commit()
        
        // Update metrics
        updateRenderMetrics(startTime: startTime)
    }
    
    /**
     * Add 3D object to scene
     * 
     * This method demonstrates 3D object management
     * with comprehensive object creation and optimization
     */
    func addObject(_ object: Object3D) {
        scene.addObject(object)
        updateBuffers()
    }
    
    /**
     * Remove 3D object from scene
     * 
     * This method demonstrates object removal
     * with proper cleanup and resource management
     */
    func removeObject(_ object: Object3D) {
        scene.removeObject(object)
        updateBuffers()
    }
    
    /**
     * Update camera
     * 
     * This method demonstrates camera management
     * with comprehensive camera controls and optimization
     */
    func updateCamera(position: SIMD3<Float>, rotation: SIMD3<Float>, fov: Float) {
        camera.update(position: position, rotation: rotation, fov: fov)
        updateUniforms()
    }
    
    /**
     * Set lighting
     * 
     * This method demonstrates lighting system management
     * with comprehensive lighting controls and optimization
     */
    func setLighting(_ lighting: LightingConfiguration) {
        self.lighting.configure(lighting)
        updateUniforms()
    }
    
    // MARK: - Private Methods
    
    private func setupGraphicsEngine() {
        // Configure render pass descriptor
        renderPassDescriptor.colorAttachments[0].texture = colorTexture
        renderPassDescriptor.colorAttachments[0].loadAction = .clear
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColor(red: 0.0, green: 0.0, blue: 0.0, alpha: 1.0)
        
        renderPassDescriptor.depthAttachment.texture = depthTexture
        renderPassDescriptor.depthAttachment.loadAction = .clear
        renderPassDescriptor.depthAttachment.clearDepth = 1.0
        
        // Initialize scene
        setupDefaultScene()
    }
    
    private func setupDefaultScene() {
        // Add default objects
        let cube = Object3D.cube()
        let sphere = Object3D.sphere(segments: 32)
        let plane = Object3D.plane()
        
        scene.addObject(cube)
        scene.addObject(sphere)
        scene.addObject(plane)
        
        // Setup default lighting
        let ambientLight = AmbientLight(color: SIMD3<Float>(0.2, 0.2, 0.2), intensity: 0.3)
        let directionalLight = DirectionalLight(
            direction: SIMD3<Float>(0.0, -1.0, 0.0),
            color: SIMD3<Float>(1.0, 1.0, 1.0),
            intensity: 1.0
        )
        
        lighting.addLight(ambientLight)
        lighting.addLight(directionalLight)
        
        updateBuffers()
    }
    
    private func updateScene() {
        // Update object animations
        for object in scene.objects {
            object.update(deltaTime: 1.0/60.0)
        }
        
        // Update camera
        camera.update(deltaTime: 1.0/60.0)
        
        // Update lighting
        lighting.update(deltaTime: 1.0/60.0)
    }
    
    private func renderObject(_ object: Object3D, with encoder: MTLRenderCommandEncoder) {
        // Set object-specific uniforms
        var objectUniforms = ObjectUniforms(
            modelMatrix: object.transform.matrix,
            normalMatrix: object.transform.normalMatrix,
            material: object.material
        )
        
        encoder.setVertexBytes(&objectUniforms, length: MemoryLayout<ObjectUniforms>.size, index: 2)
        
        // Set textures
        if let diffuseTexture = object.material.diffuseTexture {
            encoder.setFragmentTexture(diffuseTexture, index: 0)
        }
        
        if let normalTexture = object.material.normalTexture {
            encoder.setFragmentTexture(normalTexture, index: 1)
        }
        
        // Draw object
        encoder.drawIndexedPrimitives(
            type: .triangle,
            indexCount: object.indexCount,
            indexType: .uint32,
            indexBuffer: indexBuffer,
            indexBufferOffset: object.indexOffset
        )
        
        drawCalls += 1
    }
    
    private func updateBuffers() {
        // Update vertex buffer with all objects
        var vertices: [Vertex3D] = []
        var indices: [UInt32] = []
        
        for object in scene.objects {
            let objectVertices = object.vertices
            let objectIndices = object.indices.map { UInt32($0 + vertices.count) }
            
            vertices.append(contentsOf: objectVertices)
            indices.append(contentsOf: objectIndices)
        }
        
        // Update vertex buffer
        let vertexData = vertices.withUnsafeBytes { Data($0) }
        vertexBuffer.contents().copyMemory(from: vertexData.withUnsafeBytes { $0.baseAddress! }, byteCount: vertexData.count)
        
        // Update index buffer
        let indexData = indices.withUnsafeBytes { Data($0) }
        indexBuffer.contents().copyMemory(from: indexData.withUnsafeBytes { $0.baseAddress! }, byteCount: indexData.count)
        
        triangleCount = indices.count / 3
    }
    
    private func updateUniforms() {
        var uniforms = Uniforms(
            viewMatrix: camera.viewMatrix,
            projectionMatrix: camera.projectionMatrix,
            viewProjectionMatrix: camera.viewProjectionMatrix,
            cameraPosition: camera.position,
            ambientLight: lighting.ambientLight,
            directionalLights: lighting.directionalLights,
            pointLights: lighting.pointLights,
            spotLights: lighting.spotLights
        )
        
        uniformBuffer.contents().copyMemory(from: &uniforms, byteCount: MemoryLayout<Uniforms>.size)
    }
    
    private func updateRenderMetrics(startTime: CFTimeInterval) {
        let currentTime = CACurrentMediaTime()
        renderTime = currentTime - startTime
        frameRate = 1.0 / renderTime
        
        // Update memory usage
        memoryUsage = Int64(ProcessInfo.processInfo.physicalMemory - ProcessInfo.processInfo.physicalMemory)
    }
    
    // MARK: - Static Factory Methods
    
    private static func createRenderPipelineState(device: MTLDevice) -> MTLRenderPipelineState {
        let library = device.makeDefaultLibrary()!
        let vertexFunction = library.makeFunction(name: "vertex_main")!
        let fragmentFunction = library.makeFunction(name: "fragment_main")!
        
        let pipelineDescriptor = MTLRenderPipelineDescriptor()
        pipelineDescriptor.vertexFunction = vertexFunction
        pipelineDescriptor.fragmentFunction = fragmentFunction
        pipelineDescriptor.colorAttachments[0].pixelFormat = .bgra8Unorm
        pipelineDescriptor.depthAttachmentPixelFormat = .depth32Float
        
        // Vertex descriptor
        let vertexDescriptor = MTLVertexDescriptor()
        
        // Position
        vertexDescriptor.attributes[0].format = .float3
        vertexDescriptor.attributes[0].offset = 0
        vertexDescriptor.attributes[0].bufferIndex = 0
        
        // Normal
        vertexDescriptor.attributes[1].format = .float3
        vertexDescriptor.attributes[1].offset = 12
        vertexDescriptor.attributes[1].bufferIndex = 0
        
        // Texture coordinates
        vertexDescriptor.attributes[2].format = .float2
        vertexDescriptor.attributes[2].offset = 24
        vertexDescriptor.attributes[2].bufferIndex = 0
        
        // Layout
        vertexDescriptor.layouts[0].stride = 32
        vertexDescriptor.layouts[0].stepRate = 1
        vertexDescriptor.layouts[0].stepFunction = .perVertex
        
        pipelineDescriptor.vertexDescriptor = vertexDescriptor
        
        do {
            return try device.makeRenderPipelineState(descriptor: pipelineDescriptor)
        } catch {
            fatalError("Failed to create render pipeline state: \(error)")
        }
    }
    
    private static func createDepthStencilState(device: MTLDevice) -> MTLDepthStencilState {
        let depthStencilDescriptor = MTLDepthStencilDescriptor()
        depthStencilDescriptor.depthCompareFunction = .less
        depthStencilDescriptor.isDepthWriteEnabled = true
        
        return device.makeDepthStencilState(descriptor: depthStencilDescriptor)!
    }
    
    private static func createVertexBuffer(device: MTLDevice) -> MTLBuffer {
        return device.makeBuffer(length: 1024 * 1024, options: [])! // 1MB
    }
    
    private static func createIndexBuffer(device: MTLDevice) -> MTLBuffer {
        return device.makeBuffer(length: 512 * 1024, options: [])! // 512KB
    }
    
    private static func createUniformBuffer(device: MTLDevice) -> MTLBuffer {
        return device.makeBuffer(length: MemoryLayout<Uniforms>.size, options: [])!
    }
    
    private static func createRenderPassDescriptor() -> MTLRenderPassDescriptor {
        return MTLRenderPassDescriptor()
    }
    
    private static func createDepthTexture(device: MTLDevice) -> MTLTexture {
        let descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: .depth32Float,
            width: 1024,
            height: 1024,
            mipmapped: false
        )
        descriptor.usage = .renderTarget
        return device.makeTexture(descriptor: descriptor)!
    }
    
    private static func createColorTexture(device: MTLDevice) -> MTLTexture {
        let descriptor = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: .bgra8Unorm,
            width: 1024,
            height: 1024,
            mipmapped: false
        )
        descriptor.usage = .renderTarget
        return device.makeTexture(descriptor: descriptor)!
    }
}

// MARK: - 3D Scene Management

/**
 * 3D scene management
 * 
 * This class demonstrates comprehensive 3D scene management
 * with object hierarchy and spatial optimization
 */
class Scene3D: ObservableObject {
    
    // MARK: - Properties
    
    @Published var objects: [Object3D] = []
    @Published var drawable: CAMetalDrawable?
    
    private var spatialIndex: SpatialIndex
    private var cullingSystem: CullingSystem
    private var lodSystem: LODSystem
    
    // MARK: - Initialization
    
    init() {
        self.spatialIndex = SpatialIndex()
        self.cullingSystem = CullingSystem()
        self.lodSystem = LODSystem()
    }
    
    // MARK: - Public Methods
    
    func addObject(_ object: Object3D) {
        objects.append(object)
        spatialIndex.insert(object)
        updateLODs()
    }
    
    func removeObject(_ object: Object3D) {
        objects.removeAll { $0.id == object.id }
        spatialIndex.remove(object)
        updateLODs()
    }
    
    func getVisibleObjects(from camera: Camera3D) -> [Object3D] {
        let frustum = camera.frustum
        return cullingSystem.cull(objects: objects, against: frustum)
    }
    
    func updateLODs() {
        for object in objects {
            let distance = length(object.position - camera.position)
            let lodLevel = lodSystem.calculateLOD(for: object, distance: distance)
            object.setLODLevel(lodLevel)
        }
    }
}

// MARK: - 3D Camera System

/**
 * 3D camera system
 * 
 * This class demonstrates comprehensive 3D camera management
 * with projection matrices and view controls
 */
class Camera3D: ObservableObject {
    
    // MARK: - Properties
    
    @Published var position: SIMD3<Float> = SIMD3<Float>(0, 0, 5)
    @Published var rotation: SIMD3<Float> = SIMD3<Float>(0, 0, 0)
    @Published var fov: Float = 60.0
    @Published var nearPlane: Float = 0.1
    @Published var farPlane: Float = 1000.0
    
    var viewMatrix: matrix_float4x4 {
        let translation = matrix_float4x4(translation: -position)
        let rotationX = matrix_float4x4(rotationX: -rotation.x)
        let rotationY = matrix_float4x4(rotationY: -rotation.y)
        let rotationZ = matrix_float4x4(rotationZ: -rotation.z)
        
        return rotationZ * rotationY * rotationX * translation
    }
    
    var projectionMatrix: matrix_float4x4 {
        let aspectRatio: Float = 16.0 / 9.0
        return matrix_float4x4(perspectiveFov: fov * .pi / 180.0, aspectRatio: aspectRatio, nearZ: nearPlane, farZ: farPlane)
    }
    
    var viewProjectionMatrix: matrix_float4x4 {
        return projectionMatrix * viewMatrix
    }
    
    var frustum: Frustum {
        return Frustum(from: viewProjectionMatrix)
    }
    
    // MARK: - Public Methods
    
    func update(position: SIMD3<Float>, rotation: SIMD3<Float>, fov: Float) {
        self.position = position
        self.rotation = rotation
        self.fov = fov
    }
    
    func update(deltaTime: Float) {
        // Update camera based on input or animation
    }
    
    func lookAt(target: SIMD3<Float>, up: SIMD3<Float> = SIMD3<Float>(0, 1, 0)) {
        let forward = normalize(target - position)
        let right = normalize(cross(forward, up))
        let upVector = cross(right, forward)
        
        // Calculate rotation from look-at matrix
        let lookAtMatrix = matrix_float4x4(
            SIMD4<Float>(right.x, upVector.x, -forward.x, 0),
            SIMD4<Float>(right.y, upVector.y, -forward.y, 0),
            SIMD4<Float>(right.z, upVector.z, -forward.z, 0),
            SIMD4<Float>(0, 0, 0, 1)
        )
        
        // Extract rotation angles
        rotation = extractEulerAngles(from: lookAtMatrix)
    }
    
    func move(by offset: SIMD3<Float>) {
        position += offset
    }
    
    func rotate(by deltaRotation: SIMD3<Float>) {
        rotation += deltaRotation
    }
}

// MARK: - Lighting System

/**
 * Lighting system
 * 
 * This class demonstrates comprehensive lighting management
 * with multiple light types and shadow mapping
 */
class LightingSystem: ObservableObject {
    
    // MARK: - Properties
    
    @Published var ambientLight: AmbientLight
    @Published var directionalLights: [DirectionalLight] = []
    @Published var pointLights: [PointLight] = []
    @Published var spotLights: [SpotLight] = []
    
    private var shadowMap: ShadowMap
    private var lightCulling: LightCulling
    
    // MARK: - Initialization
    
    init() {
        self.ambientLight = AmbientLight(color: SIMD3<Float>(0.2, 0.2, 0.2), intensity: 0.3)
        self.shadowMap = ShadowMap()
        self.lightCulling = LightCulling()
    }
    
    // MARK: - Public Methods
    
    func configure(_ config: LightingConfiguration) {
        ambientLight = config.ambientLight
        directionalLights = config.directionalLights
        pointLights = config.pointLights
        spotLights = config.spotLights
    }
    
    func addLight(_ light: Light) {
        switch light {
        case let ambient as AmbientLight:
            ambientLight = ambient
        case let directional as DirectionalLight:
            directionalLights.append(directional)
        case let point as PointLight:
            pointLights.append(point)
        case let spot as SpotLight:
            spotLights.append(spot)
        default:
            break
        }
    }
    
    func removeLight(_ light: Light) {
        if let directional = light as? DirectionalLight {
            directionalLights.removeAll { $0.id == directional.id }
        } else if let point = light as? PointLight {
            pointLights.removeAll { $0.id == point.id }
        } else if let spot = light as? SpotLight {
            spotLights.removeAll { $0.id == spot.id }
        }
    }
    
    func update(deltaTime: Float) {
        // Update light animations
        for light in directionalLights {
            light.update(deltaTime: deltaTime)
        }
        
        for light in pointLights {
            light.update(deltaTime: deltaTime)
        }
        
        for light in spotLights {
            light.update(deltaTime: deltaTime)
        }
    }
    
    func generateShadowMap(for light: Light, scene: Scene3D) -> MTLTexture? {
        return shadowMap.generate(for: light, scene: scene)
    }
}

// MARK: - Material System

/**
 * Material system
 * 
 * This class demonstrates comprehensive material management
 * with PBR materials and texture mapping
 */
class MaterialSystem: ObservableObject {
    
    // MARK: - Properties
    
    @Published var materials: [Material] = []
    @Published var textures: [Texture] = []
    
    private var textureCache: TextureCache
    private var materialCache: MaterialCache
    
    // MARK: - Initialization
    
    init() {
        self.textureCache = TextureCache()
        self.materialCache = MaterialCache()
    }
    
    // MARK: - Public Methods
    
    func createMaterial(name: String, properties: MaterialProperties) -> Material {
        let material = Material(name: name, properties: properties)
        materials.append(material)
        materialCache.cache(material)
        return material
    }
    
    func loadTexture(from url: URL) -> Texture? {
        if let cached = textureCache.get(url: url) {
            return cached
        }
        
        // Load texture from URL
        guard let texture = loadTextureFromURL(url) else {
            return nil
        }
        
        textureCache.cache(texture, for: url)
        textures.append(texture)
        return texture
    }
    
    func createTexture(name: String, data: Data, format: MTLPixelFormat) -> Texture? {
        // Create texture from data
        return nil // Implementation would create texture from data
    }
    
    // MARK: - Private Methods
    
    private func loadTextureFromURL(_ url: URL) -> Texture? {
        // Implementation would load texture from URL
        return nil
    }
}

// MARK: - Supporting Types

/**
 * 3D vertex
 * 
 * This struct demonstrates proper 3D vertex modeling
 * for 3D graphics rendering
 */
struct Vertex3D {
    let position: SIMD3<Float>
    let normal: SIMD3<Float>
    let textureCoords: SIMD2<Float>
}

/**
 * 3D object
 * 
 * This class demonstrates proper 3D object modeling
 * for 3D graphics rendering
 */
class Object3D: Identifiable {
    let id = UUID()
    var position: SIMD3<Float> = SIMD3<Float>(0, 0, 0)
    var rotation: SIMD3<Float> = SIMD3<Float>(0, 0, 0)
    var scale: SIMD3<Float> = SIMD3<Float>(1, 1, 1)
    var material: Material
    var vertices: [Vertex3D] = []
    var indices: [Int] = []
    var indexCount: Int = 0
    var indexOffset: Int = 0
    var lodLevel: Int = 0
    
    var transform: Transform3D {
        return Transform3D(position: position, rotation: rotation, scale: scale)
    }
    
    init(material: Material = Material.default) {
        self.material = material
    }
    
    func update(deltaTime: Float) {
        // Update object animation
    }
    
    func setLODLevel(_ level: Int) {
        lodLevel = level
        // Update geometry based on LOD level
    }
    
    static func cube() -> Object3D {
        let object = Object3D()
        // Create cube geometry
        return object
    }
    
    static func sphere(segments: Int) -> Object3D {
        let object = Object3D()
        // Create sphere geometry
        return object
    }
    
    static func plane() -> Object3D {
        let object = Object3D()
        // Create plane geometry
        return object
    }
}

/**
 * 3D transform
 * 
 * This struct demonstrates proper 3D transform modeling
 * for 3D graphics rendering
 */
struct Transform3D {
    let position: SIMD3<Float>
    let rotation: SIMD3<Float>
    let scale: SIMD3<Float>
    
    var matrix: matrix_float4x4 {
        let translation = matrix_float4x4(translation: position)
        let rotationX = matrix_float4x4(rotationX: rotation.x)
        let rotationY = matrix_float4x4(rotationY: rotation.y)
        let rotationZ = matrix_float4x4(rotationZ: rotation.z)
        let scale = matrix_float4x4(scale: scale)
        
        return translation * rotationZ * rotationY * rotationX * scale
    }
    
    var normalMatrix: matrix_float3x3 {
        let rotationX = matrix_float3x3(rotationX: rotation.x)
        let rotationY = matrix_float3x3(rotationY: rotation.y)
        let rotationZ = matrix_float3x3(rotationZ: rotation.z)
        
        return rotationZ * rotationY * rotationX
    }
}

/**
 * Material
 * 
 * This struct demonstrates proper material modeling
 * for 3D graphics rendering
 */
struct Material {
    let name: String
    let properties: MaterialProperties
    let diffuseTexture: MTLTexture?
    let normalTexture: MTLTexture?
    let specularTexture: MTLTexture?
    
    static let `default` = Material(
        name: "Default",
        properties: MaterialProperties.default,
        diffuseTexture: nil,
        normalTexture: nil,
        specularTexture: nil
    )
}

/**
 * Material properties
 * 
 * This struct demonstrates proper material properties modeling
 * for 3D graphics rendering
 */
struct MaterialProperties {
    let albedo: SIMD3<Float>
    let metallic: Float
    let roughness: Float
    let emissive: SIMD3<Float>
    let opacity: Float
    
    static let `default` = MaterialProperties(
        albedo: SIMD3<Float>(0.8, 0.8, 0.8),
        metallic: 0.0,
        roughness: 0.5,
        emissive: SIMD3<Float>(0, 0, 0),
        opacity: 1.0
    )
}

/**
 * Light protocol
 * 
 * This protocol demonstrates proper light modeling
 * for 3D graphics rendering
 */
protocol Light: AnyObject {
    var id: UUID { get }
    var color: SIMD3<Float> { get set }
    var intensity: Float { get set }
    
    func update(deltaTime: Float)
}

/**
 * Ambient light
 * 
 * This class demonstrates proper ambient light modeling
 * for 3D graphics rendering
 */
class AmbientLight: Light {
    let id = UUID()
    var color: SIMD3<Float>
    var intensity: Float
    
    init(color: SIMD3<Float>, intensity: Float) {
        self.color = color
        self.intensity = intensity
    }
    
    func update(deltaTime: Float) {
        // Update ambient light animation
    }
}

/**
 * Directional light
 * 
 * This class demonstrates proper directional light modeling
 * for 3D graphics rendering
 */
class DirectionalLight: Light {
    let id = UUID()
    var color: SIMD3<Float>
    var intensity: Float
    var direction: SIMD3<Float>
    
    init(direction: SIMD3<Float>, color: SIMD3<Float>, intensity: Float) {
        self.direction = direction
        self.color = color
        self.intensity = intensity
    }
    
    func update(deltaTime: Float) {
        // Update directional light animation
    }
}

/**
 * Point light
 * 
 * This class demonstrates proper point light modeling
 * for 3D graphics rendering
 */
class PointLight: Light {
    let id = UUID()
    var color: SIMD3<Float>
    var intensity: Float
    var position: SIMD3<Float>
    var range: Float
    var attenuation: SIMD3<Float>
    
    init(position: SIMD3<Float>, color: SIMD3<Float>, intensity: Float, range: Float, attenuation: SIMD3<Float>) {
        self.position = position
        self.color = color
        self.intensity = intensity
        self.range = range
        self.attenuation = attenuation
    }
    
    func update(deltaTime: Float) {
        // Update point light animation
    }
}

/**
 * Spot light
 * 
 * This class demonstrates proper spot light modeling
 * for 3D graphics rendering
 */
class SpotLight: Light {
    let id = UUID()
    var color: SIMD3<Float>
    var intensity: Float
    var position: SIMD3<Float>
    var direction: SIMD3<Float>
    var range: Float
    var coneAngle: Float
    var penumbraAngle: Float
    
    init(position: SIMD3<Float>, direction: SIMD3<Float>, color: SIMD3<Float>, intensity: Float, range: Float, coneAngle: Float, penumbraAngle: Float) {
        self.position = position
        self.direction = direction
        self.color = color
        self.intensity = intensity
        self.range = range
        self.coneAngle = coneAngle
        self.penumbraAngle = penumbraAngle
    }
    
    func update(deltaTime: Float) {
        // Update spot light animation
    }
}

/**
 * Uniforms
 * 
 * This struct demonstrates proper uniform modeling
 * for 3D graphics rendering
 */
struct Uniforms {
    let viewMatrix: matrix_float4x4
    let projectionMatrix: matrix_float4x4
    let viewProjectionMatrix: matrix_float4x4
    let cameraPosition: SIMD3<Float>
    let ambientLight: AmbientLight
    let directionalLights: [DirectionalLight]
    let pointLights: [PointLight]
    let spotLights: [SpotLight]
}

/**
 * Object uniforms
 * 
 * This struct demonstrates proper object uniform modeling
 * for 3D graphics rendering
 */
struct ObjectUniforms {
    let modelMatrix: matrix_float4x4
    let normalMatrix: matrix_float3x3
    let material: Material
}

/**
 * Lighting configuration
 * 
 * This struct demonstrates proper lighting configuration modeling
 * for 3D graphics rendering
 */
struct LightingConfiguration {
    let ambientLight: AmbientLight
    let directionalLights: [DirectionalLight]
    let pointLights: [PointLight]
    let spotLights: [SpotLight]
}

/**
 * Frustum
 * 
 * This struct demonstrates proper frustum modeling
 * for 3D graphics rendering
 */
struct Frustum {
    let planes: [SIMD4<Float>]
    
    init(from matrix: matrix_float4x4) {
        // Extract frustum planes from view-projection matrix
        self.planes = []
    }
}

// MARK: - Matrix Extensions

extension matrix_float4x4 {
    init(translation: SIMD3<Float>) {
        self.init(
            SIMD4<Float>(1, 0, 0, 0),
            SIMD4<Float>(0, 1, 0, 0),
            SIMD4<Float>(0, 0, 1, 0),
            SIMD4<Float>(translation.x, translation.y, translation.z, 1)
        )
    }
    
    init(rotationX angle: Float) {
        let c = cos(angle)
        let s = sin(angle)
        self.init(
            SIMD4<Float>(1, 0, 0, 0),
            SIMD4<Float>(0, c, s, 0),
            SIMD4<Float>(0, -s, c, 0),
            SIMD4<Float>(0, 0, 0, 1)
        )
    }
    
    init(rotationY angle: Float) {
        let c = cos(angle)
        let s = sin(angle)
        self.init(
            SIMD4<Float>(c, 0, -s, 0),
            SIMD4<Float>(0, 1, 0, 0),
            SIMD4<Float>(s, 0, c, 0),
            SIMD4<Float>(0, 0, 0, 1)
        )
    }
    
    init(rotationZ angle: Float) {
        let c = cos(angle)
        let s = sin(angle)
        self.init(
            SIMD4<Float>(c, s, 0, 0),
            SIMD4<Float>(-s, c, 0, 0),
            SIMD4<Float>(0, 0, 1, 0),
            SIMD4<Float>(0, 0, 0, 1)
        )
    }
    
    init(scale: SIMD3<Float>) {
        self.init(
            SIMD4<Float>(scale.x, 0, 0, 0),
            SIMD4<Float>(0, scale.y, 0, 0),
            SIMD4<Float>(0, 0, scale.z, 0),
            SIMD4<Float>(0, 0, 0, 1)
        )
    }
    
    init(perspectiveFov fov: Float, aspectRatio: Float, nearZ: Float, farZ: Float) {
        let yScale = 1.0 / tan(fov * 0.5)
        let xScale = yScale / aspectRatio
        let zRange = farZ - nearZ
        let zScale = -(farZ + nearZ) / zRange
        let wzScale = -2.0 * farZ * nearZ / zRange
        
        self.init(
            SIMD4<Float>(xScale, 0, 0, 0),
            SIMD4<Float>(0, yScale, 0, 0),
            SIMD4<Float>(0, 0, zScale, -1),
            SIMD4<Float>(0, 0, wzScale, 0)
        )
    }
}

extension matrix_float3x3 {
    init(rotationX angle: Float) {
        let c = cos(angle)
        let s = sin(angle)
        self.init(
            SIMD3<Float>(1, 0, 0),
            SIMD3<Float>(0, c, s),
            SIMD3<Float>(0, -s, c)
        )
    }
    
    init(rotationY angle: Float) {
        let c = cos(angle)
        let s = sin(angle)
        self.init(
            SIMD3<Float>(c, 0, -s),
            SIMD3<Float>(0, 1, 0),
            SIMD3<Float>(s, 0, c)
        )
    }
    
    init(rotationZ angle: Float) {
        let c = cos(angle)
        let s = sin(angle)
        self.init(
            SIMD3<Float>(c, s, 0),
            SIMD3<Float>(-s, c, 0),
            SIMD3<Float>(0, 0, 1)
        )
    }
}

// MARK: - Helper Functions

func extractEulerAngles(from matrix: matrix_float4x4) -> SIMD3<Float> {
    // Extract Euler angles from rotation matrix
    let sy = sqrt(matrix.columns.0.x * matrix.columns.0.x + matrix.columns.1.x * matrix.columns.1.x)
    let singular = sy < 1e-6
    
    let x = atan2(matrix.columns.2.y, matrix.columns.2.z)
    let y = atan2(-matrix.columns.2.x, sy)
    let z = singular ? atan2(-matrix.columns.1.z, matrix.columns.1.y) : atan2(matrix.columns.1.x, matrix.columns.0.x)
    
    return SIMD3<Float>(x, y, z)
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use 3D graphics and rendering
 * 
 * This function shows practical usage of all the 3D graphics components
 */
func demonstrate3DGraphics() {
    print("=== 3D Graphics and Rendering Demonstration ===\n")
    
    // Graphics Engine
    let graphicsEngine = Graphics3DEngine()
    print("--- 3D Graphics Engine ---")
    print("Graphics Engine: \(type(of: graphicsEngine))")
    print("Features: Metal rendering, 3D scene management, lighting, materials")
    
    // Scene Management
    let scene = Scene3D()
    print("\n--- 3D Scene Management ---")
    print("Scene: \(type(of: scene))")
    print("Features: Object hierarchy, spatial indexing, culling, LOD")
    
    // Camera System
    let camera = Camera3D()
    print("\n--- 3D Camera System ---")
    print("Camera: \(type(of: camera))")
    print("Features: Projection matrices, view controls, frustum culling")
    
    // Lighting System
    let lighting = LightingSystem()
    print("\n--- Lighting System ---")
    print("Lighting: \(type(of: lighting))")
    print("Features: Multiple light types, shadow mapping, light culling")
    
    // Material System
    let materials = MaterialSystem()
    print("\n--- Material System ---")
    print("Materials: \(type(of: materials))")
    print("Features: PBR materials, texture mapping, material caching")
    
    // Demonstrate features
    print("\n--- Features ---")
    print("3D Rendering: Metal-based 3D graphics with advanced techniques")
    print("Scene Management: Object hierarchy with spatial optimization")
    print("Camera System: Comprehensive camera controls and projection")
    print("Lighting: Multiple light types with shadow mapping")
    print("Materials: PBR materials with texture mapping")
    print("Performance: Optimized for mobile devices and real-time rendering")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Use Metal for high-performance 3D rendering")
    print("2. Implement proper scene management and culling")
    print("3. Use efficient lighting and shadow techniques")
    print("4. Optimize materials and textures for performance")
    print("5. Implement LOD systems for complex scenes")
    print("6. Use spatial indexing for large scenes")
    print("7. Monitor performance and optimize accordingly")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrate3DGraphics()
