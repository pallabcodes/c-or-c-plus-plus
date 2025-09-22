# CI/CD for Objective-C Projects

- Use GitHub Actions macOS runners (Xcode pre-installed)
- Build matrix for iOS versions/devices as needed
- fastlane lanes for build, test, code signing, TestFlight
- Cache DerivedData to speed up builds

Example (GitHub Actions):

```yaml
name: iOS CI
on: [push, pull_request]
jobs:
  build:
    runs-on: macos-14
    steps:
      - uses: actions/checkout@v4
      - name: Select Xcode
        run: sudo xcode-select -s "/Applications/Xcode_15.4.app"
      - name: Build & Test
        run: xcodebuild -scheme App -sdk iphonesimulator -destination 'platform=iOS Simulator,name=iPhone 15' test | xcpretty
```
