import 'package:flutter/foundation.dart';
import 'package:flutter/widgets.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:dicom_view/dicom_view.dart';
import 'package:dicom_view/dicom_view_platform_interface.dart';
import 'package:dicom_view/dicom_view_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockDicomViewPlatform
    with MockPlatformInterfaceMixin
    implements DicomViewPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');

  @override
  Future<MethodChannelDicomView?> createViewChannel() =>
      Future.value(MethodChannelDicomView(0));
}

void main() {
  final DicomViewPlatform initialPlatform = DicomViewPlatform.instance;

  test('$MethodChannelDicomViewPlatform is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelDicomViewPlatform>());
  });

  test('getPlatformVersion', () async {
    DicomViewPlugin dicomViewPlugin = DicomViewPlugin();
    MockDicomViewPlatform fakePlatform = MockDicomViewPlatform();
    DicomViewPlatform.instance = fakePlatform;

    expect(await dicomViewPlugin.getPlatformVersion(), '42');
  });

  test('createView', () async {
    DicomViewPlugin dicomViewPlugin = DicomViewPlugin();
    MockDicomViewPlatform fakePlatform = MockDicomViewPlatform();
    DicomViewPlatform.instance = fakePlatform;

    expect(await dicomViewPlugin.createView(),
        isInstanceOf<MethodChannelDicomView>());
  });

  testWidgets('DicomView load file', (tester) async {
    debugDefaultTargetPlatformOverride = TargetPlatform.linux;
    // Create the widget by telling the tester to build it.
    await tester.pumpWidget(DicomView(
      onDicomViewCreated: (DicomViewController controller) {
        controller.setFile(file: 'test.dcm');
      },
    ));

    tester.pumpAndSettle();
    debugDumpApp();
    debugDefaultTargetPlatformOverride = null;
  });
}
