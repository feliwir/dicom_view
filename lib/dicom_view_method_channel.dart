import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'dicom_view_platform_interface.dart';

/// An implementation of [DicomViewPlatform] that uses method channels.
class MethodChannelDicomViewPlatform extends DicomViewPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final MethodChannel methodChannel = const MethodChannel('dicom_view');

  @override
  Future<String?> getPlatformVersion() async {
    final version =
        await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<int?> createViewChannel() async {
    final viewId = await methodChannel.invokeMethod<int>('createView');
    return viewId;
  }
}

class MethodChannelDicomView {
  final int id;

  /// The method channel used to interact with the native platform.
  @visibleForTesting
  late MethodChannel methodChannel;

  MethodChannelDicomView(this.id) {
    methodChannel = MethodChannel('dicom_view_$id');
  }

  Future<int?> getTextureId() async {
    final viewId = await methodChannel.invokeMethod<int>('getTextureId');
    return viewId;
  }

  Future<void> setDicomFile({required String file}) async {
    await methodChannel.invokeMethod<void>('setDicomFile', <String, dynamic>{
      'file': file,
    });
  }
}
