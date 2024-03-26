#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint dicom_view.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  # Execute CMake build
  system ("cmake -G Xcode -B build -S .")

  s.name             = 'dicom_view'
  s.version          = '0.0.1'
  s.summary          = 'A new Flutter plugin project.'
  s.description      = <<-DESC
A new Flutter plugin project.
                       DESC
  s.homepage         = 'https://mbits.info'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'mbits imaging GmbH' => 'vedder@mbits.info' }

  s.source           = { :path => '.' }
  s.source_files     = 'Classes/**/*'
  s.dependency 'FlutterMacOS'

  s.platform = :osx, '10.11'
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
  s.swift_version = '5.0'
end
