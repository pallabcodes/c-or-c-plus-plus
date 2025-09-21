/*
 * iOS Frameworks: SwiftUI Development
 * 
 * This file demonstrates production-grade SwiftUI development patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master modern declarative UI development with SwiftUI
 * - Understand custom views and view composition
 * - Implement proper state management patterns
 * - Apply navigation and data flow best practices
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import SwiftUI
import Combine

// MARK: - Custom Views

/**
 * Custom card view with advanced styling and animations
 * 
 * This view demonstrates production-grade SwiftUI component development
 * with proper accessibility support and responsive design
 */
struct CustomCardView<Content: View>: View {
    
    // MARK: - Properties
    
    let content: Content
    let style: CardStyle
    let onTap: (() -> Void)?
    let onLongPress: (() -> Void)?
    
    @State private var isPressed = false
    @State private var dragOffset = CGSize.zero
    
    // MARK: - Initialization
    
    init(
        style: CardStyle = .default,
        onTap: (() -> Void)? = nil,
        onLongPress: (() -> Void)? = nil,
        @ViewBuilder content: () -> Content
    ) {
        self.style = style
        self.onTap = onTap
        self.onLongPress = onLongPress
        self.content = content()
    }
    
    // MARK: - Body
    
    var body: some View {
        content
            .padding(style.padding)
            .background(style.backgroundColor)
            .cornerRadius(style.cornerRadius)
            .shadow(
                color: style.shadowColor,
                radius: style.shadowRadius,
                x: style.shadowOffset.x,
                y: style.shadowOffset.y
            )
            .scaleEffect(isPressed ? 0.95 : 1.0)
            .offset(dragOffset)
            .animation(.spring(response: 0.3, dampingFraction: 0.6), value: isPressed)
            .animation(.spring(response: 0.3, dampingFraction: 0.6), value: dragOffset)
            .onTapGesture {
                onTap?()
            }
            .onLongPressGesture {
                onLongPress?()
            } onPressingChanged: { pressing in
                isPressed = pressing
            }
            .gesture(
                DragGesture()
                    .onChanged { value in
                        dragOffset = value.translation
                    }
                    .onEnded { value in
                        withAnimation(.spring()) {
                            dragOffset = .zero
                        }
                    }
            )
            .accessibilityElement(children: .combine)
            .accessibilityAddTraits(.isButton)
    }
}

/**
 * Card style configuration
 */
struct CardStyle {
    let backgroundColor: Color
    let cornerRadius: CGFloat
    let padding: EdgeInsets
    let shadowColor: Color
    let shadowRadius: CGFloat
    let shadowOffset: CGSize
    
    static let `default` = CardStyle(
        backgroundColor: .white,
        cornerRadius: 12,
        padding: EdgeInsets(top: 16, leading: 16, bottom: 16, trailing: 16),
        shadowColor: .black.opacity(0.1),
        shadowRadius: 8,
        shadowOffset: CGSize(width: 0, height: 2)
    )
    
    static let elevated = CardStyle(
        backgroundColor: .white,
        cornerRadius: 16,
        padding: EdgeInsets(top: 20, leading: 20, bottom: 20, trailing: 20),
        shadowColor: .black.opacity(0.15),
        shadowRadius: 12,
        shadowOffset: CGSize(width: 0, height: 4)
    )
    
    static let minimal = CardStyle(
        backgroundColor: .gray.opacity(0.1),
        cornerRadius: 8,
        padding: EdgeInsets(top: 12, leading: 12, bottom: 12, trailing: 12),
        shadowColor: .clear,
        shadowRadius: 0,
        shadowOffset: .zero
    )
}

// MARK: - Advanced List View

/**
 * Advanced list view with custom cells and animations
 * 
 * This view demonstrates production-grade list implementation
 * with proper data management and user experience
 */
struct AdvancedListView: View {
    
    // MARK: - Properties
    
    @StateObject private var viewModel = ListViewModel()
    @State private var searchText = ""
    @State private var selectedItem: ListItem?
    @State private var showingDetail = false
    
    // MARK: - Body
    
    var body: some View {
        NavigationView {
            VStack(spacing: 0) {
                // Search bar
                SearchBar(text: $searchText)
                    .padding(.horizontal)
                    .padding(.top, 8)
                
                // List content
                if viewModel.isLoading {
                    loadingView
                } else if viewModel.filteredItems.isEmpty {
                    emptyStateView
                } else {
                    listView
                }
            }
            .navigationTitle("Items")
            .navigationBarTitleDisplayMode(.large)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Refresh") {
                        viewModel.refreshData()
                    }
                }
            }
        }
        .onAppear {
            viewModel.loadData()
        }
        .onChange(of: searchText) { newValue in
            viewModel.searchText = newValue
        }
        .sheet(isPresented: $showingDetail) {
            if let selectedItem = selectedItem {
                DetailView(item: selectedItem)
            }
        }
    }
    
    // MARK: - Subviews
    
    private var loadingView: some View {
        VStack(spacing: 16) {
            ProgressView()
                .scaleEffect(1.2)
            Text("Loading items...")
                .foregroundColor(.secondary)
        }
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
    
    private var emptyStateView: some View {
        VStack(spacing: 16) {
            Image(systemName: "list.bullet")
                .font(.system(size: 48))
                .foregroundColor(.secondary)
            
            Text("No items found")
                .font(.title2)
                .fontWeight(.medium)
            
            Text("Try adjusting your search or refresh the list")
                .foregroundColor(.secondary)
                .multilineTextAlignment(.center)
            
            Button("Refresh") {
                viewModel.refreshData()
            }
            .buttonStyle(.borderedProminent)
        }
        .padding()
        .frame(maxWidth: .infinity, maxHeight: .infinity)
    }
    
    private var listView: some View {
        List {
            ForEach(viewModel.filteredItems) { item in
                ListItemView(item: item)
                    .onTapGesture {
                        selectedItem = item
                        showingDetail = true
                    }
                    .swipeActions(edge: .trailing, allowsFullSwipe: false) {
                        Button("Delete", role: .destructive) {
                            viewModel.deleteItem(item)
                        }
                        
                        Button("Favorite") {
                            viewModel.toggleFavorite(item)
                        }
                        .tint(.yellow)
                    }
            }
        }
        .listStyle(PlainListStyle())
        .refreshable {
            await viewModel.refreshDataAsync()
        }
    }
}

// MARK: - List Item View

/**
 * Custom list item view with advanced features
 * 
 * This view demonstrates production-grade list item implementation
 * with proper layout, animation, and user interaction
 */
struct ListItemView: View {
    
    // MARK: - Properties
    
    let item: ListItem
    @State private var isPressed = false
    
    // MARK: - Body
    
    var body: some View {
        CustomCardView(
            style: .default,
            onTap: {
                // Handle tap
            },
            onLongPress: {
                // Handle long press
            }
        ) {
            HStack(spacing: 12) {
                // Thumbnail
                AsyncImage(url: URL(string: item.imageURL)) { image in
                    image
                        .resizable()
                        .aspectRatio(contentMode: .fill)
                } placeholder: {
                    RoundedRectangle(cornerRadius: 8)
                        .fill(Color.gray.opacity(0.3))
                        .overlay {
                            ProgressView()
                        }
                }
                .frame(width: 60, height: 60)
                .clipShape(RoundedRectangle(cornerRadius: 8))
                
                // Content
                VStack(alignment: .leading, spacing: 4) {
                    Text(item.title)
                        .font(.headline)
                        .foregroundColor(.primary)
                        .lineLimit(1)
                    
                    Text(item.subtitle)
                        .font(.subheadline)
                        .foregroundColor(.secondary)
                        .lineLimit(2)
                    
                    HStack {
                        Text(item.category)
                            .font(.caption)
                            .padding(.horizontal, 8)
                            .padding(.vertical, 4)
                            .background(Color.blue.opacity(0.1))
                            .foregroundColor(.blue)
                            .clipShape(Capsule())
                        
                        Spacer()
                        
                        if item.isFavorite {
                            Image(systemName: "heart.fill")
                                .foregroundColor(.red)
                                .font(.caption)
                        }
                    }
                }
                
                Spacer()
                
                // Chevron
                Image(systemName: "chevron.right")
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
        .scaleEffect(isPressed ? 0.98 : 1.0)
        .animation(.easeInOut(duration: 0.1), value: isPressed)
        .onLongPressGesture {
            // Handle long press
        } onPressingChanged: { pressing in
            isPressed = pressing
        }
    }
}

// MARK: - Search Bar

/**
 * Custom search bar with advanced features
 * 
 * This view demonstrates production-grade search implementation
 * with proper focus management and user experience
 */
struct SearchBar: View {
    
    // MARK: - Properties
    
    @Binding var text: String
    @FocusState private var isFocused: Bool
    
    // MARK: - Body
    
    var body: some View {
        HStack {
            Image(systemName: "magnifyingglass")
                .foregroundColor(.secondary)
            
            TextField("Search items...", text: $text)
                .focused($isFocused)
                .textFieldStyle(PlainTextFieldStyle())
            
            if !text.isEmpty {
                Button("Clear") {
                    text = ""
                    isFocused = false
                }
                .foregroundColor(.secondary)
            }
        }
        .padding(.horizontal, 12)
        .padding(.vertical, 8)
        .background(Color.gray.opacity(0.1))
        .cornerRadius(10)
    }
}

// MARK: - Detail View

/**
 * Detail view for displaying item information
 * 
 * This view demonstrates production-grade detail view implementation
 * with proper navigation and user experience
 */
struct DetailView: View {
    
    // MARK: - Properties
    
    let item: ListItem
    @Environment(\.dismiss) private var dismiss
    @State private var isFavorite = false
    
    // MARK: - Body
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(alignment: .leading, spacing: 20) {
                    // Header image
                    AsyncImage(url: URL(string: item.imageURL)) { image in
                        image
                            .resizable()
                            .aspectRatio(contentMode: .fill)
                    } placeholder: {
                        RoundedRectangle(cornerRadius: 12)
                            .fill(Color.gray.opacity(0.3))
                            .overlay {
                                ProgressView()
                            }
                    }
                    .frame(height: 200)
                    .clipShape(RoundedRectangle(cornerRadius: 12))
                    
                    // Content
                    VStack(alignment: .leading, spacing: 16) {
                        // Title and favorite button
                        HStack {
                            Text(item.title)
                                .font(.largeTitle)
                                .fontWeight(.bold)
                            
                            Spacer()
                            
                            Button {
                                isFavorite.toggle()
                            } label: {
                                Image(systemName: isFavorite ? "heart.fill" : "heart")
                                    .font(.title2)
                                    .foregroundColor(isFavorite ? .red : .secondary)
                            }
                        }
                        
                        // Category
                        Text(item.category)
                            .font(.headline)
                            .padding(.horizontal, 12)
                            .padding(.vertical, 6)
                            .background(Color.blue.opacity(0.1))
                            .foregroundColor(.blue)
                            .clipShape(Capsule())
                        
                        // Description
                        Text(item.subtitle)
                            .font(.body)
                            .foregroundColor(.secondary)
                        
                        // Additional details
                        VStack(alignment: .leading, spacing: 8) {
                            DetailRow(title: "ID", value: item.id.uuidString)
                            DetailRow(title: "Created", value: item.createdAt.formatted())
                            DetailRow(title: "Status", value: item.isActive ? "Active" : "Inactive")
                        }
                        .padding()
                        .background(Color.gray.opacity(0.1))
                        .cornerRadius(8)
                    }
                    .padding(.horizontal)
                }
            }
            .navigationTitle("Details")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Done") {
                        dismiss()
                    }
                }
            }
        }
        .onAppear {
            isFavorite = item.isFavorite
        }
    }
}

// MARK: - Detail Row

/**
 * Detail row for displaying key-value pairs
 * 
 * This view demonstrates reusable component design
 * with proper layout and styling
 */
struct DetailRow: View {
    
    // MARK: - Properties
    
    let title: String
    let value: String
    
    // MARK: - Body
    
    var body: some View {
        HStack {
            Text(title)
                .font(.subheadline)
                .fontWeight(.medium)
                .foregroundColor(.secondary)
            
            Spacer()
            
            Text(value)
                .font(.subheadline)
                .foregroundColor(.primary)
        }
    }
}

// MARK: - View Models

/**
 * List view model for managing list state
 * 
 * This class demonstrates proper view model implementation
 * with reactive programming and state management
 */
class ListViewModel: ObservableObject {
    
    // MARK: - Published Properties
    
    @Published var items: [ListItem] = []
    @Published var isLoading = false
    @Published var searchText = ""
    
    // MARK: - Computed Properties
    
    var filteredItems: [ListItem] {
        if searchText.isEmpty {
            return items
        } else {
            return items.filter { item in
                item.title.localizedCaseInsensitiveContains(searchText) ||
                item.subtitle.localizedCaseInsensitiveContains(searchText) ||
                item.category.localizedCaseInsensitiveContains(searchText)
            }
        }
    }
    
    // MARK: - Methods
    
    func loadData() {
        isLoading = true
        
        // Simulate network request
        DispatchQueue.global(qos: .userInitiated).async { [weak self] in
            Thread.sleep(forTimeInterval: 1.0)
            
            let mockItems = self?.generateMockData() ?? []
            
            DispatchQueue.main.async {
                self?.items = mockItems
                self?.isLoading = false
            }
        }
    }
    
    func refreshData() {
        loadData()
    }
    
    func refreshDataAsync() async {
        await withCheckedContinuation { continuation in
            loadData()
            
            // Simulate completion
            DispatchQueue.main.asyncAfter(deadline: .now() + 1.0) {
                continuation.resume()
            }
        }
    }
    
    func deleteItem(_ item: ListItem) {
        items.removeAll { $0.id == item.id }
    }
    
    func toggleFavorite(_ item: ListItem) {
        if let index = items.firstIndex(where: { $0.id == item.id }) {
            items[index].isFavorite.toggle()
        }
    }
    
    private func generateMockData() -> [ListItem] {
        return (1...50).map { index in
            ListItem(
                id: UUID(),
                title: "Item \(index)",
                subtitle: "This is a detailed description for item \(index) that provides more context about what this item represents and its purpose.",
                category: ["Technology", "Design", "Business", "Science"].randomElement() ?? "General",
                imageURL: "https://picsum.photos/200/200?random=\(index)",
                isFavorite: Bool.random(),
                isActive: Bool.random(),
                createdAt: Date().addingTimeInterval(-Double.random(in: 0...86400 * 30))
            )
        }
    }
}

// MARK: - Supporting Types

/**
 * List item model for demonstration
 */
struct ListItem: Identifiable, Equatable {
    let id: UUID
    let title: String
    let subtitle: String
    let category: String
    let imageURL: String
    var isFavorite: Bool
    let isActive: Bool
    let createdAt: Date
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use SwiftUI development patterns
 * 
 * This function shows practical usage of all the SwiftUI components
 */
func demonstrateSwiftUIDevelopment() {
    print("=== SwiftUI Development Demonstration ===\n")
    
    // Create custom card view
    let cardView = CustomCardView(style: .default) {
        VStack {
            Text("Sample Card")
                .font(.headline)
            Text("This is a sample card content")
                .font(.subheadline)
                .foregroundColor(.secondary)
        }
    }
    
    print("--- Custom Views ---")
    print("Custom card view: \(type(of: cardView))")
    print("Card styles available: default, elevated, minimal")
    
    // Create advanced list view
    let listView = AdvancedListView()
    print("\n--- Advanced List View ---")
    print("List view: \(type(of: listView))")
    print("Features: Search, pull-to-refresh, swipe actions")
    
    // Create list view model
    let viewModel = ListViewModel()
    print("\n--- View Model ---")
    print("View model: \(type(of: viewModel))")
    print("Published properties: items, isLoading, searchText")
    
    // Demonstrate state management
    print("\n--- State Management ---")
    print("@State: Local view state")
    print("@StateObject: View model lifecycle")
    print("@Published: Reactive properties")
    print("@Binding: Two-way data binding")
    
    // Demonstrate navigation
    print("\n--- Navigation ---")
    print("NavigationView: Root navigation container")
    print("NavigationLink: Programmatic navigation")
    print("Sheet: Modal presentation")
    print("Toolbar: Navigation bar customization")
    
    // Demonstrate animations
    print("\n--- Animations ---")
    print("Spring animations: Natural motion")
    print("Ease animations: Smooth transitions")
    print("Custom animations: Tailored effects")
    print("Gesture animations: Interactive feedback")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateSwiftUIDevelopment()
