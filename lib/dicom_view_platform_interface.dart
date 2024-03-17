import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'dicom_view_method_channel.dart';

abstract class DicomViewPlatform extends PlatformInterface {
  /// Constructs a DicomViewPlatform.
  DicomViewPlatform() : super(token: _token);

  static final Object _token = Object();

  static DicomViewPlatform _instance = MethodChannelDicomViewPlatform();

  /// The default instance of [DicomViewPlatform] to use.
  ///
  /// Defaults to [MethodChannelDicomView].
  static DicomViewPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [DicomViewPlatform] when
  /// they register themselves.
  static set instance(DicomViewPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  Future<MethodChannelDicomView?> createViewChannel() {
    throw UnimplementedError('createView() has not been implemented.');
  }
}
