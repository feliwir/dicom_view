import 'package:dicom_view/dicom_view_method_channel.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/gestures.dart';
import 'package:flutter/services.dart';
import 'package:flutter/widgets.dart';

import 'dicom_view_linux.dart';
import 'dicom_view_platform_interface.dart';

class DicomViewPlugin {
  Future<String?> getPlatformVersion() {
    return DicomViewPlatform.instance.getPlatformVersion();
  }

  Future<MethodChannelDicomView?> createView() {
    return DicomViewPlatform.instance.createViewChannel();
  }
}

typedef DicomViewCreatedCallback = void Function(
    DicomViewController controller);

class DicomView extends StatelessWidget {
  final DicomViewCreatedCallback onDicomViewCreated;
  const DicomView({super.key, required this.onDicomViewCreated});

  @override
  Widget build(BuildContext context) {
    switch (defaultTargetPlatform) {
      case TargetPlatform.android:
        return Directionality(
            textDirection: TextDirection.ltr,
            child: AndroidView(
              viewType: 'plugins.dicom_view/dicom_view',
              onPlatformViewCreated: _onPlatformViewCreated,
              gestureRecognizers: const <Factory<
                  OneSequenceGestureRecognizer>>{},
            ));
      case TargetPlatform.iOS:
        return UiKitView(
          viewType: 'plugins.dicom_view/dicom_view',
          onPlatformViewCreated: _onPlatformViewCreated,
          gestureRecognizers: const <Factory<OneSequenceGestureRecognizer>>{},
        );
      case TargetPlatform.linux:
        return DicomViewLinux();
      default:
        throw UnsupportedError("Unsupported platform view");
    }
  }

  // Callback method when platform view is created
  void _onPlatformViewCreated(int id) =>
      onDicomViewCreated(DicomViewController._(id));
}

class DicomViewController {
  final MethodChannel _channel;

  DicomViewController._(int id)
      : _channel = MethodChannel('plugins.dicom_view/dicom_view_$id');

  Future<void> setFile({required String file}) async {
    return _channel.invokeMethod('setFile', file);
  }
}
